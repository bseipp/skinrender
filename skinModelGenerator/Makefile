buildPbrt: buildPbrtInput.cpp
	g++ -Wall -std=c++0x buildPbrtInput.cpp ./dart_throwing/RNG.cpp ./dart_throwing/PDSampling.cpp ./dart_throwing/RangeList.cpp ./dart_throwing/ScallopedSector.cpp -o bpbrt

run: bpbrt
	./bpbrt

clean:
	rm bpbrt *.exr *.pbrt