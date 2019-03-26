/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   density.h
 * Author: bseipp
 *
 * Created on February 5, 2019, 9:58 AM
 */

#ifndef DENSITY_H
#define DENSITY_H

#include <thread>
#include "../../scene_description/properties.h"
#include "../../config/config.h"
#include "../header/noise.h"

char classify(float d, vec2f uv);
void buildDensityModel(int offset, Properties simulation);
void generateVolumeModel(Properties simulation);

#endif /* DENSITY_H */
