
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

// core/floatfile.cpp*
#include "floatfile.h"
#include <ctype.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <fstream>

namespace pbrt {

bool ReadFloatFile(const char *filename, std::vector<Float> *values) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        Error("Unable to open file \"%s\"", filename);
        return false;
    }

    int c;
    bool inNumber = false;
    char curNumber[32];
    int curNumberPos = 0;
    int lineNumber = 1;
    while ((c = getc(f)) != EOF) {
        if (c == '\n') ++lineNumber;
        if (inNumber) {
            CHECK_LT(curNumberPos, (int)sizeof(curNumber))
                << "Overflowed buffer for parsing number in file: " << filename
                << ", at line " << lineNumber;
            if (isdigit(c) || c == '.' || c == 'e' || c == '-' || c == '+')
                curNumber[curNumberPos++] = c;
            else {
                curNumber[curNumberPos++] = '\0';
                values->push_back(atof(curNumber));
                inNumber = false;
                curNumberPos = 0;
            }
        } else {
            if (isdigit(c) || c == '.' || c == '-' || c == '+') {
                inNumber = true;
                curNumber[curNumberPos++] = c;
            } else if (c == '#') {
                while ((c = getc(f)) != '\n' && c != EOF)
                    ;
                ++lineNumber;
            } else if (!isspace(c)) {
                Warning("Unexpected text found at line %d of float file \"%s\"",
                        lineNumber, filename);
            }
        }
    }
    fclose(f);
    return true;
}

bool ReadSkinCharFile(const char *filename, std::vector<Float> *tissue_type) {
    std::ifstream fis(filename);
    char cc;
    while(fis.get(cc)) {
	tissue_type->push_back((int)cc);
    }
    fis.close();
    return true;
    /*
    FILE *f = fopen(filename, "r");
    if (!f) {
        Error("Unable to open file \"%s\"", filename);
        return false;
    }
    int c;
    while ((c = getc(f)) != EOF) {
	tissue_type->push_back((int)c);
    }
    fclose(f);
    return true;
    */
}

bool ReadSkinFloatFile(const char *filename,
			std::vector<Float> *trans_r,
			std::vector<Float> *trans_g,
			std::vector<Float> *trans_b,
			std::vector<Float> *scat_r,
			std::vector<Float> *scat_g,
			std::vector<Float> *scat_b) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        Error("Unable to open file \"%s\"", filename);
        return false;
    }

    int c;
    bool inNumber = false;
    char curNumber[32];
    int curNumberPos = 0;
    int lineNumber = 1;
    bool trans_filler = true;
    bool block_read = false;
    int local_counter = 1;
    float rgb_block[6];
    int block_counter = 0;
    int channel_counter = 0;
    float conv_number;
    while ((c = getc(f)) != EOF) {
        if (c == '\n') ++lineNumber;
        if (inNumber) {
            CHECK_LT(curNumberPos, (int)sizeof(curNumber))
                << "Overflowed buffer for parsing number in file: " << filename
                << ", at line " << lineNumber;
            if (isdigit(c) || c == '.' || c == 'e' || c == '-' || c == '+')
                curNumber[curNumberPos++] = c;
            else {
                curNumber[curNumberPos++] = '\0';
		conv_number = atof(curNumber);

		if(trans_filler){
		    if (channel_counter == 0) {
			trans_r->push_back(conv_number);
		    } else if (channel_counter == 1) {
			trans_g->push_back(conv_number);
		    } else if (channel_counter == 2) {
			trans_b->push_back(conv_number);
		    }
		    local_counter++;
		    channel_counter++;
		} else {
		    if (channel_counter == 0) {
			scat_r->push_back(conv_number);
		    } else if (channel_counter == 1) {
			scat_g->push_back(conv_number);
		    } else if (channel_counter == 2) {
			scat_b->push_back(conv_number);
		    }
		    local_counter--;
		    channel_counter++;
		}
		if(local_counter == 4){
		    trans_filler = false;
		    channel_counter = 0;
		} else if (local_counter == 1) {
		    trans_filler = true;
		    channel_counter = 0;
		}
		inNumber = false;
                curNumberPos = 0;
            }
        } else {
            if (isdigit(c) || c == '.' || c == '-' || c == '+') {
                inNumber = true;
                curNumber[curNumberPos++] = c;
            } else if (c == '#') {
                while ((c = getc(f)) != '\n' && c != EOF)
                    ;
                ++lineNumber;
            } else if (!isspace(c)) {
                Warning("Unexpected text found at line %d of float file \"%s\"",
                        lineNumber, filename);
            }
        }
    }
    fclose(f);
    return true;
}

bool WriteSkinFloatFile(const char *filename,
			std::vector<Float> *trans_r) {
    FILE *f = fopen(filename, "w+");
    if (!f) {
        Error("Unable to open file \"%s\"", filename);
        return false;
    }
    std::vector<Float>::iterator itr;
    printf("\nFile start\n");
    for(itr = trans_r->begin(); itr < trans_r->end(); ++itr){
	fprintf(f,"%f ",*itr);
	//f<<*itr<<" ";
    }
    printf("\nFile end\n");
    fclose(f);

}


}  // namespace pbrt
