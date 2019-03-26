/**********************
 **********************
 * Density functions: *
 **********************
 **********************/

#include "../header/density.h"
#include <stdio.h>
#include <string.h>
char* densityGrid;

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

void buildDensityModel(int thread, Properties simulation){
    unsigned int slicesPerThread = simulation.density.z / (THREADS - 1);
    unsigned int memory_offset = simulation.density.x * simulation.density.y * slicesPerThread * thread;
    int max = (thread + 1 == THREADS ? slicesPerThread * thread + (simulation.density.z % (THREADS - 1)) : slicesPerThread * (thread + 1));
    unsigned int max_memory_offset = simulation.density.x * simulation.density.y * max - 1;
    vec2f center = vec2f(simulation.density.x / 2.0, simulation.density.y / 2.0);

    float SCALE = 900 / PI;
    string noiseData = "";
    fprintf(stderr, "Thread %d: Building Density Model for %d to %d || Memory_offset %u to %u\n", thread + 1, slicesPerThread * thread, max - 1, memory_offset, max_memory_offset);
    for(int z = slicesPerThread * thread; z < max; z++) {
        for(int x = 0; x < simulation.density.x; x++){
            for(int y = 0; y < simulation.density.y; y++){
                vec2f uv = vec2f(atan2(x - center.x, y - center.y) * 180 / PI, z / SCALE);

                float distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));

                densityGrid[memory_offset] = classify(distance, uv);
                memory_offset++;
            }
        }
    }
}

void generateVolumeModel(Properties simulation){
    /*********************************************************************
     *  Create an array that is large enough to hold the entire density  *
     *********************************************************************/

    unsigned int arraySize = simulation.density.z * simulation.density.x * simulation.density.y;
    try {
        densityGrid = new char[arraySize];
        memset( densityGrid, '0', sizeof(char) * arraySize );
    } catch (std::bad_alloc&) {
        cout << endl << "Bad alloc" << endl;
        exit(0);
    }

    /**********************************************************
     * Determine how large of a chunk each thread should take *
     **********************************************************/
    fprintf(stderr, "Building density volume.\n");
    std::thread threads[THREADS];
    for (int i = 0; i < THREADS; i++){
        threads[i] = std::thread(buildDensityModel, i, simulation);
    }

    for (int i = 0; i < THREADS; i++){
        threads[i].join();
    }

    fprintf(stderr, "Writing density volume file.\n");
    FILE* fd = fopen((PBRT_VOLUME_FOLDER + simulation.arm.filename).c_str(), "wb");
    fwrite(&densityGrid[0], sizeof(char), simulation.density.z * simulation.density.x * simulation.density.y, fd);
    fclose(fd);
}
