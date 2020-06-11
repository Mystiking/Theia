files := $(wildcard *.cpp)
INCLUDE := common

animation_demo:
	g++ animation_demo.cpp common/vboindexer.cpp common/objloader.cpp common/texture.cpp -o demo -lglut -lGLU -lGL -lglfw -lGLEW
