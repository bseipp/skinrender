#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "dart_throwing/PDSampling.h"
#include "vec2.h"
#include <thread>

using namespace std;

const int ARM_LENGTH = 5;
const int ARM_RADIUS = 1;
const int CHANNELS = 6;
const double PI = 3.14159265;

//Thickness in micrometers for different internal densities
const float BONE_DISTANCE_FROM_CENTER = 100;
const float MUSCLE_DISTANCE_FROM_CENTER = 264;
const float HYPERDERMIS_DISTANCE_FROM_CENTER = 300;
const float DERMIS_DISTANCE_FROM_CENTER = 311;
const float EPIDERMIS_DISTANCE_FROM_CENTER = 313;

enum Layer {NOT_IN_ARM, EPIDERMIS, DERMIS, HYPERDERMIS, MUSCLE, BONE };
enum Channel { TRANSMITANCE_R, TRANSMITANCE_G, TRANSMITANCE_B, ALBIEDO_R, ALBIEDO_G, ALBIEDO_B };

const int MUSCLE_NOISE_CYCLES = 60;
const int HYPERDERMIS_NOISE_CYCLES = 1;
const int DERMIS_NOISE_CYCLES = 60;

char* grid;

/*******************
 *******************
 * Hair functions: *
 *******************
 *******************/
string addHair(float hairy_factor){
    float theta, theta_prime, z;
    PDSampler *sampler;
    float points[4][3];
    string hairGeometry = "";
    
    cout << "Placing hairs..." << endl;

    sampler = new PureSampler(hairy_factor);
    sampler->complete();
    cout << endl;
    
    /* For each point, convert to cylindrical coordinates */
    for(int i = 0; i < (int) sampler->points.size(); i++){
        cout << "\rBuilding hair curve: " << i + 1;

        hairGeometry += "\tShape \"curve\" \"string type\" [ \"cylinder\" ] \"point P\" [ ";
        
        /*************************************************************************
         * Sampler creates values between -1 and 1.
         * The x component is then scaled by PI to get a theta.
         * The y component is used for z in the cooresponding cylindrical coord.
         * This is for the first set of the arm from z = 0 -> z = 2
         *************************************************************************/
        
        z = sampler->points[i].y + 1;
        theta = sampler->points[i].x * PI;
        points[0][0] = (ARM_RADIUS - .05) * cos(theta);
        points[0][1] = (ARM_RADIUS - .05) * sin(theta);
        points[0][2] = z;
        
        //NIH Visible human - Possible source for model
        //NIH has man and women
        //3D volume dataset where imaged cross sections of the entire skin set
        //Someone may have made 3d geometric models for the body based off this, find out
        //Cryosection MRI and CT data NIH Geometric model?
        
        z += .1;
        points[1][0] = (ARM_RADIUS + .1) * cos(theta);
        points[1][1] = (ARM_RADIUS + .1) * sin(theta);
        points[1][2] = z;
        
        z += .1;
        theta_prime = theta + (.01 * PI);
        points[2][0] = (ARM_RADIUS + .1) * cos(theta_prime);
        points[2][1] = (ARM_RADIUS + .1) * sin(theta_prime);
        points[2][2] = z;

        z += .1;
        points[3][0] = (ARM_RADIUS + .1) * cos(theta);
        points[3][1] = (ARM_RADIUS + .1) * sin(theta);
        points[3][2] = z;
        
        for (int j = 0; j < 4; j++){
            hairGeometry += to_string(points[j][0]) + " " + to_string(points[j][1]) + " " + to_string(points[j][2]) + " ";
        }
        
        hairGeometry += "] \"float width0\" [ 0.004972 ] \"float width1\" [ 0.004278 ]\n\n";
        
        
        hairGeometry += "\tShape \"curve\" \"string type\" [ \"cylinder\" ] \"point P\" [ ";
        
        /*************************************************************************
         * Build a second set of points, for the bottom half of the arm
         * From z = -2 -> z = 0
         *************************************************************************/
        
        for (int j = 0; j < 4; j++){
            hairGeometry += to_string(points[j][0]) + " " + to_string(points[j][1]) + " " + to_string(points[j][2] - 2) + " ";
        }
        
        hairGeometry += "] \"float width0\" [ 0.004972 ] \"float width1\" [ 0.004278 ]\n\n";
        
    }
    return hairGeometry;
}

/********************
 ********************
 * Noise functions: *
 ********************
 ********************/
