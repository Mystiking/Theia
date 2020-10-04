#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Image file loading tools
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

GLuint load_texture(const char* imagepath, unsigned int shaderId) {
    GLuint texture;

    glGenTextures(shaderId, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set the texture wrapping/filtering options (on the currently bound object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(imagepath, &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
    return texture;
}

GLuint loadBMP_texture(const char* imagepath, unsigned int shaderId) {
    unsigned char header[54]; // BMP files begin with a 54-bytes header
    unsigned int dataPos;     // Position in the BMP file where the data begins
    unsigned int width, height;
    unsigned int imageSize;   // width * height * 3
    // RGB data
    unsigned char* data;

    FILE* file = fopen(imagepath, "rb");
    if (!file) {
        printf("Image could not be opened.\n");
        return 0;
    }

    if (fread(header, 1, 54, file) != 54 ) { // A real BMP file has a header of 54 bytes
        printf("Not a proper BMP file.\n");
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a proper BMP file.\n");
        return 0;
    }

    // Read integers from byte array
    dataPos   = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width     = *(int*)&(header[0x12]);
    height    = *(int*)&(header[0x016]);

    if (imageSize == 0) {
        imageSize = width * height * 3; // Guess potentially missing information
    }
    if (dataPos == 0) {
        dataPos = 54; // Guess potentially missing information
    }
    // Allocate memory for the RGB data
    data = new unsigned char[imageSize];
    // Read the RGB data into data
    fread(data, 1, imageSize, file);
    fclose(file);

    // OpenGL Texture part
    GLuint textureID;
    glGenTextures(shaderId, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    free(data);

    return textureID;
}
