/* 
 * File:   properties.h
 * Author: bseipp
 *
 * Created on August 21, 2018, 1:19 PM
 */

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <iostream>

using namespace std;

class Properties 
{
public:

    class View {
    public:
        float z, fov;
        int raysPerPixel, xRes, yRes;  
    };

    class Arm {
    public:
        int x, y, z, hairDensityFactor;
        string hairColor, filename;
        float optics[6][2][3];
    };

    class Dermatascope {
    public:
        float lightRgb[3];
        float materialRgb[3];
    };

    class Room {
    public:
        int x, y, z;
        float lightRgb[3];
        float materialRgb[3];
    };
    
    View view;
    Arm arm;
    Dermatascope dermatascope;
    Room room;
    string filename;
    
    Properties(string row) 
    {
        vector<string> result;
        istringstream iss(row);

        for (string token; getline(iss, token, ',');)
        {
            result.push_back(move(token));
        }
        
        filename = result[0];
        
        view.z = stof(result[1], nullptr);
        view.fov = stof(result[2], nullptr);
        view.raysPerPixel = stoi(result[3], nullptr);
        view.xRes = stoi(result[4], nullptr);
        view.yRes = stoi(result[5], nullptr);
        
        arm.x = stoi(result[6], nullptr);
        arm.y = stoi(result[7], nullptr);
        arm.z = stoi(result[8], nullptr);
        arm.filename = result[9];
        arm.hairColor = result[10];
        arm.hairDensityFactor = stoi(result[11], nullptr);
        //Handle optics
        int counter = 12;
        for (int i = 0; i < 6; i++){
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 3; k++){
                    arm.optics[i][j][k] = stof(result[counter], nullptr);
                    counter++;
                }
            }
        }
        
        dermatascope.lightRgb[0] = stof(result[42], nullptr);
        dermatascope.lightRgb[1] = stof(result[43], nullptr);
        dermatascope.lightRgb[2] = stof(result[44], nullptr);
        dermatascope.materialRgb[0] = stof(result[45], nullptr);
        dermatascope.materialRgb[1] = stof(result[46], nullptr);
        dermatascope.materialRgb[2] = stof(result[47], nullptr);

        room.x = stof(result[48], nullptr);
        room.y = stof(result[49], nullptr);
        room.z = stof(result[50], nullptr);
        room.lightRgb[0] = stof(result[51], nullptr);
        room.lightRgb[1] = stof(result[52], nullptr);
        room.lightRgb[2] = stof(result[53], nullptr);
        room.materialRgb[0] = stof(result[54], nullptr);
        room.materialRgb[1] = stof(result[55], nullptr);
        room.materialRgb[2] = stof(result[56], nullptr);        
    }
};

#endif /* PROPERTIES_H */

