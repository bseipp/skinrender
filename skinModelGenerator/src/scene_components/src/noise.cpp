#include "../header/noise.h"

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