//Mod the hash value inputs by the bumpy factor
unsigned int hashValue(int x, int y) {
    x = x * 1664525u + 1013904223u;
    y = y * 1664525u + 1013904223u;
    int z = 1013904223u;
    
    x += y * z;
    y += z * x;
    z += x * y;
    x += y * z;
    
    return x >> 16u;
}

// smooth fade from 1 to 0 as t goes from 0 to 1
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// linear interpolation between a and b
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// convert low two bits of hash code to gradient
float grad(unsigned int hashv, float x, float y) {
    return ((hashv & 1) ? x : -x) + ((hashv & 2) ? y : -y);
}

// 2D noise function
float noise2(vec2f v) {
    // split v into integer and fractional parts
    vec2f vi = vec2f(floor(v.x), floor(v.y));
    vec2f vf = v - vi;
    
    // smooth blend curve
    float fx = fade(vf.x), fy = fade(vf.y);
    
    // blend results from four corners of square
    return lerp(lerp(grad(hashValue(int(vi.x)  , int(vi.y))  , vf.x  , vf.y  ),
                     grad(hashValue(int(vi.x+1), int(vi.y  )), vf.x-1, vf.y  ),
                     fx),
                lerp(grad(hashValue(int(vi.x  ), int(vi.y+1)), vf.x  , vf.y-1),
                     grad(hashValue(int(vi.x+1), int(vi.y+1)), vf.x-1, vf.y-1),
                     fx),
                fy);
}

float calculateNoise(vec2f uv, int octives, int scale) {
    float z = 0.0;
    for (int i = 1; i < octives; i*=2){
        z += noise2(uv * (float)i) / i;
    }
    
    return z * scale;
}

/**********************
 **********************
 * Density functions: *
 **********************
 **********************/
int convertToUM(int l){
    return l * 626;
}

char classify(float d, vec2f uv){
    //calculate noise values for given point
    float bone_noise = 0;
    float muscle_noise = calculateNoise(vec2f(uv.x / MUSCLE_NOISE_CYCLES, uv.y), 4, 10);
    float hyperdermis_noise = calculateNoise(vec2f(uv.x / HYPERDERMIS_NOISE_CYCLES, uv.y), 4, 20);
    float dermis_noise = calculateNoise(vec2f(uv.x / DERMIS_NOISE_CYCLES, uv.y), 4, 5);
    float epidermis_noise = 0;
    
    if (hyperdermis_noise > 8)
        hyperdermis_noise = 8;
    
    if (dermis_noise > 1)
        dermis_noise = 1;
    
    //Calculate the distances for each layer as determined by noise function
    if (d < BONE_DISTANCE_FROM_CENTER + bone_noise){
        return '5';
    }
    
    if (d < MUSCLE_DISTANCE_FROM_CENTER + muscle_noise){
        return '4';
    }
    
    if (d < HYPERDERMIS_DISTANCE_FROM_CENTER + hyperdermis_noise){
        return '3';
    }
    
    if (d < DERMIS_DISTANCE_FROM_CENTER + dermis_noise){
        return '2';
    }
    
    if (d < EPIDERMIS_DISTANCE_FROM_CENTER + epidermis_noise){
        return '1';
    }
    
    return '0';
}

void buildDensityModel(int offset){
    int arm_length = convertToUM(ARM_LENGTH);
    int arm_radius = convertToUM(ARM_RADIUS);
    
    vec2f center = vec2f(arm_radius / 2.0, arm_radius / 2.0);

    //Iterate over each point on the grid
    int max = (arm_length > offset + 500 ? offset + 500 : arm_length);
    
    float SCALE = 900 / PI;
    string noiseData = "";
    int memory_offset = offset * arm_radius * arm_radius;
    fprintf(stderr, "Building Density Model for %d to %d\n", offset, max);
    for(int z = offset; z < max; z++) {
        for(int x = 0; x < arm_radius; x++){
            for(int y = 0; y < arm_radius; y++){
                vec2f uv = vec2f(atan2(x - center.x, y - center.y) * 180 / PI, z / SCALE);
                
                float distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));
                
                grid[memory_offset] = classify(distance, uv);
                memory_offset++;
            }
        }
    }
}

