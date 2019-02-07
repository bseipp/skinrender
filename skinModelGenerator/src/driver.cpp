#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "config/config.h"
#include "scene_description/properties.h"
#include "scene_components/header/density.h"
#include "scene_components/header/hair.h"
#include "scene_components/header/room.h"
#include "scene_components/header/arm.h"
#include "scene_components/header/dermatoscope.h"
#include "scene_components/header/view.h"

/**********************
 **********************
 *  Driver functions: *
 **********************
 **********************/
void run(string filename){
    //Read the input file and parse values out
    std::string cmd;
    ifstream file;
    file.open(filename);
    if (file.fail()) { 
        std::cout << "Bad file name" << std::endl; 
        return;
    }

    std::string line;
    getline(file, line); //Burn Headers
    while(getline(file, line)){
        Properties simulation(line);
        
        ofstream pbrtFile;
        
        //Check that the files we are about to create don't already exist. 
        std::string originalFilename = simulation.filename;
        int duplicateFileCounter = 1;
        ifstream file((simulation.filename + ".pbrt").c_str());
        while (file.good()){
            simulation.filename = originalFilename + "_" + std::to_string(duplicateFileCounter);
            duplicateFileCounter++;
            file.close();
            file.open((simulation.filename + ".pbrt").c_str());
        }

        pbrtFile.open(simulation.filename + ".pbrt");
        pbrtFile << generateView(simulation);  
        pbrtFile << "WorldBegin\n\n";
        pbrtFile << generateArmScene(simulation);
        pbrtFile << generateDermatascope(simulation);
        pbrtFile << generateRoomScene(simulation);
        pbrtFile << generateHair(simulation);
        pbrtFile << "WorldEnd";
        pbrtFile.close();

        //Verify if density file specified exists, if not, generate a new one
        ifstream f(PBRT_VOLUME_FOLDER + simulation.arm.filename.c_str());
        if (!f.good()){
            generateVolumeModel(simulation);
        }

        cmd = "./pbrt " + simulation.filename + ".pbrt";
        system(cmd.c_str());
        
        cmd = "mv " + simulation.filename + ".pbrt " + PBRT_SCENE_FOLDER;
        system(cmd.c_str());
    } 
}

int main(int argc, char *argv[])
{
    if (argc > 1){
        run(argv[1]);
        return 0;
    } else {
        cout << "No file specified." << endl;
        return -1;
    }
}
