buildPbrt: src/buildPbrtInput.cpp src/properties.h
	g++ -Wall -pthread -std=c++0x src/buildPbrtInput.cpp src/dart_throwing/RNG.cpp src/dart_throwing/PDSampling.cpp src/dart_throwing/RangeList.cpp src/dart_throwing/ScallopedSector.cpp src/properties.h

clean:
	rm bpbrt