void generateVolumeModel(int threadCount){
    int arm_length = convertToUM(ARM_LENGTH);
    int arm_radius = convertToUM(ARM_RADIUS);
    unsigned int arraySize = arm_length * arm_radius * arm_radius;
    try {
        grid = new char[arraySize];
    } catch (std::bad_alloc&) {
        cout << endl << "Bad alloc" << endl;
        exit(0);
    }
    
    fprintf(stderr, "Building density volume.\n");
    thread threads[threadCount];
    for (int i = 1; i < threadCount; i++){
        threads[i] = thread(buildDensityModel, i * 500);
    }
    threads[0] = thread(buildDensityModel, 0);
    for (int i = 0; i < threadCount; i++){
        threads[i].join();
    }
    
    fprintf(stderr, "Writing density structure file.\n");
    ofstream densityFormatFile;
    densityFormatFile.open("density.txt");
    densityFormatFile << "File: density.raw\n";
    densityFormatFile << "x: " + to_string(arm_radius) + "\n";
    densityFormatFile << "y: " + to_string(arm_radius) + "\n";
    densityFormatFile << "z: " + to_string(arm_length) + "\n";
    densityFormatFile << "type: char\n";
    densityFormatFile.close();
    
    fprintf(stderr, "Writing density volume file.\n");
    FILE* fd = fopen("density.raw", "wb");
    fwrite(&grid[0], sizeof(char), arm_length * arm_radius * arm_radius * CHANNELS, fd);
    fclose(fd);
}

/*****************************
 *****************************
 * PBRT Generator functions: *
 *****************************
 *****************************/
void renderMenu(string file){
    char renderChoice;
    string cmd = "./pbrt ";
    
renderInput:
    cout << endl << "Render after generation? Y/N" << endl;
    cin >> renderChoice;
    if (renderChoice == 'Y' || renderChoice == 'y'){
        cmd += file + ".pbrt";
        system(cmd.c_str());
    } else if (renderChoice == 'N' || renderChoice == 'n'){
        return;
    } else {
        goto renderInput;
    }
}

void volumeMenu(int threads){
    char generateChoice;
    
volumeGenerateInput:
    cout << endl << "\nGenerate volume? Y/N" << endl;
    cin >> generateChoice;
    if (generateChoice == 'Y' || generateChoice == 'y'){
        generateVolumeModel(threads);
    } else if (generateChoice == 'N' || generateChoice == 'n'){
        return;
    } else {
        goto volumeGenerateInput;
    }
}

void run(string filename);

void inputFile(){
    string filename = " ";
    cout << "Enter a file to render:" << endl;
    cin >> filename;
    if (filename.compare(" ") == 0){
        inputFile();
        return;
    }
    run(filename);
}

string generateArmScene(string propertiesFileName, int volumeX, int volumeY, int volumeZ){
    string armScene = "";
    
    armScene += "##############\n# Create Arm #\n##############\n";
 
    //create the medium
    armScene += "MakeNamedMedium \"smoke\" \"string type\" \"skin_heterogeneous\" \"integer trans_x\" " + to_string(volumeX) + " \"integer trans_y\" " + to_string(volumeY) + " \"integer trans_z\" " + to_string(volumeZ) + "";
    armScene += "\t\"integer scat_x\" " + to_string(volumeX) + " \"integer scat_y\" " + to_string(volumeY) + " \"integer scat_z\" " + to_string(volumeZ) + "\n";
    armScene += "\t\"point p0\" [ -0.999999 -0.800000 -0.840000 ] \"point p1\" [ 2.700000 2.490000 0.890000 ]\n";
    armScene += "\t\"string density_file\" [\"geometry/density.raw\"]\n";
    armScene += "\t\"string volumetric_colors\" [\"geometry/density.raw\"]\n\n";
    
    //Create the material
    armScene += "AttributeBegin\n";
    armScene += "\tTexture \"brianskin\" \"color\" \"imagemap\"\n";
    armScene += "\t\t\"string filename\" [\"brian.png\"]\n\n";
    armScene +="\tMediumInterface \"smoke\" \"\"\n";
    armScene +="\tMaterial \"skin\" \"texture Kd\" \"brianskin\"\n";
    armScene += "\t\t\"float eta\" [1.33] \"color mfp\" [1.2953e-03 9.5238e-04 6.7114e-04]\n";
    
    //loop to fetch properties
    armScene += "\t\t\"string t1\" [\"";
    armScene += "0.42 0.71 0.42 0.93 0.42 0.42 0.93 0.71 0.71 0.42 0.42 0.42 0.42 0.93 0.42 0.93 0.42 0.71 0.42 0.42 0.93 0.42 0.42 0.42 0.71 0.93 0.71 0.42 0.42 0.42";
    armScene += "\"]\n";
    
    armScene += "\t\t\"float asr\" 130 \"float asg\" 80 \"float asb\" 180\n";
    armScene += "\t\t\"float uroughness\" [0.05] \"float vroughness\" [0.05]\n";
    armScene += "\t\t\"bool remaproughness\" [\"false\"]";
    
    armScene += "\tShape \"cylinder\" \"float radius\" " + to_string(ARM_RADIUS) + "\n";
    armScene += "\t\t\"float zmin\" -" + to_string(float(ARM_LENGTH) / 2) + "\n";
    armScene += "\t\t\"float zmax\" " + to_string(float(ARM_LENGTH) / 2) + "\n";
    armScene += "\t\t\"float phimax\" 360\n";
    armScene += "AttributeEnd\n\n\n";
    
    return armScene;
}

