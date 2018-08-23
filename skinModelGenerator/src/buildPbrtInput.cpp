#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "dart_throwing/PDSampling.h"
#include "vec2.h"
#include <thread>
#include "properties.h"

using namespace std;

const int ARM_LENGTH = 5;
const int ARM_RADIUS = 1;
const int CHANNELS = 6;
const double PI = 3.1415;

//Thickness in micrometers for different internal densities
const float BONE_DISTANCE_FROM_CENTER = 100;
const float MUSCLE_DISTANCE_FROM_CENTER = 264;
const float HYPERDERMIS_DISTANCE_FROM_CENTER = 300;
const float DERMIS_DISTANCE_FROM_CENTER = 311;
const float EPIDERMIS_DISTANCE_FROM_CENTER = 313;

enum Layer {NOT_IN_ARM, EPIDERMIS, DERMIS, HYPERDERMIS, MUSCLE, BONE };
enum Channel { RED, GREEN, BLUE };
enum Property { TRANSMITANCE, ALBIEDO };

const int MUSCLE_NOISE_CYCLES = 60;
const int HYPERDERMIS_NOISE_CYCLES = 1;
const int DERMIS_NOISE_CYCLES = 60;

const int THREADS = 7;

const string PBRT_SCENE_FOLDER = "scenes/";
const string PBRT_IMAGE_FOLDER = "images/";
const string PBRT_VOLUME_FOLDER = "volume/";
const string PBRT_TEXTURE_FOLDER = "textures/";
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
    cout << endl;
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

void generateVolumeModel(Properties simulation){
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
    thread threads[THREADS];
    for (int i = 1; i < THREADS; i++){
        threads[i] = thread(buildDensityModel, i * 500);
    }
    threads[0] = thread(buildDensityModel, 0);
    for (int i = 0; i < THREADS; i++){
        threads[i].join();
    }
    
    fprintf(stderr, "Writing density volume file.\n");
    FILE* fd = fopen(simulation.arm.filename.c_str(), "wb");
    fwrite(&grid[0], sizeof(char), arm_length * arm_radius * arm_radius * CHANNELS, fd);
    fclose(fd);
}

void run(string filename);

/**********************
 **********************
 *  Scene functions:  *
 **********************
 **********************/
string generateArmScene(Properties simulation){
    string armScene = "";

    armScene += "##############\n";
    armScene += "# Create Arm #\n";
    armScene += "##############\n\n";
 
    //create the medium
    armScene += "MakeNamedMedium \"smoke\" \"string type\" \"skin_heterogeneous\" \"integer trans_x\" " + to_string(simulation.arm.x) + " \"integer trans_y\" " + to_string(simulation.arm.y) + " \"integer trans_z\" " + to_string(simulation.arm.z) + "\n";
    armScene += "\t\"point p0\" [ -2.5 -1. -1. ] \"point p1\" [ 2.5 1 1 ]\n";
    armScene += "\t\"string density_file\" [\"" + PBRT_VOLUME_FOLDER + simulation.arm.filename + "\"]\n";
    armScene += "\t\"string volumetric_colors\" [\"" + PBRT_VOLUME_FOLDER + simulation.arm.filename + "\"]\n\n";
    armScene += "\t\"color sigma_a\" [30 30 30] \"color sigma_s\" [50 50 50]\n\n";
    
    //Create the material
    armScene += "AttributeBegin\n";
    armScene += "\tRotate 90 0 1 0\n";
    armScene += "\tTexture \"brianskin\" \"color\" \"imagemap\"\n";
    armScene += "\t\t\"string filename\" [\"" + PBRT_SCENE_FOLDER + PBRT_TEXTURE_FOLDER + "brian.png\"]\n\n";
    
    armScene +="\tMediumInterface \"smoke\" \"\"\n";
    armScene +="\tMaterial \"skin\" \"texture Kd\" \"brianskin\"\n";
    armScene += "\t\t\"float eta\" [1.33] \"color mfp\" [1.2953e-03 9.5238e-04 6.7114e-04]\n";
    
    //loop to fetch properties
    armScene += "\t\t\"string t1\" [\"";
    
    for (int layer = NOT_IN_ARM; layer <= BONE; layer++){
        for (int property = TRANSMITANCE; property <= ALBIEDO; property++){
            for (int channel = RED; channel <= BLUE; channel++){
                armScene += to_string(simulation.arm.optics[layer][property][channel]) + " ";
            }
        }
    }
    armScene += "\"]\n";
    
    armScene += "\t\t\"float asr\" 130 \"float asg\" 80 \"float asb\" 180\n";
    armScene += "\t\t\"float uroughness\" [0.05] \"float vroughness\" [0.05]\n";
    
    armScene += "\tShape \"cylinder\" \"float radius\" 1\n";
    armScene += "\t\t\"float zmin\" -2.5\n";
    armScene += "\t\t\"float zmax\" 2.5\n";
    armScene += "\t\t\"float phimax\" 360\n";
    armScene += "AttributeEnd\n\n\n";
    
    return armScene;
}

