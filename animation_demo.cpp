#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Common includes
//#include "common/texture.hpp"
//#include "common/load_texture.h"
//#include "common/shaders.h"
//#include "common/controls.h"
//#include "common/light.h"
//#include "common/vboindexer.hpp"
//#include "common/objloader.hpp"
//#include "common/tangentspace.hpp"

#//include "common/light.h"
//#include "common/camera.h"
#include "common/render.hpp"
#include "common/controls.h"
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

    bool first_person = true;
    if (first_person) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // Get mouse cursor position
        glfwGetCursorPos(window, &mouse_xpos, &mouse_ypos);
        // Reset cursor position for the next frame
        glfwSetCursorPos(window, SCREENXCENTER, SCREENYCENTER);
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
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint model_shader = LoadShaders( "shaders/demo.vs", "shaders/demo.fs");

    /*
    std::vector<Model*> models;
    Model player_model("objects/suzanne.obj", "textures/Red.png", model_shader);
    Player player(glm::vec3(0.0f), glm::vec3(0.0f, 10.0f, 0.0f), player_model);

    Model enemy_model("objects/suzanne.obj", "textures/Blue.png", model_shader);
    Enemy enemy(glm::vec3(0.0f), &player, enemy_model);
    std::vector<Enemy*> enemies = { &enemy };

    models.push_back(&player.player_model);
    models.push_back(&enemy.enemy_model);
    */

    std::vector<Model*> models;
    Model stick_arm("objects/stick_arm.dae", "textures/Green.png", model_shader);
    stick_arm.model_matrix = glm::mat4(1.0f);
    Model ground("objects/tile.dae", "textures/checkers.png", model_shader);
    ground.model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 1.0f, 100.0f));
    models = { &ground, &stick_arm };
    Render render(window, models, "shaders/passthrough.vs", "shaders/passthrough.fs", SCREENWIDTH, SCREENHEIGHT);
    render.add_shader(model_shader);

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
        // Game Logic
        if (first_person) {
            first_person_camera(window);
        } else {
            //handle_user_input(window, &player, enemies);
        }
        // Draw scene
        render.draw(projection, view, LightPosition);
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

    render.clean_up();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}
