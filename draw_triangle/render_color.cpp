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


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    std::vector<glm::vec3> vertices = {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
    };

    std::vector<unsigned short> faces = {
        0, 1, 2,
        2, 3, 1,
        0, 2, 4,
        4, 6, 2,
        4, 5, 6,
        5, 7, 6,
        7, 3, 5,
        5, 1, 3,
        1, 0, 5,
        5, 4, 0,
        3, 7, 2,
        2, 6, 7,
    };

    std::vector<glm::vec3> tri_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),

        glm::vec3(0.5f, 0.5f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.5f),
        glm::vec3(0.0f, 0.5f, 0.5f),

        glm::vec3(0.8f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.8f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.8f),

        glm::vec3(0.8f, 0.0f, 0.6f),
        glm::vec3(0.6f, 0.8f, 0.0f),
        glm::vec3(0.0f, 0.6f, 0.8f),
    };


    std::vector<GLfloat> g_color_buffer_data;
    std::vector<glm::vec3> g_vertex_buffer_data;

    for (int f = 0; f < faces.size(); f+=3) {
        std::cout << f << std::endl;
        g_vertex_buffer_data.push_back(vertices[faces[f]]);
        g_vertex_buffer_data.push_back(vertices[faces[f+1]]);
        g_vertex_buffer_data.push_back(vertices[faces[f+2]]);

        glm::vec3 color = tri_colors[f / 3];
        for (int i = 0; i < 3; i++) {
            g_color_buffer_data.push_back(color[0]);
            g_color_buffer_data.push_back(color[1]);
            g_color_buffer_data.push_back(color[2]);
        }
    }

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(glm::vec3), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

    // Loading in shaders
    GLuint programID = LoadShaders( "../shaders/VertexShader.glsl",
                                    "../shaders/FragmentShader.glsl");
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size()*sizeof(GLfloat), &g_color_buffer_data[0], GL_STATIC_DRAW);

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

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        /*
        glDrawElements(
                    GL_TRIANGLES,
                    g_indices_buffer_data.size(),
                    GL_UNSIGNED_SHORT,
                    (void*)0
        );
        */

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
    return 0;
}
