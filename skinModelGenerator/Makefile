buildPbrt: buildPbrtInput.cpp properties.h
	g++ -Wall -pthread -std=c++0x buildPbrtInput.cpp ./dart_throwing/RNG.cpp ./dart_throwing/PDSampling.cpp ./dart_throwing/RangeList.cpp ./dart_throwing/ScallopedSector.cpp properties.h -o bpbrt

run: bpbrt
	./bpbrt

clean:
	rm bpbrt *.exr *.pbrt
