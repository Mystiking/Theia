#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Texture includes
#include "../LoadTexture.h"
// Mesh includes
#include "../Shaders.h"
#include "../Mesh.h"
#include "../Projection.h"

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

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Make sure we always draw the nearest triangles first
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwMakeContextCurrent(window); // Initialise GLEW
    //glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    /*
    std::vector<glm::vec3> g_vertex_buffer_data = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(-1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f)
    };
    */

    std::vector<glm::vec3> g_vertex_buffer_data = {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
    };



    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(glm::vec3), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

    std::vector<unsigned short> g_indices_buffer_data = {
        0, 1, 2,
        2, 3, 1,
        0, 2, 4
    };

    std::vector<float> g_uv_buffer_data = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    GLuint indicesbuffer;
    glGenBuffers(1, &indicesbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_indices_buffer_data.size()*sizeof(unsigned short), &g_indices_buffer_data[0], GL_STATIC_DRAW);

    // Loading in shaders
    GLuint programID = LoadShaders( "../shaders/VertexTextureShader.glsl",
                                    "../shaders/FragmentTextureShader.glsl");
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    //GLuint texture = loadBMP_texture("wall.bmp", 1);
    GLuint texture = load_texture("uvtemplate.bmp", 1);
    //GLuint texture = load_texture("wall.jpg", 1);
    GLuint textureID = glGetUniformLocation(programID, "textureSampler");


    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, g_uv_buffer_data.size()*sizeof(float), &g_uv_buffer_data[0], GL_STATIC_DRAW);

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
        glUseProgram(programID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureID, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
        );

        glBindBuffer(GL_ARRAY_BUFFER, indicesbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     g_indices_buffer_data.size()*sizeof(unsigned short),
                     &g_indices_buffer_data[0],
                     GL_STATIC_DRAW);


        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
                1,
                2,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
        );

        glDrawElements(
                    GL_TRIANGLES,
                    g_indices_buffer_data.size(),
                    GL_UNSIGNED_SHORT,
                    (void*)0
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
    return 0;
}