string generateRoomScene(Properties simulation){
    string roomScene = "";

    roomScene += "######################\n";
    roomScene += "#     build room     #\n";
    roomScene += "######################\n\n";

    roomScene += "###############\n# Create Room #\n###############\n";
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   " + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";

    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   " + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "   " + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "    " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "    " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "   " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " -" + to_string(simulation.room.z) + "    " + to_string(simulation.room.y) + " -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.z) + "    " + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.room.materialRgb[0]) + " " + to_string(simulation.room.materialRgb[1]) + " " + to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "    " + to_string(simulation.room.x) + " " + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + "    " + to_string(simulation.room.x) + " -" + to_string(simulation.room.y) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tAreaLightSource \"diffuse\" \"rgb L\" [ " + to_string(simulation.room.lightRgb[0]) + " " + to_string(simulation.room.lightRgb[1]) + " " + to_string(simulation.room.lightRgb[2]) + " ]\n";
    roomScene += "\tShape \"trianglemesh\"        \"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + to_string(simulation.room.x / 3) + " -" + to_string(simulation.room.y / 3) + " " + to_string(simulation.room.z) + "   -" + to_string(simulation.room.x / 3) + " " + to_string(simulation.room.y / 3) + " " + to_string(simulation.room.z) + "    " + to_string(simulation.room.x / 3) + " " + to_string(simulation.room.y / 3) + " " + to_string(simulation.room.z) + "    " + to_string(simulation.room.x / 3) + " -" + to_string(simulation.room.y / 3) + " " + to_string(simulation.room.z) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    return roomScene;
}

string generateHair(Properties simulation){    
    string sceneHair = "";

    sceneHair += "######################\n";
    sceneHair += "#     build hair     #\n";
    sceneHair += "######################\n\n";
    sceneHair += "TransformBegin\n";
    sceneHair += "\tRotate 90 1 0 0\n";
    sceneHair += "\tRotate 90 0 1 0\n";
    sceneHair += "\tMakeNamedMaterial  \"black_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 8 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"red_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 3 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"brown_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 1.3 ] \"float beta_m\" .25 \"float alpha\" 2\n";
    sceneHair += "\tMakeNamedMaterial  \"blonde_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ .3 ]\n\n";
    sceneHair += "\tNamedMaterial \"" + simulation.arm.hairColor + "_hair\"\n\n";
    sceneHair += addHair(simulation.arm.hairDensityFactor * .02);
    sceneHair += "TransformEnd\n";
    return sceneHair;
}

string generateView(Properties simulation){
    string sceneView = "";
    
    sceneView += "###############\n# Create View #\n###############\n";
    sceneView += "LookAt 0 0.1 " + to_string(1 + simulation.view.z) + " #eye\n";
    sceneView += "\t 0 0 0 #look at point\n";
    sceneView += "\t0 0 1 #up vector\n";
    sceneView += "Camera \"perspective\" \"float fov\" " + to_string(simulation.view.fov) + "\n";
    sceneView += "Sampler \"02sequence\" \"integer pixelsamples\" " + to_string(simulation.view.raysPerPixel) + "\n";
    sceneView += "Integrator \"volpath\" \"integer maxdepth\" [1]\n";
    sceneView += "Film \"image\" \"string filename\" \"" + PBRT_IMAGE_FOLDER + simulation.filename + ".png\"\n";
    sceneView += "\"integer xresolution\" [" + to_string(simulation.view.xRes) + "] \"integer yresolution\" [" + to_string(simulation.view.yRes) + "]\n\n";
    
    return sceneView;
}

