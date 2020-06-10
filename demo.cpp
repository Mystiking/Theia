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
//#include "common/light.h"
#include "common/vboindexer.hpp"
#include "common/objloader.hpp"
#include "common/tangentspace.hpp"

#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif


int main() {
    glewExperimental = true;
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

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Make sure we always draw the nearest triangles first
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set the mouse at the center of the screen
    //glfwPollEvents();
    //glfwSetCursorPos(window, SCREENXCENTER, SCREENYCENTER);

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
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


    // Create and compile GLSL program which will take care of shadow mapping and rendering
    GLuint programID = LoadShaders( "shaders/demo.vs", "shaders/demo.fs");

    // Create and compile GLSL program which will render the rendered texture on the screen
    GLuint quad_programID = LoadShaders("shaders/passthrough.vs", "shaders/passthrough.fs");
	// Create and compile our GLSL program from the shaders
	GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "TextureSampler");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our "LightPosition" uniform
	GLuint lightID = glGetUniformLocation(programID, "LightPosition");

    // Render-to-texture
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, SCREENWIDTH, SCREENHEIGHT, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTexture, 0);

    glDrawBuffer(GL_NONE);

    std::vector<Drawable> drawables;

    //std::vector<Drawable> tiles;
    //std::vector<int> tile_ids;
    // Creation of a 100 x 100 tile world
    int num_rows = 16;
    int num_cols = 16;
    float tile_width = 1.0;
    float tile_height = 1.0;
    float x_start = -tile_width / 2.0f - (num_rows / 2.0f - 1.0f) * tile_width;
    float y_start = -tile_height / 2.0f - (num_cols / 2.0f - 1.0f) * tile_height;
    int tile_id = 0;
    for (int j = 0; j < num_cols; j++) {
        for (int i = 0; i < num_rows; i++) {
            Drawable tile(glm::translate(glm::scale(glm::mat4(1.0),
                                                    glm::vec3(2.0f, 1.0, 2.0f)),
                                         glm::vec3(x_start + tile_width * i,
                                                   0,
                                                   y_start + tile_height * j)));
            char* tex;
            switch (tile_id % 4) {
                case 0:
                    {
                        tex = "textures/white_tile.png";
                        break;
                    }
                case 1:
                    {
                        tex = "textures/red_tile.png";
                        break;
                    }
                case 2:
                    {
                        tex = "textures/blue_tile.png";
                        break;
                    }
                case 3:
                    {
                        tex = "textures/green_tile.png";
                        break;
                    }
                default:
                    {
                        tex = "textures/blue_tile.png";
                        break;
                    }

            }

            tile.create("objects/tile.obj", tex,
               programID, quad_programID,
               ViewMatrixID, ModelMatrixID, MatrixID, lightID,
               renderTexture,
               TextureID, texID);
            tile.bind_buffers();
            tiles.push_back(tile);
            tile_ids.push_back(tile_id++);
        }
    }

    Drawable player_sprite(glm::translate(glm::mat4(1.0), glm::vec3(0, 0.5, 0)));
    player_sprite.create("objects/suzanne.obj", "textures/Red.png",
               programID, quad_programID,
               ViewMatrixID, ModelMatrixID, MatrixID, lightID,
               renderTexture,
               TextureID, texID);
    player_sprite.bind_buffers();
    player.drawable = player_sprite;
    player.player_model_matrix = player_sprite.model_matrix;

    Drawable enemy_sprite(glm::translate(glm::mat4(1.0), glm::vec3(0, 0.5, 0)));
    enemy_sprite.create("objects/suzanne.obj", "textures/Blue.png",
               programID, quad_programID,
               ViewMatrixID, ModelMatrixID, MatrixID, lightID,
               renderTexture,
               TextureID, texID);
    enemy_sprite.bind_buffers();
    enemy.drawable = enemy_sprite;
    enemy.enemy_model_matrix = enemy_sprite.model_matrix;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return -1;
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

        // Step 1) Render colors and lighting information

        // Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update_camera_third_person(window);
        handle_user_input(window);

        for (auto t : tiles) {
            t.draw(view, projection, player.light);
        }
        player.drawable.draw(view, projection, player.light);
        enemy.drawable.draw(view, projection, player.light);

        // Step 2) Render to the screen using the "quads" program
		glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);

        for (auto t : tiles) {
            t.draw_quads();
        }
        player.drawable.draw_quads();
        enemy.drawable.draw_quads();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
    for (auto t : tiles) {
        t.clean_up();
    }
    player.drawable.clean_up();

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
    return 0;
}
