#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#ifndef GLINCLUDE
#define GLINCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
#include "render.hpp"
#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "model.hpp"
#endif

#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include "factory/shapes.hpp"

void print_mat4(glm::mat4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void print_vec3(glm::vec3 v) {
        std::cout << v.x << " " << v.y << " " << v.z << "\n";
}

void print_quat(glm::quat q) {
        std::cout << q.x << " " << q.y << " " << q.z << " " << q.w << "\n";
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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
    glClearColor(0.133f, 0.133f, 0.133f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint model_shader = LoadShaders( "shaders/shadows.vs", "shaders/shadows.fs");
    GLuint d_shader = LoadShaders( "shaders/depthshader.vs", "shaders/depthshader.fs");
    GLuint q_shader = LoadShaders( "shaders/quad_depth_debug.vs", "shaders/quad_depth_debug.fs");

    Model cube_0 = cube(8, 8, 8);
    Model cube_1 = cube(8, 8, 8);
    subdivide_cube(&cube_0, 3);
    refine_model(&cube_1, 3);
    unwrap(&cube_0);
    unwrap(&cube_1);
    Model plane_ = cube(200, 1, 200);
    GLuint texture = load_texture("textures/uvtemplate.bmp", 0);
    //cube_0.texture = texture;
    //cube_1.texture = texture;
    cube_0.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 3.0, 0.0));
    cube_1.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(-5, 3.0, 0.0));
    plane_.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -4.0, 0.0));

    //Render render(window, {&cube_0, &cube_1, &plane_}, d_shader, q_shader, model_shader, SCREENWIDTH, SCREENHEIGHT);
    Render render(window, {&cube_1, &cube_0, &plane_}, d_shader, q_shader, model_shader, SCREENWIDTH, SCREENHEIGHT);

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
        first_person_camera(window);
        render.renderScene();
        //render.draw(projection, view, LightPosition);
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

