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
//#include "../Projection.h"
#include "../Light.h"
#include "../Controls.h"
#include "factory.h"

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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    std::vector<GLfloat> g_color_buffer_data;
    std::vector<glm::vec3> g_vertex_buffer_data;
    std::vector<glm::vec3> g_vertex_normal_buffer_data;

    // I
    Mesh I = create_cube_mesh(1.0f, 4.0f, 1.0f, glm::vec3(-4.0f, 0.0f, 0.0f));
    //Mesh I = create_sphere_mesh(glm::vec3(0.0f, 0.0f, 0.0f), 2.5f, 5);
    // L(1)
    Mesh L1 = create_cube_mesh(1.0f, 3.0f, 1.0f, glm::vec3(-2.0f, 0.5f, 0.0f));
    // L(2)
    Mesh L2 = create_cube_mesh(3.0f, 1.0f, 1.0f, glm::vec3(-1.0f, -1.5f, 0.0f));
    // U(1)
    Mesh U1 = create_cube_mesh(1.0f, 4.0f, 1.0f, glm::vec3(2.0f, 0.0f, 0.0f));
    // U(2)
    Mesh U2 = create_cube_mesh(1.0f, 1.0f, 1.0f, glm::vec3(3.0f, -1.5f, 0.0f));
    // U(3)
    Mesh U3 = create_cube_mesh(1.0f, 4.0f, 1.0f, glm::vec3(4.0f, 0.0f, 0.0f));
    // Earth
    Mesh E = create_cube_mesh(50.0f, 1.0f, 50.f, glm::vec3(0.0f, -2.5f, 0.0f));

    std::vector<Mesh> meshes = {
        I, L1, L2, U1, U2, U3, E
        //I, //E
    };

    for (int mi = 0; mi < meshes.size(); mi++) {
        for (int t = 0; t < meshes[mi].tris.size(); t++) {
            for (int i = 0; i < 3; i++) {
                g_vertex_buffer_data.push_back(meshes[mi].tris[t].p[i]);
                g_vertex_normal_buffer_data.push_back(meshes[mi].normals[t].p[i]);

                if (mi != meshes.size() - 1) {
                g_color_buffer_data.push_back(0.75f);
                g_color_buffer_data.push_back(0.0f);
                g_color_buffer_data.push_back(0.0f);
                } else {

                g_color_buffer_data.push_back(0.75f);
                g_color_buffer_data.push_back(0.75f);
                g_color_buffer_data.push_back(0.75f);
                }

            }
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
    GLuint programID = LoadShaders( "../shaders/VertexShaderOGL_lighting.glsl",
                                    "../shaders/FragmentShaderOGL_lighting.glsl");
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint MID= glGetUniformLocation(programID, "M");
    GLuint VID = glGetUniformLocation(programID, "V");
    GLuint PID = glGetUniformLocation(programID, "P");
    GLuint cameraPositionID = glGetUniformLocation(programID, "cameraPosition");
    GLuint lightPositionID = glGetUniformLocation(programID, "lightPosition_worldspace");

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size()*sizeof(GLfloat), &g_color_buffer_data[0], GL_STATIC_DRAW);


    GLuint normalbuffer;
    glGenBuffers(2, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, g_vertex_normal_buffer_data.size()*sizeof(glm::vec3), &g_vertex_normal_buffer_data[0], GL_STATIC_DRAW);

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do{
        updateCamera(window);
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
        glUniformMatrix4fv(MID, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(VID, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(PID, 1, GL_FALSE, &model[0][0]);
        glUniform3f(cameraPositionID, position[0], position[1], position[2]);
        glUniform3f(lightPositionID, LightPosition[0], LightPosition[1], LightPosition[2]);
        //glUniform3f(cameraPositionID, 0.0f, 1.0f, 1.0f);

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

        // 3rd  attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size());
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
        glDisableVertexAttribArray(2);

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
