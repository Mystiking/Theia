#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#endif
// Constants
#define SCREENWIDTH   1024
#define SCREENHEIGHT  768
#define SCREENXCENTER 512
#define SCREENYCENTER 384
#define HALFPI 1.57f
// Mouse x and y position
double mouse_xpos, mouse_ypos, mouse_scroll_dx, mouse_scroll_dy;
// Initial position of the camera
glm::vec3 initial_position = glm::vec3(0.0f, 0.0f, 5.0f);
// Position of the camera
glm::vec3 position = initial_position;
// Horizontal angle of the camera (towards -Z)
float horizontal_angle = 3.14f;
// Vertical angle of the camera (towards the horizon)
float vertical_angle = 0.0f;
// Initial Field of View
float initial_fov = 45.0f;
// Field of view of the camera
float fov = initial_fov;

// Camera perspective settings
float near = 0.1f;
float far = 100.0f;
float aspect_ratio = (float)SCREENWIDTH / (float)SCREENHEIGHT;
// Speed of the camera
float speed = 10.0f;
// Mouse speed
float mouse_speed = 0.05f;

float delta_time, current_time;
float last_time = float(glfwGetTime());

// Projection matrix
glm::mat4 projection = glm::perspective(
    glm::radians(fov),
    aspect_ratio,
    near,
    far
);

// View matrix
glm::mat4 view = glm::lookAt(
    position,
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

// Model matrix (worldspace == model space for now)
glm::mat4 model = glm::mat4(1.0f);

// The Model * View * Projection matrix
glm::mat4 mvp = projection * view * model;

// Used for normal mapping
glm::mat3 mv3x3 = glm::mat3(view * model);

void scroll_callback(GLFWwindow *window, double dx, double dy) {
    mouse_scroll_dx = dx;
    mouse_scroll_dy = dy;
}

int updateCamera(GLFWwindow *window) {
    /* Camera Orientation */
    current_time = float(glfwGetTime());
    delta_time = current_time - last_time;
    last_time = current_time;

    // Get mouse cursor position
    glfwGetCursorPos(window, &mouse_xpos, &mouse_ypos);
    // Reset cursor position for the next frame
    glfwSetCursorPos(window, SCREENXCENTER, SCREENYCENTER);
    // Update viewing angles
    horizontal_angle += mouse_speed * delta_time * (SCREENXCENTER - float(mouse_xpos));
    vertical_angle += mouse_speed * delta_time * (SCREENYCENTER - float(mouse_ypos));

    // "Direction" of the camera
    glm::vec3 direction(
        cos(vertical_angle) * sin(horizontal_angle),
        sin(vertical_angle),
        cos(vertical_angle) * cos(horizontal_angle)
    );
    // "Right" of the camera
    glm::vec3 right(
        sin(horizontal_angle - HALFPI),
        0,
        cos(horizontal_angle - HALFPI)
    );
    // "Up" of the camera
    glm::vec3 up = glm::cross(right, direction);

    /* Camera Position */
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W  ) == GLFW_PRESS) {
        position += direction * delta_time * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S  ) == GLFW_PRESS) {
        position -= direction * delta_time * speed;
    }
    // Move right
    if (glfwGetKey(window, GLFW_KEY_D  ) == GLFW_PRESS) {
        position += right * delta_time * speed;
    }
    // Move left
    if (glfwGetKey(window, GLFW_KEY_A  ) == GLFW_PRESS) {
        position -= right * delta_time * speed;
    }

    /* Camera Zoom */
    fov = initial_fov - 5 * mouse_scroll_dy;

    // Updating the perspective matrices
    projection = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    view = glm::lookAt(position, position+direction, up);
    mvp = projection * view * model;

    mv3x3 = glm::mat3(view * model);

    return 0;
}
