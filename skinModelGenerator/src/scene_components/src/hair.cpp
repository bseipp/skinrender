#include "../header/hair.h"

/*******************
 *******************
 * Hair functions: *
 *******************
 *******************/
std::string addHair(float hairy_factor)
{
    float theta, theta_prime, z;
    PDSampler *sampler;
    float points[4][3];
    std::string hairGeometry = "";
    
    std::cout << "Placing hairs..." << std::endl;

    sampler = new PureSampler(hairy_factor);
    sampler->complete();
    std::cout << std::endl;
    
    /* For each point, convert to cylindrical coordinates */
    for(int i = 0; i < (int) sampler->points.size(); i++){
        std::cout << "\rBuilding hair curve: " << i + 1;

        hairGeometry += "\tShape \"curve\" \"string type\" [ \"cylinder\" ] \"point P\" [ ";
        
        /*************************************************************************
         * Sampler creates values between -1 and 1.
         * The x component is then scaled by PI to get a theta.
         * The y component is used for z in the cooresponding cylindrical coord.
         * This is for the first set of the arm from z = 0 -> z = 2
         *************************************************************************/
        
        z = sampler->points[i].y + 1;
        theta = sampler->points[i].x * PI;
        points[0][0] = (ARM_RADIUS - .05) * cos(theta);
        points[0][1] = (ARM_RADIUS - .05) * sin(theta);
        points[0][2] = z;
        
        //NIH Visible human - Possible source for model
        //NIH has man and women
        //3D volume dataset where imaged cross sections of the entire skin set
        //Someone may have made 3d geometric models for the body based off this, find out
        //Cryosection MRI and CT data NIH Geometric model?
        
        z += .1;
        points[1][0] = (ARM_RADIUS + .1) * cos(theta);
        points[1][1] = (ARM_RADIUS + .1) * sin(theta);
        points[1][2] = z;
        
        z += .1;
        theta_prime = theta + (.01 * PI);
        points[2][0] = (ARM_RADIUS + .1) * cos(theta_prime);
        points[2][1] = (ARM_RADIUS + .1) * sin(theta_prime);
        points[2][2] = z;

        z += .1;
        points[3][0] = (ARM_RADIUS + .1) * cos(theta);
        points[3][1] = (ARM_RADIUS + .1) * sin(theta);
        points[3][2] = z;
        
        for (int j = 0; j < 4; j++){
            hairGeometry += std::to_string(points[j][0]) + " " + std::to_string(points[j][1]) + " " + std::to_string(points[j][2]) + " ";
        }
        
        hairGeometry += "] \"float width0\" [ 0.004972 ] \"float width1\" [ 0.004278 ]\n\n";
        
        
        hairGeometry += "\tShape \"curve\" \"string type\" [ \"cylinder\" ] \"point P\" [ ";
        
        /*************************************************************************
         * Build a second set of points, for the bottom half of the arm
         * From z = -2 -> z = 0
         *************************************************************************/
        
        for (int j = 0; j < 4; j++){
            hairGeometry += std::to_string(points[j][0]) + " " + std::to_string(points[j][1]) + " " + std::to_string(points[j][2] - 2) + " ";
        }
        
        hairGeometry += "] \"float width0\" [ 0.004972 ] \"float width1\" [ 0.004278 ]\n\n";
        
    }
    std::cout << std::endl;
    return hairGeometry;
}

std::string generateHair(Properties simulation)
{    
    std::string sceneHair = "";

    sceneHair += "######################\n";
    sceneHair += "#     build hair     #\n";
    sceneHair += "######################\n\n";
    sceneHair += "TransformBegin\n";
    sceneHair += "\tRotate 90 1 0 0\n";
    sceneHair += "\tRotate 90 0 1 0\n";
    sceneHair += "\tMakeNamedMaterial  \"black_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 8 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"red_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 3 ]\n";
    sceneHair += "\tMakeNamedMaterial  \"brown_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ 1.3 ] \"float beta_m\" .25 \"float alpha\" 2\n";
    sceneHair += "\tMakeNamedMaterial  \"blonde_hair\" \"string type\" [ \"hair\" ] \"float eumelanin\" [ .3 ]\n\n";
    sceneHair += "\tNamedMaterial \"" + simulation.arm.hairColor + "_hair\"\n\n";
    sceneHair += addHair(simulation.arm.hairDensityFactor * .02);
    sceneHair += "TransformEnd\n";
    return sceneHair;
}