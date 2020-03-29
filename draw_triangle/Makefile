files := $(wildcard *.cpp)
include := ../Vector3.h

all:
	g++ render_example.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

texture:
	g++ texture_example.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

example:
	g++ render_triangle.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW
