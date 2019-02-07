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
    
    for (int angle = 0; angle < 90; angle+=15) {
        dermatascopeView += "TransformBegin\n";
        dermatascopeView += "\tRotate " + std::to_string(angle) + " 0 0 1\n";
        dermatascopeView += "\t#Light from scope form halo\n";
        dermatascopeView += "\tAttributeBegin\n";
        dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0]) + " " + std::to_string(simulation.dermatascope.lightRgb[1]) + " " + std::to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ 15. 0 " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + " ]\n";
        dermatascopeView += "\tAttributeEnd\n\n";

        dermatascopeView += "\tAttributeBegin\n";
        dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0]) + " " + std::to_string(simulation.dermatascope.lightRgb[1]) + " " + std::to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ -15. 0 " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + " ]\n";
        dermatascopeView += "\tAttributeEnd\n\n";

        dermatascopeView += "\tAttributeBegin\n";
        dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0]) + " " + std::to_string(simulation.dermatascope.lightRgb[1]) + " " + std::to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ 0 -15. " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + " ]\n";
        dermatascopeView += "\tAttributeEnd\n\n";

        dermatascopeView += "\tAttributeBegin\n";
        dermatascopeView += "\t\tLightSource \"point\" \"rgb I\" [ " + std::to_string(simulation.dermatascope.lightRgb[0]) + " " + std::to_string(simulation.dermatascope.lightRgb[1]) + " " + std::to_string(simulation.dermatascope.lightRgb[2]) + " ] \"point from\" [ 0 15. " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + " ]\n";
        dermatascopeView += "\tAttributeEnd\n\n";
        dermatascopeView += "TransformEnd\n";
    }
    
    
    
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tAreaLightSource \"diffuse\" \"rgb L\" [ " + std::to_string(simulation.dermatascope.lightRgb[0]) + " " + std::to_string(simulation.dermatascope.lightRgb[1]) + " " + std::to_string(simulation.dermatascope.lightRgb[2]) + " ]\n";
    dermatascopeView += "\t\tShape \"disk\" \"float height\" " + std::to_string(ARM_RADIUS + .1 + simulation.view.z * UNIT_LENGTHS_PER_CENTIMETER) + "\n";
    dermatascopeView += "\t\t\t\"float radius\" 15.\n";
    dermatascopeView += "\t\t\t\"float innerradius\" 0\n";
    dermatascopeView += "\t\t\t\"float phimax\" 360\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    
    
    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -15 -15 " + std::to_string(ARM_RADIUS + 14.8) + "   30 -15 " + std::to_string(ARM_RADIUS + 14.8) + "   30 15 " + std::to_string(ARM_RADIUS + 14.8) + "   -15 15 " + std::to_string(ARM_RADIUS + 14.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -15 -15 14.8   -15 15 " + std::to_string(ARM_RADIUS + 14.8) + "   -15 15 " + std::to_string(ARM_RADIUS + 24.8) + "   -15 -15 " + std::to_string(ARM_RADIUS + 24.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -15 -15 14.8   30 -15 " + std::to_string(ARM_RADIUS + 14.8) + "   30 -15 " + std::to_string(ARM_RADIUS + 24.8) + "   -15 -15 " + std::to_string(ARM_RADIUS + 24.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -15 15 24.8   -15 15 " + std::to_string(ARM_RADIUS + 14.8) + "    30 15 " + std::to_string(ARM_RADIUS + 14.8) + "    30 15 " + std::to_string(ARM_RADIUS + 24.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ 30 15 24.8   30 15 " + std::to_string(ARM_RADIUS + 14.8) + "    30 -15 " + std::to_string(ARM_RADIUS + 14.8) + "    30 -15 " + std::to_string(ARM_RADIUS + 24.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";

    dermatascopeView += "\tAttributeBegin\n";
    dermatascopeView += "\t\tTexture \"checks\" \"spectrum\" \"checkerboard\"\n";
    dermatascopeView += "\t\t\"float uscale\" [8] \"float vscale\" [8]\n";
    dermatascopeView += "\t\t\"rgb tex1\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ] \"rgb tex2\" [ " + std::to_string(simulation.dermatascope.materialRgb[0]) + " " + std::to_string(simulation.dermatascope.materialRgb[1]) + " " + std::to_string(simulation.dermatascope.materialRgb[2]) + " ]\n";
    dermatascopeView += "\t\tMaterial \"matte\" \"texture Kd\" \"checks\"    Shape \"trianglemesh\"\n";
    dermatascopeView += "\t\t\"integer indices\" [0 1 2 0 2 3]\n";
    dermatascopeView += "\t\t\"point P\" [ -15 -15 24.8   -15 15 " + std::to_string(ARM_RADIUS + 24.8) + "    30 15 " + std::to_string(ARM_RADIUS + 24.8) + "    30 -15 " + std::to_string(ARM_RADIUS + 24.8) + " ]\n";
    dermatascopeView += "\tAttributeEnd\n\n";
    dermatascopeView += "TransformEnd\n\n";
    
    return dermatascopeView;
}