string generateRoomScene(int length, int width, int height){
    string roomScene = "";
    
    roomScene += "###############\n# Create Room #\n###############\n";
    roomScene += "AttributeBegin\n\tTranslate 0 0 -" + to_string(width) + "\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ -" + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " " + to_string(width) + " 0   -" + to_string(width) + " " + to_string(width) + " 0 ]\n";
    roomScene += "AttributeEnd\n\n";

    roomScene += "AttributeBegin\n\tTranslate 0 0 " + to_string(width) + "\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ -" + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " " + to_string(width) + " 0   -" + to_string(width) + " " + to_string(width) + " 0 ]\n";
    roomScene += "AttributeEnd\n\n";

    roomScene += "AttributeBegin\n\tTranslate " + to_string(height) + " 0 0\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ 0 -" + to_string(height) + " -" + to_string(height) + "   0 " + to_string(height) + " -" + to_string(height) + "  0 " + to_string(height) + " " + to_string(height) + "   0 -" + to_string(height) + " " + to_string(height) + " ]\n";
    roomScene += "AttributeEnd\n\n";

    roomScene += "AttributeBegin\n\tTranslate -" + to_string(height) + " 0 0\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ 0 -" + to_string(height) + " -" + to_string(height) + "   0 " + to_string(height) + " -" + to_string(height) + "  0 " + to_string(height) + " " + to_string(height) + "   0 -" + to_string(height) + " " + to_string(height) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n\tTranslate 0 -" + to_string(length) + " 0\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ -" + to_string(length) + " 0 -" + to_string(length) + "   -" + to_string(length) + " 0 " + to_string(length) + "  " + to_string(length) + " 0 " + to_string(length) + "   " + to_string(length) + " 0 -" + to_string(length) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n\tTranslate 0 " + to_string(length) + " 0\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    roomScene += "\tShape \"trianglemesh\"\n";
    roomScene += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\t\"point P\" [ -" + to_string(length) + " 0 -" + to_string(length) + "   -" + to_string(length) + " 0 " + to_string(length) + "  " + to_string(length) + " 0 " + to_string(length) + "   " + to_string(length) + " 0 -" + to_string(length) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    return roomScene;
}

string generateLighting(int height, int lIntensity){
    string sceneLighting = "";
    
    sceneLighting += "################\n# Create Light #\n################\n";
    sceneLighting += "AttributeBegin\n";
    sceneLighting += "\tAreaLightSource \"diffuse\" \"blackbody L\" [ 4000 " + to_string(lIntensity) + " ]\n";
    sceneLighting += "\tTranslate -" + to_string(height) + " 0 0\n";
    sceneLighting += "\tShape \"trianglemesh\"";
    sceneLighting += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    sceneLighting += "\t\t\"point P\" [ 0 -2 -2   0 2 -2  0 2 2   0 -2 2 ]\n";
    sceneLighting += "AttributeEnd\n\n";
    sceneLighting += "WorldEnd";
    
    return sceneLighting;
}

string generateHair(string hair_color, float poisson_radius){
    string sceneHair = "";
    
    sceneHair += "\tMakeNamedMaterial  \"black_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 8 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"red_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 3 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"brown_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 1.3 ] \"float beta_m\" .25 \"float alpha\" 2\n";
    sceneHair += "\tMakeNamedMaterial  \"blonde_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ .3 ]\n";
    
    sceneHair += "\n\tNamedMaterial \"" + hair_color + "_hair\"\n\n";
    sceneHair += addHair(poisson_radius);
    
    return sceneHair;
}

