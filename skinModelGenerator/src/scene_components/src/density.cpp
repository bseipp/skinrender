/**********************
 **********************
 * Density functions: *
 **********************
 **********************/

#include "../header/density.h"

char* densityGrid;


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
                
                densityGrid[memory_offset] = classify(distance, uv);
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
        densityGrid = new char[arraySize];
    } catch (std::bad_alloc&) {
        cout << endl << "Bad alloc" << endl;
        exit(0);
    }
    
    fprintf(stderr, "Building density volume.\n");
    std::thread threads[THREADS];
    for (int i = 1; i < THREADS; i++){
        threads[i] = std::thread(buildDensityModel, i * 500);
    }
    threads[0] = std::thread(buildDensityModel, 0);
    for (int i = 0; i < THREADS; i++){
        threads[i].join();
    }
    
    fprintf(stderr, "Writing density volume file.\n");
    FILE* fd = fopen(simulation.arm.filename.c_str(), "wb");
    fwrite(&densityGrid[0], sizeof(char), arm_length * arm_radius * arm_radius * CHANNELS, fd);
    fclose(fd);
}
