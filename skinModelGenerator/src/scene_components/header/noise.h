/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   noise.h
 * Author: bseipp
 *
 * Created on February 5, 2019, 10:02 AM
 */

#ifndef NOISE_H
#define NOISE_H

#include "../../utility/vec2.h"
#include "../../config/config.h"

unsigned int hashValue(int x, int y);

// smooth fade from 1 to 0 as t goes from 0 to 1
float fade(float t);

// linear interpolation between a and b
float lerp(float a, float b, float t);

// convert low two bits of hash code to gradient
float grad(unsigned int hashv, float x, float y);

// 2D noise function
float noise2(vec2f v);

float calculateNoise(vec2f uv, int octives, int scale);

#endif /* NOISE_H */

