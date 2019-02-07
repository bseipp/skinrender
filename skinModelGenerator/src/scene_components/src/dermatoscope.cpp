/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/dermatoscope.h"

std::string generateDermatascope(Properties simulation){
    std::string dermatascopeView = "";

    dermatascopeView += "######################\n";
    dermatascopeView += "# build dermatascope #\n";
    dermatascopeView += "######################\n\n";
    
    dermatascopeView += "TransformBegin\n";
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTranslate 0 0 " + std::to_string(ARM_RADIUS + 7.3) + "\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"rgb Kd\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tShape \"cylinder\" \"float radius\" 15.\n";
    dermatascopeView += "\t\t\"float zmin\" -7.5\n";
    dermatascopeView += "\t\t\"float zmax\" 7.5\n";
    dermatascopeView += "\t\t\"float phimax\" 360\n";
    dermatascopeView += "\tAttributeEnd\n\n";
    
    //Handle Light components
    dermatascopeView += "TransformBegin\n";
    
    dermatascopeView += "\tTranslate 0 0 " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + "\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tAreaLightSource \"diffuse\" \"rgb L\" [ " + std::to_string(simulation.dermatascope.lightRgb[0] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[1] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[2] * DERMATASCOPE_LIGHT_INTENSITY) + " ]\n";
    dermatascopeView += "\t\tShape \"disk\" \"float height\" 0\n";
    dermatascopeView += "\t\t\t\"float radius\" 15.\n";
    dermatascopeView += "\t\t\t\"float innerradius\" 0\n";
    dermatascopeView += "\t\t\t\"float phimax\" 360\n";
    dermatascopeView += "\tAttributeEnd\n\n";
    
    dermatascopeView += "\t#Light from scope form halo\n";
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[1] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[2] * DERMATASCOPE_LIGHT_INTENSITY) + " ] \"point from\" [ 15. 0 0 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[1] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[2] * DERMATASCOPE_LIGHT_INTENSITY) + " ] \"point from\" [ -15. 0 0 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[1] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[2] * DERMATASCOPE_LIGHT_INTENSITY) + " ] \"point from\" [ 0 -15. 0 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[1] * DERMATASCOPE_LIGHT_INTENSITY) + " " + std::to_string(simulation.dermatascope.lightRgb[2] * DERMATASCOPE_LIGHT_INTENSITY) + " ] \"point from\" [ 0 15. 0 ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";    
    
    dermatascopeView += "TransformEnd\n";
   
    dermatascopeView += "AttributeBegin\n";
    dermatascopeView += "\tTranslate 0 0 " + std::to_string(ARM_RADIUS + 19.8) + "\n";
    dermatascopeView += "\tMaterial \"matte\" \"rgb Kd\" [ " + std::to_string(simulation.room.materialRgb[0]) + " " + std::to_string(simulation.room.materialRgb[1]) + " " + std::to_string(simulation.room.materialRgb[2]) + " ]\n";
    dermatascopeView += "\tShape \"trianglemesh\"  \"integer indices\" [0 1 5   5 4 0   0 3 7   7 4 0   5 6 7   7 4 5   3 2 6   6 7 3   2 1 5   5 6 2   1 2 3   3 0 1]\n";
    dermatascopeView += "\t\"point P\" [ -15 -30  5    -15 15  5    15 15  5    15 -30  5  -15 -30 -5    -15 15 -5    15 15 -5    15 -30 -5 ]\n";
    dermatascopeView += "AttributeEnd\n\n";
    
    return dermatascopeView;
}