string generateDermatascope(Properties simulation){
    string dermatascopeView = "";

    dermatascopeView += "######################\n";
    dermatascopeView += "# build dermatascope #\n";
    dermatascopeView += "######################\n\n";
    
    dermatascopeView += "TransformBegin\n";
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTranslate 0 0 1.3\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"rgb Kd\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tShape \"cylinder\" \"float radius\" .5\n";
    dermatascopeView += "\t\t\"float zmin\" -.5\n";
    dermatascopeView += "\t\t\"float zmax\" .5\n";
    dermatascopeView += "\t\t\"float phimax\" 360\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\t#Light from scope form halo\n";
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + to_string(simulation.dermatascope.lightRgb[0]) + " " + to_string(simulation.dermatascope.lightRgb[1]) + " " + to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ .5 0 " + to_string(1.01 + simulation.view.z) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + to_string(simulation.dermatascope.lightRgb[0]) + " " + to_string(simulation.dermatascope.lightRgb[1]) + " " + to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ -.5 0 " + to_string(1.01 + simulation.view.z) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + to_string(simulation.dermatascope.lightRgb[0]) + " " + to_string(simulation.dermatascope.lightRgb[1]) + " " + to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ 0 -.5 " + to_string(1.01 + simulation.view.z) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + to_string(simulation.dermatascope.lightRgb[0]) + " " + to_string(simulation.dermatascope.lightRgb[1]) + " " + to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ 0 .5 " + to_string(1.01 + simulation.view.z) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tAreaLightSource \"diffuse\" \"rgb L\" [ " + to_string(simulation.dermatascope.lightRgb[0]) + " " + to_string(simulation.dermatascope.lightRgb[1]) + " " + to_string(simulation.dermatascope.lightRgb[2]) + " ]\n";
    dermatascopeView += "\t\tShape \"disk\" \"float height\" 1.71\n";
    dermatascopeView += "\t\t\t\"float radius\" .5\n";
    dermatascopeView += "\t\t\t\"float innerradius\" 0\n";
    dermatascopeView += "\t\t\t\"float phimax\" 360\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -.5 -.5 1.8   1. -.5 1.8   1. .5 1.8   -.5 .5 1.8 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -.5 -.5 1.8   -.5 .5 1.8   -.5 .5 2.3   -.5 -.5 2.3 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -.5 -.5 1.8   1. -.5 1.8   1. -.5 2.3   -.5 -.5 2.3 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -.5 .5 2.3   -.5 .5 1.8    1. .5 1.8    1. .5 2.3 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ 1. .5 2.3   1. .5 1.8    1. -.5 1.8    1. -.5 2.3 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + to_string(simulation.dermatascope.materialRgb[0]) + " " + to_string(simulation.dermatascope.materialRgb[1]) + " " + to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -.5 -.5 2.3   -.5 .5 2.3    1. .5 2.3    1. -.5 2.3 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";
    dermatascopeView += "TransformEnd\n\n";
    
    return dermatascopeView;
}

/**********************
 **********************
 *  Driver functions: *
 **********************
 **********************/
void run(string filename){
    //Read the input file and parse values out
    string cmd;
    ifstream file;
    file.open(filename);
    if (file.fail()) { 
        cout << "Bad file name" << endl; 
        return;
    }

    string line;
    getline(file, line); //Burn Headers
    while(getline(file, line)){
        Properties simulation(line);
        
        ofstream pbrtFile;
        
        //Check that the files we are about to create don't already exist. 
        string originalFilename = simulation.filename;
        int duplicateFileCounter = 1;
        ifstream file((PBRT_SCENE_FOLDER + simulation.filename + ".pbrt").c_str());
        while (file.good()){
            simulation.filename = originalFilename + " (" + to_string(duplicateFileCounter) + ")";
            duplicateFileCounter++;
            file.close();
            file.open((PBRT_SCENE_FOLDER + simulation.filename + ".pbrt").c_str());
        }

        pbrtFile.open(PBRT_SCENE_FOLDER + simulation.filename + ".pbrt");
        pbrtFile << generateView(simulation);  
        pbrtFile << "WorldBegin\n\n";
        pbrtFile << generateArmScene(simulation);
        pbrtFile << generateDermatascope(simulation);
        pbrtFile << generateRoomScene(simulation);
        pbrtFile << generateHair(simulation);
        pbrtFile << "WorldEnd";
        pbrtFile.close();

        //Verify if density file specified exists, if not, generate a new one
        ifstream f(PBRT_VOLUME_FOLDER + simulation.arm.filename.c_str());
        if (!f.good()){
            generateVolumeModel(simulation);
        }

        cmd = "./pbrt " + PBRT_SCENE_FOLDER + simulation.filename + ".pbrt";
        system(cmd.c_str());
    } 
}

int main(int argc, char *argv[])
{
    if (argc > 1){
        run(argv[1]);
        return 0;
    } else {
        cout << "No file specified." << endl;
        return -1;
    }
}
