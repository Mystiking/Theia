#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Common includes
#include "common/texture.hpp"
//#include "common/load_texture.h"
#include "common/shaders.h"
#include "common/controls.h"
#include "common/light.h"
#include "common/drawable.h"
#include "common/vboindexer.hpp"
#include "common/objloader.hpp"
#include "common/tangentspace.hpp"

#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif


int main() {
    //glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise Game Window\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x Anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want old opengl

    // Open a window and create its OpenGL context
    GLFWwindow *window; // Global for simplicity
    window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Game Window", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to initialise Game Window\n");
        glfwTerminate();
        return -1;
    }


    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Make sure we always draw the nearest triangles first
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, SCREENXCENTER, SCREENYCENTER);

    glfwMakeContextCurrent(window); // Initialise GLEW
    //glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.1f, 0.0f);


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "shaders/game.vs",
                                    "shaders/game.fs");

	GLuint textureID  = glGetUniformLocation(programID, "TextureSampler");

	GLuint normalTextureID  = glGetUniformLocation(programID, "NormalTextureSampler");

    GLuint mvpID = glGetUniformLocation(programID, "MVP");
    GLuint MID= glGetUniformLocation(programID, "M");
    GLuint VID = glGetUniformLocation(programID, "V");
    GLuint PID = glGetUniformLocation(programID, "P");
    GLuint MV3X3ID = glGetUniformLocation(programID, "MV3x3");
    GLuint lightPositionID = glGetUniformLocation(programID, "LightPosition_worldspace");


    std::vector<Drawable> drawables;
    for (int i = 0; i < 10; i++) {
        Drawable d(glm::translate(glm::mat4(1.0), glm::vec3(i * 1.0, 0.0, 0.0)));
        d.create("objects/bunny_simple.obj", "textures/uvtemplate.bmp", "textures/uvtemplate.tga",
                programID, lightPositionID, mvpID, MID, VID, PID, MV3X3ID, textureID, normalTextureID);
        d.bind_buffers();
        drawables.push_back(d);
    }


	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do{
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f fps\n", double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glUseProgram(programID);

        updateCamera(window);

        for (auto d : drawables) {
            d.draw(view, projection, LightPosition);
        }
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
    for (auto d : drawables) {
        d.clean_up();
    }
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
    return 0;
}
