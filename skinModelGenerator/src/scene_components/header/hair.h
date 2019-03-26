/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   hair.h
 * Author: bseipp
 *
 * Created on February 5, 2019, 10:05 AM
 */

#ifndef HAIR_H
#define HAIR_H

#include <math.h>       /* sqrt */
#include <string>     // std::string, std::to_string
#include <iostream>
#include "../../dart_throwing/PDSampling.h"
#include "../../config/config.h"
#include "../../scene_description/properties.h"

std::string addHair(Properties simulation);
std::string generateHair(Properties simulation);


#endif /* HAIR_H */
