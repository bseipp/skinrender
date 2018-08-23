CXX = g++
CXXFLAGS = -Wall -g

density: Shader.o density.cpp density.vert density.frag
	$(CXX) $(CXXFLAGS) Shader.o density.cpp -o density -lGLEW -lglfw -framework OpenGL -framework GLUT

Shader.o: Shader.cpp Shader.hpp
	$(CXX) $(CXXFLAGS) -c Shader.cpp

clean:
	rm *.o density

run: 
	./density