#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Mesh includes
#include "../Shaders.h"
#include "../Mesh.h"
#include "../Projection.h"

#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif

/*
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};
*/


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
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f)
    };
    */

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(glm::vec3), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

    std::vector<unsigned short> g_indices_buffer_data = {
        3, 1, 0,
        3, 2, 1,
        4, 2, 3,
        4, 5, 2,
        4, 0, 7,
        4, 3, 0,
        7, 5, 4,
        7, 6, 5,
        0, 6, 7,
        0, 1, 6,
        6, 2, 5,
        6, 1, 2
    };


    const static GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
    };

    static GLfloat g_line_colors_buffer_data[12*6*3];
    for (int i = 0; i < 12*6*3; i++) {
        g_line_colors_buffer_data[i] = 0.0f;
    }

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    GLuint indicesbuffer;
    glGenBuffers(1, &indicesbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_indices_buffer_data.size()*sizeof(unsigned short), &g_indices_buffer_data[0], GL_STATIC_DRAW);

    // Loading in shaders
    GLuint programID = LoadShaders( "../shaders/VertexShader.glsl",
                                    "../shaders/FragmentShader.glsl");
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	double lastTime = glfwGetTime();
	int nbFrames = 0;
    bool draw_wireframe = false;
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
        if (draw_wireframe) {

            // Draw lines
            // 2nd attribute buffer : colors
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(g_line_colors_buffer_data),
                         g_line_colors_buffer_data,
                         GL_STATIC_DRAW);

            glVertexAttribPointer(
                1,                                // attribute index
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glBindBuffer(GL_ARRAY_BUFFER, indicesbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         g_indices_buffer_data.size()*sizeof(unsigned short),
                         &g_indices_buffer_data[0],
                         GL_STATIC_DRAW);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glDrawElements(
                    GL_TRIANGLES,
                    g_indices_buffer_data.size(),
                    GL_UNSIGNED_SHORT,
                    (void*)0
            );
        } else {
            // Draw polygons
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(g_color_buffer_data),
                         g_color_buffer_data,
                         GL_STATIC_DRAW);
            glVertexAttribPointer(
                1,                                // attribute index.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glBindBuffer(GL_ARRAY_BUFFER, indicesbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         g_indices_buffer_data.size()*sizeof(unsigned short),
                         &g_indices_buffer_data[0],
                         GL_STATIC_DRAW);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(10.0, 10.0);

            glDrawElements(
                    GL_TRIANGLES,
                    g_indices_buffer_data.size(),
                    GL_UNSIGNED_SHORT,
                    (void*)0
            );
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
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
