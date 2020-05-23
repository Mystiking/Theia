files := $(wildcard *.cpp)
include := ../Vector3.h

all:
	g++ render_example.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

texture:
	g++ render_texture_example.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

example:
	g++ render_triangle.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

color:
	g++ render_color.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

ilu:
	g++ render_ilu.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW

lighting:
	g++ render_ogl_lighting.cpp $(include) -o program  -lglut -lGLU -lGL -lglfw -lGLEW
