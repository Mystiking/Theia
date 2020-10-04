files := $(wildcard *.cpp)
INCLUDE := common

space_demo:
	g++ demo.cpp -o demo -lglut -lGLU -lGL -lglfw -lGLEW
