files := $(wildcard *.cpp)
INCLUDE := common

demo:
	g++ demo.cpp common/vboindexer.cpp common/objloader.cpp common/texture.cpp common/tangentspace.cpp -o demo -lglut -lGLU -lGL -lglfw -lGLEW
