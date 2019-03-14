/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   config.h
 * Author: bseipp
 *
 * Created on January 30, 2019, 4:31 PM
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

const int UNIT_LENGTHS_PER_METER = 1000; //1000 mm in a meter, so each meter measurement is 1000 unit lengths
const int UNIT_LENGTHS_PER_CENTIMETER = 10; //10 mm in a meter, so each meter measurement is 1000 unit lengths

const int ARM_LENGTH = 122; // 1 Foot or 305 mm so a unit distance in world space is 1mm
const int ARM_RADIUS = 61; // ~2.4 inches or 61 mm
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

const std::string PBRT_SCENE_FOLDER = "scenes/";
const std::string PBRT_IMAGE_FOLDER = "images/";
const std::string PBRT_VOLUME_FOLDER = "volume/";
const std::string PBRT_TEXTURE_FOLDER = "textures/";


const int DERMATASCOPE_LIGHT_INTENSITY = 4;
const int ROOM_LIGHT_INTENSITY = 5;

const bool DEBUG_SCENEVIEW = false;

const bool CYLINDER_ARM_SHAPE = false; //Use either a cylinder or a triangle mesh for the shape of the arm
const bool RENDER_IMAGE = false; // Render the image after creating the scene
const bool MOVE_SCENE_FILE = false; // Move the scene file after creating the filw, moves to scenes folder


#endif /* CONFIG_H */

