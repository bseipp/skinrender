/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/view.h"

std::string generateView(Properties simulation){
    std::string sceneView = "";
    
    sceneView += "###############\n# Create View #\n###############\n";
    //sceneView += "LookAt 0 0.1 " + to_string(simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER + ARM_RADIUS) + " #eye\n";
    sceneView += "LookAt 0 0 " + std::to_string(ARM_RADIUS + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + " #eye\n";
    sceneView += "\t 0 0.1 0 #look at point\n";
    sceneView += "\t0 0 1 #up vector\n";
    sceneView += "Camera \"perspective\" \"float fov\" " + std::to_string(simulation.view.fov) + "\n";
    sceneView += "Sampler \"02sequence\" \"integer pixelsamples\" " + std::to_string(simulation.view.raysPerPixel) + "\n";
    sceneView += "Integrator \"volpath\" \"integer maxdepth\" [1]\n";
    sceneView += "Film \"image\" \"string filename\" \"" + PBRT_IMAGE_FOLDER + simulation.filename + ".png\"\n";
    sceneView += "\"integer xresolution\" [" + std::to_string(simulation.view.xRes) + "] \"integer yresolution\" [" + std::to_string(simulation.view.yRes) + "]\n\n";
    
    return sceneView;
}