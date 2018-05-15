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
    for (unsigned int i = 1; i < octives; i*=2){
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

/*****************************
 *****************************
 * PBRT Generator functions: *
 *****************************
 *****************************/
void renderMenu(string file)
{
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

void generateFile(string filename);

void inputFile()
{
    string filename = " ";
    cout << "Enter a file to render:" << endl;
    cin >> filename;
    if (filename.compare(" ") == 0){
        inputFile();
        return;
    }
    generateFile(filename);
}

void generateFile(string filename)
{
    ifstream file;
    file.open(filename);
    if (file.fail()) { cout << "Bad file name, try again." << endl; inputFile();}
    int length = 10, width = 10, height = 10, distance = 9, fov = 60, rays = 128, iWidth = 400, iHeight = 400, lIntensity = 20;
    string iFilename = "temp", line, property, value, hair_color = "brown";

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
        } else {
            cout << "Invalid property, cannot map " + property + ". Skipping." << endl;
            continue;
        }
    }

    ofstream pbrtFile;
    pbrtFile.open(iFilename + ".pbrt");
    pbrtFile << "###############\n# Create View #\n###############\n";
    pbrtFile << "LookAt 0 -" + to_string(distance) + " 0 #eye\n";
    pbrtFile << "\t 0 0 0 #look at point\n\t0 0 1 #up vector\n";
    pbrtFile << "Camera \"perspective\" \"float fov\" " + to_string(fov) + "\n";
    pbrtFile << "Sampler \"halton\" \"integer pixelsamples\" " + to_string(rays) + "\n";
    pbrtFile << "Integrator \"path\"\nFilm \"image\" \"string filename\" \"" + iFilename + ".exr\"\n";
    pbrtFile << "\"integer xresolution\" [" + to_string(iWidth) + "] \"integer yresolution\" [" + to_string(iHeight) + "]\n\n";
    
    pbrtFile << "WorldBegin\nRotate 45 1 0 0\nRotate 90 0 1 0\nActiveTransform All\n\n";
    pbrtFile << "##############\n# Create Arm #\n##############\n";
    pbrtFile << "AttributeBegin\n\tTexture \"brianskin\" \"color\" \"imagemap\"\n";
    pbrtFile << "\t\t\"string filename\" [\"brian.png\"]\n\n";
    pbrtFile << "\tMaterial \"kdsubsurface\" \"texture Kd\" \"brianskin\"\n";
    pbrtFile << "\t\t\"float eta\" [1.33] \"color mfp\" [1.2953e-03 9.5238e-04 6.7114e-04]\n";
    pbrtFile << "\t\t\"float uroughness\" [0.05] \"float vroughness\" [0.05]\n";
    pbrtFile << "\t\t\"bool remaproughness\" [\"false\"]";
    pbrtFile << "\tShape \"cylinder\" \"float radius\" " + to_string(ARM_RADIUS) + "\n";
    pbrtFile << "\t\t\"float zmin\" -" + to_string(float(ARM_LENGTH) / 2) + "\n";
    pbrtFile << "\t\t\"float zmax\" " + to_string(float(ARM_LENGTH) / 2) + "\n";
    pbrtFile << "\t\t\"float phimax\" 360\n";
    pbrtFile << "AttributeEnd\n\n\n";
    
    pbrtFile << "###############\n# Create Room #\n###############\n";
    pbrtFile << "AttributeBegin\n\tTranslate 0 0 -" + to_string(width) + "\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ -" + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " " + to_string(width) + " 0   -" + to_string(width) + " " + to_string(width) + " 0 ]\n";
    pbrtFile << "AttributeEnd\n\n";

    pbrtFile << "AttributeBegin\n\tTranslate 0 0 " + to_string(width) + "\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ -" + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " -" + to_string(width) + " 0   " + to_string(width) + " " + to_string(width) + " 0   -" + to_string(width) + " " + to_string(width) + " 0 ]\n";
    pbrtFile << "AttributeEnd\n\n";

    pbrtFile << "AttributeBegin\n\tTranslate " + to_string(height) + " 0 0\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ 0 -" + to_string(height) + " -" + to_string(height) + "   0 " + to_string(height) + " -" + to_string(height) + "  0 " + to_string(height) + " " + to_string(height) + "   0 -" + to_string(height) + " " + to_string(height) + " ]\n";
    pbrtFile << "AttributeEnd\n\n";

    pbrtFile << "AttributeBegin\n\tTranslate -" + to_string(height) + " 0 0\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ 0 -" + to_string(height) + " -" + to_string(height) + "   0 " + to_string(height) + " -" + to_string(height) + "  0 " + to_string(height) + " " + to_string(height) + "   0 -" + to_string(height) + " " + to_string(height) + " ]\n";
    pbrtFile << "AttributeEnd\n\n";
    
    pbrtFile << "AttributeBegin\n\tTranslate 0 -" + to_string(length) + " 0\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ -" + to_string(length) + " 0 -" + to_string(length) + "   -" + to_string(length) + " 0 " + to_string(length) + "  " + to_string(length) + " 0 " + to_string(length) + "   " + to_string(length) + " 0 -" + to_string(length) + " ]\n";
    pbrtFile << "AttributeEnd\n\n";
    
    pbrtFile << "AttributeBegin\n\tTranslate 0 " + to_string(length) + " 0\n";
    pbrtFile << "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    pbrtFile << "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    pbrtFile << "\t\t\"rgb tex1\" [ .95 .95 .95 ] \"rgb tex2\" [ .95 .95 .95 ]\n";
    pbrtFile << "\tMaterial \"matte\" \"texture Kd\" \"checks\"";
    pbrtFile << "\tShape \"trianglemesh\"\n";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ -" + to_string(length) + " 0 -" + to_string(length) + "   -" + to_string(length) + " 0 " + to_string(length) + "  " + to_string(length) + " 0 " + to_string(length) + "   " + to_string(length) + " 0 -" + to_string(length) + " ]\n";
    pbrtFile << "AttributeEnd\n\n";
    
    pbrtFile << "\tMakeNamedMaterial  \"black_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 8 ]\n";
    pbrtFile << "\tMakeNamedMaterial  \"red_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 3 ]\n";
    pbrtFile << "\tMakeNamedMaterial  \"brown_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 1.3 ] \"float beta_m\" .25 \"float alpha\" 2\n";
    pbrtFile << "\tMakeNamedMaterial  \"blonde_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ .3 ]\n";
    
    pbrtFile << "\n\tNamedMaterial \"" + hair_color + "_hair\"\n\n";
    pbrtFile << addHair(poisson_radius);
    
    pbrtFile << "################\n# Create Light #\n################\n";
    pbrtFile << "AttributeBegin\n";
    pbrtFile << "\tAreaLightSource \"diffuse\" \"blackbody L\" [ 4000 " + to_string(lIntensity) + " ]\n";
    pbrtFile << "\tTranslate -" + to_string(height) + " 0 0\n";
    pbrtFile << "\tShape \"trianglemesh\"";
    pbrtFile << "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    pbrtFile << "\t\t\"point P\" [ 0 -2 -2   0 2 -2  0 2 2   0 -2 2 ]\n";
    pbrtFile << "AttributeEnd\n\n";
    pbrtFile << "WorldEnd";

    pbrtFile.close();
    
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
    int t_count = 7;
    thread threads[t_count];
    for (int i = 1; i < t_count; i++){
        threads[i] = thread(buildDensityModel, i * 500);
    }
    threads[0] = thread(buildDensityModel, 0);
    for (int i = 0; i < t_count; i++){
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

    renderMenu(iFilename);
}


int main()
{
    cout << "Welcome." << endl;
    inputFile();
    return 0;
}
