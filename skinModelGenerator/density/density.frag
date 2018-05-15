#version 410 core //GL Version 400 = 4.0, 410 = 4.1, etc.

// input from vertex shader
in vec4 position;
// output to frame buffer
layout(location = 0)
out int label; // change to char

const float PI = 3.14159265;
//Thickness in micrometers for different internal densities
const float BONE_DISTANCE_FROM_CENTER = 100.f;
const float MUSCLE_DISTANCE_FROM_CENTER = 264.f;
const float HYPERDERMIS_DISTANCE_FROM_CENTER = 300.f;
const float DERMIS_DISTANCE_FROM_CENTER = 311.f;
const float EPIDERMIS_DISTANCE_FROM_CENTER = 313.f;
const int MUSCLE_NOISE_CYCLES = 60;
const int HYPERDERMIS_NOISE_CYCLES = 1;
const int DERMIS_NOISE_CYCLES = 60;

uint hashValue(int xv, int yv) {
    uint x = uint(xv);
    uint y = uint(yv);

    x = x * 1664525u + 1013904223u;
    y = y * 1664525u + 1013904223u;
    uint z = 1013904223u;
    
    x += y * z;
    y += z * x;
    z += x * y;
    x += y * z;
    
    return x >> 16u;
}

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// convert low two bits of hash code to gradient
float grad(uint hashv, float x, float y) {
    return ((hashv & 1u) == 1 ? x : -x) + ((hashv & 2u) == 2 ? y : -y);
}

// 2D noise function
float noise(vec2 v) {
    // split v into integer and fractional parts
    vec2 vi = vec2(floor(v.x), floor(v.y));
    vec2 vf = v - vi;
    
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

float calculateNoise(vec2 uv, int octives, int scale) {
    float z = 0.0;
    for (uint i = 1; i < octives; i*=2){
        z += noise(uv * float(i)) / i;
    }
    
    return z * scale;
}

int classify(float d, vec2 uv){
    //calculate noise values for given point
    float bone_noise = 0;
    float muscle_noise = calculateNoise(vec2(uv.x / MUSCLE_NOISE_CYCLES, uv.y), 4, 10);
    float hyperdermis_noise = calculateNoise(vec2(uv.x / HYPERDERMIS_NOISE_CYCLES, uv.y), 4, 20);
    float dermis_noise = calculateNoise(vec2(uv.x / DERMIS_NOISE_CYCLES, uv.y), 4, 5);
    float epidermis_noise = 0;
    
    if (hyperdermis_noise > 8)
        hyperdermis_noise = 8;
    
    if (dermis_noise > 1)
        dermis_noise = 1;
    
    //Calculate the distances for each layer as determined by noise function
    if (d < BONE_DISTANCE_FROM_CENTER + bone_noise){
        return 0;
    }
    
    if (d < MUSCLE_DISTANCE_FROM_CENTER + muscle_noise){
        return 1;
    }
    
    if (d < HYPERDERMIS_DISTANCE_FROM_CENTER + hyperdermis_noise){
        return 2;
    }
    
    if (d < DERMIS_DISTANCE_FROM_CENTER + dermis_noise){
        return 3;
    }
    
    if (d < EPIDERMIS_DISTANCE_FROM_CENTER + epidermis_noise){
        return 4;
    }
}

void main() {
    //GL_FRAG_COORD -> pixel location (x, y, depth, ?)
    vec2 p = vec2(gl_FragCoord.xy);
    float scale;
    scale = 900.0 / PI;

    //Figure out the x, y, z based off the position in screen space need some help with this
    float x = p.x;
    float y = p.y;
    int z = 1;
    
    vec2 center = vec2(313, 313);
    vec2 uv = vec2(atan(x - center.x, y - center.y) * 180 / PI, z / scale);
    float distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));

    label = classify(distance, uv);
}
