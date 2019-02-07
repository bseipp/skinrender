##################
#     Macros     #
##################
CC = g++
CCFLAGS = -Wall -std=c++0x
CC_FLAGS_APPEND = -pthread -std=c++0x

SCENE_COMPONENTS = src/scene_components
SCENE_DESCRIPTION = src/scene_description/properties.h
CONFIG = src/config/config.h
UTILITY = src/utility/vec2.h
BUILD_DESTINATION = build

RNG = src/dart_throwing/RNG.cpp 
PD_SAMPLING = src/dart_throwing/PDSampling.cpp 
RANGE_LIST = src/dart_throwing/RangeList.cpp 
SCALLOPED_SECTOR = src/dart_throwing/ScallopedSector.cpp
DARTS = $(BUILD_DESTINATION)/RNG.o $(BUILD_DESTINATION)/PDSampling.o $(BUILD_DESTINATION)/RangeList.o $(BUILD_DESTINATION)/ScallopedSector.o

FILES = $(BUILD_DESTINATION)/hair.o $(BUILD_DESTINATION)/darts.a $(BUILD_DESTINATION)/density.o $(BUILD_DESTINATION)/noise.o $(BUILD_DESTINATION)/arm.o $(BUILD_DESTINATION)/room.o $(BUILD_DESTINATION)/dermatoscope.o $(BUILD_DESTINATION)/view.o $(SCENE_DESCRIPTION)




##################
#     Driver     #
##################
driver: src/driver.cpp $(FILES)
	$(CC) $(CCFLAGS) $(CC_FLAGS_APPEND) src/driver.cpp $(FILES) -o bpbrt



##################
#  Source files  #
##################
$(BUILD_DESTINATION)/hair.o: $(SCENE_COMPONENTS)/src/hair.cpp $(SCENE_COMPONENTS)/header/hair.h $(BUILD_DESTINATION)/darts.a $(CONFIG)
	$(CC) $(CCFLAGS) -c $(SCENE_COMPONENTS)/src/hair.cpp -o $(BUILD_DESTINATION)/hair.o
	
$(BUILD_DESTINATION)/arm.o: $(SCENE_COMPONENTS)/src/arm.cpp $(SCENE_COMPONENTS)/header/arm.h $(CONFIG)
	$(CC) $(CCFLAGS) -c $(SCENE_COMPONENTS)/src/arm.cpp -o $(BUILD_DESTINATION)/arm.o	
	
$(BUILD_DESTINATION)/room.o: $(SCENE_COMPONENTS)/src/room.cpp $(SCENE_COMPONENTS)/header/room.h $(CONFIG)
	$(CC) $(CCFLAGS) -c $(SCENE_COMPONENTS)/src/room.cpp -o $(BUILD_DESTINATION)/room.o	

$(BUILD_DESTINATION)/dermatoscope.o: $(SCENE_COMPONENTS)/src/dermatoscope.cpp $(SCENE_COMPONENTS)/header/dermatoscope.h $(CONFIG)
	$(CC) $(CCFLAGS) -c $(SCENE_COMPONENTS)/src/dermatoscope.cpp -o $(BUILD_DESTINATION)/dermatoscope.o	

$(BUILD_DESTINATION)/view.o: $(SCENE_COMPONENTS)/src/dermatoscope.cpp $(SCENE_COMPONENTS)/header/view.h $(CONFIG)
	$(CC) $(CCFLAGS) -c $(SCENE_COMPONENTS)/src/view.cpp -o $(BUILD_DESTINATION)/view.o

$(BUILD_DESTINATION)/density.o: $(SCENE_COMPONENTS)/src/density.cpp $(SCENE_COMPONENTS)/header/density.h $(BUILD_DESTINATION)/noise.o $(UTILITY) $(CONFIG)
	$(CC) $(CCFLAGS) $(CC_FLAGS_APPEND) -c $(SCENE_COMPONENTS)/src/density.cpp -o $(BUILD_DESTINATION)/density.o

$(BUILD_DESTINATION)/noise.o: $(SCENE_COMPONENTS)/src/noise.cpp $(SCENE_COMPONENTS)/header/noise.h $(UTILITY) $(CONFIG) 
	$(CC) $(CCFLAGS) $(CC_FLAGS_APPEND) -c $(SCENE_COMPONENTS)/src/noise.cpp -o $(BUILD_DESTINATION)/noise.o	
	
	
	
##################
#   Dart files   #
##################	
$(BUILD_DESTINATION)/darts.a: $(DARTS)
	ar rvs $(BUILD_DESTINATION)/darts.a $(DARTS)	
	
$(BUILD_DESTINATION)/RNG.o: $(RNG)
	$(CC) $(CCFLAGS) -c $(RNG) -o $(BUILD_DESTINATION)/RNG.o
	
$(BUILD_DESTINATION)/PDSampling.o: $(PD_SAMPLING)
	$(CC) $(CCFLAGS) -c $(PD_SAMPLING) -o $(BUILD_DESTINATION)/PDSampling.o
	
$(BUILD_DESTINATION)/RangeList.o: $(RANGE_LIST)
	$(CC) $(CCFLAGS) -c $(RANGE_LIST) -o $(BUILD_DESTINATION)/RangeList.o
	
$(BUILD_DESTINATION)/ScallopedSector.o: $(SCALLOPED_SECTOR)
	$(CC) $(CCFLAGS) -c $(SCALLOPED_SECTOR) -o $(BUILD_DESTINATION)/ScallopedSector.o



##################
# Dummy Commands #
##################
clean:
	rm $(BUILD_DESTINATION)/*.o $(BUILD_DESTINATION)/*.a bpbrt
	

