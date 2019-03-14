/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/arm.h"

std::string generateArmScene(Properties simulation){
    std::string armScene = "";

    armScene += "##############\n";
    armScene += "# Create Arm #\n";
    armScene += "##############\n\n";
 
    //create the medium
    armScene += "MakeNamedMedium \"smoke\" \"string type\" \"skin_heterogeneous\" \"integer trans_x\" " + std::to_string(simulation.arm.x) + " \"integer trans_y\" " + std::to_string(simulation.arm.y) + " \"integer trans_z\" " + std::to_string(simulation.arm.z) + "\n";
    armScene += "\t\"point p0\" [ -" + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_RADIUS) + " -" + std::to_string(ARM_RADIUS) + " ] \"point p1\" [ " + std::to_string(ARM_LENGTH / 2) + " " + std::to_string(ARM_RADIUS) + " " + std::to_string(ARM_RADIUS) + " ]\n";
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
                armScene += std::to_string(simulation.arm.optics[layer][property][channel]) + " ";
            }
        }
    }
    armScene += "\"]\n";
    
    armScene += "\t\t\"float asr\" 130 \"float asg\" 80 \"float asb\" 180\n";
    armScene += "\t\t\"float uroughness\" [0.05] \"float vroughness\" [0.05]\n";


    if (CYLINDER_ARM_SHAPE){
      armScene += "\tShape \"cylinder\" \"float radius\" " + std::to_string(ARM_RADIUS) + " \n";
      armScene += "\t\t\"float zmin\" -" + std::to_string(ARM_LENGTH / 2) + "\n";
      armScene += "\t\t\"float zmax\" " + std::to_string(ARM_LENGTH / 2) + "\n";
      armScene += "\t\t\"float phimax\" 360\n";
    } else {
      armScene += "\tShape \"trianglemesh\"  \"integer indices\" [ 0 1 3  1 2 3  2 1 4  4 7 2  3 2 7  7 6 3  0 3 6  6 5 0  1 0 5  5 4 1  4 5 7  5 6 7  ]\n";
      armScene += "\t\t\"point P\" [ -" + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += "  " + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += "  " + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += " -" + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += "  " + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += " -" + std::to_string(ARM_LENGTH / 2) + "  " + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += " -" + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + "  ";
      armScene += "  " + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + " -" + std::to_string(ARM_LENGTH / 2) + " ]\n";
    }

    armScene += "AttributeEnd\n\n\n";
    
    return armScene;
}