string generateView(int distance, int fov, int rays, int iWidth, int iHeight, string iFilename){
    string sceneView = "";
    
    sceneView += "###############\n# Create View #\n###############\n";
    sceneView += "LookAt 0 -" + to_string(distance) + " 0 #eye\n";
    sceneView += "\t 0 0 0 #look at point\n\t0 0 1 #up vector\n";
    sceneView += "Camera \"perspective\" \"float fov\" " + to_string(fov) + "\n";
    sceneView += "Sampler \"halton\" \"integer pixelsamples\" " + to_string(rays) + "\n";
    sceneView += "Integrator \"path\"\nFilm \"image\" \"string filename\" \"" + iFilename + ".exr\"\n";
    sceneView += "\"integer xresolution\" [" + to_string(iWidth) + "] \"integer yresolution\" [" + to_string(iHeight) + "]\n\n";
    sceneView += "WorldBegin\nRotate 45 1 0 0\nRotate 90 0 1 0\nActiveTransform All\n\n";
    
    return sceneView;
}

void run(string filename){
    //Read the input file and parse values out
    ifstream file;
    file.open(filename);
    if (file.fail()) { cout << "Bad file name, try again." << endl; inputFile();}
    int length = 10, width = 10, height = 10, distance = 9, fov = 60, rays = 128, iWidth = 400, iHeight = 400, lIntensity = 20, threads = 1, volumeX = 100, volumeY = 100, volumeZ = 40;
    string iFilename = "temp", line, property, value, hair_color = "brown", propertiesFile = "properties.txt";

    float poisson_radius = 0;
    
    while(getline(file, line)){
        property = line.substr(0, line.find(" "));
        line.erase(0, line.find(" ") + 1);
        value = line;
        if (property.compare("length_of_room") == 0){
            length = stoi(value, nullptr, 10);
        } else if (property.compare("width_of_room") == 0){
            width = stoi(value, nullptr, 10);
        } else if (property.compare("height_of_room") == 0){
            height = stoi(value, nullptr, 10);
        } else if (property.compare("distance_from_object") == 0){
            distance = stoi(value, nullptr, 10);
        } else if (property.compare("field_of_view") == 0){
            fov = stoi(value, nullptr, 10);
        } else if (property.compare("rays_per_pixel") == 0){
            rays = stoi(value, nullptr, 10);
        } else if (property.compare("image_width") == 0){
            iWidth = stoi(value, nullptr, 10);
        } else if (property.compare("image_height") == 0){
            iHeight = stoi(value, nullptr, 10);
        } else if (property.compare("image_filename") == 0){
            iFilename = value;
        } else if (property.compare("light_intensity") == 0){
            lIntensity = stoi(value, nullptr, 10);
        } else if (property.compare("hair_density") == 0){
            float hair_scalar = stof(value, nullptr);
            if (hair_scalar < .025){
                hair_scalar = .025;
            } else if (hair_scalar > 10.0) {
                hair_scalar = 10.0;
            }
            poisson_radius = hair_scalar * .02;
        } else if (property.compare("hair_color") == 0) {
            hair_color = value;
        } else if (property.compare("max_threads") == 0) {
            threads = stoi(value, nullptr, 10);
        }  else if (property.compare("volume_X_dimension") == 0) {
            volumeX = stoi(value, nullptr, 10);
        }  else if (property.compare("volume_Y_dimension") == 0) {
            volumeY = stoi(value, nullptr, 10);
        }  else if (property.compare("volume_Z_dimension") == 0) {
            volumeZ = stoi(value, nullptr, 10);
        } else if (property.compare("properties_file") == 0) {
            propertiesFile = value;
        } else {
            cout << "Invalid property, cannot map " + property + ". Skipping." << endl;
            continue;
        }
    }

    //generate pbrt input file based on parsed values
    ofstream pbrtFile;
    pbrtFile.open(iFilename + ".pbrt");
    pbrtFile << generateView(distance, fov, rays, iWidth, iHeight, iFilename);   
    pbrtFile << generateArmScene(propertiesFile, volumeX, volumeY, volumeZ);
    pbrtFile << generateRoomScene(length, width, height);
    pbrtFile << generateLighting(height, lIntensity);
    pbrtFile << generateHair(hair_color, poisson_radius);
    pbrtFile.close();
    
    //Ask if a volume file should be generated, generates if requested
    volumeMenu(threads);

    //Ask if an image should be generated from this model, generates if requested
    renderMenu(iFilename);
}

int main()
{
    cout << "Welcome." << endl;
    inputFile();
    return 0;
}
