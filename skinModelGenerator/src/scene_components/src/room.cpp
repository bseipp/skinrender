/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/room.h"

std::string generateRoomScene(Properties simulation){
    std::string roomScene = "";

    roomScene += "######################\n";
    roomScene += "#     build room     #\n";
    roomScene += "######################\n\n";

    roomScene += "###############\n# Create Room #\n###############\n";
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";

    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    roomScene += "\t\"float uscale\" [8] \"float vscale\" [8]\n";
    roomScene += "\t\"rgb tex1\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    roomScene += "\tMaterial \"matte\" \"texture Kd\" \"checks\" Shape \"trianglemesh\"\n";
    roomScene += "\t\"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    roomScene += "AttributeBegin\n";
    roomScene += "\tAreaLightSource \"diffuse\" \"rgb L\" [ " + std::to_string(simulation.room.lightRgb[0]) + " " + std::to_string(simulation.room.lightRgb[1]) + " " + std::to_string(simulation.room.lightRgb[2]) + " ]\n";
    roomScene += "\tShape \"trianglemesh\"        \"integer indices\" [0 1 2 0 2 3]\n";
    roomScene += "\t\"point P\" [ -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER / 3) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "   -" + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + "    " + std::to_string(simulation.room.x * UNIT_LENGTHS_PER_METER / 3) + " -" + std::to_string(simulation.room.y * UNIT_LENGTHS_PER_METER / 3) + " " + std::to_string(simulation.room.z * UNIT_LENGTHS_PER_METER) + " ]\n";
    roomScene += "AttributeEnd\n\n";
    
    return roomScene;
}