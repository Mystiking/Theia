#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
// Own libraries
#include "camera.h"
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

float delta_time, current_time;
float last_time = float(glfwGetTime());

// How many frames do we want between each click?
int frames_pr_click = 10;
int frames_since_click = frames_pr_click;

void scroll_callback(GLFWwindow *window, double dx, double dy) {
    mouse_scroll_dx = dx;
    mouse_scroll_dy = dy;
}

int first_person_camera(GLFWwindow *window) {
    // Time since last event
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
        camera_position += direction * delta_time * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S  ) == GLFW_PRESS) {
        camera_position -= direction * delta_time * speed;
    }
    // Move right
    if (glfwGetKey(window, GLFW_KEY_D  ) == GLFW_PRESS) {
        camera_position += right * delta_time * speed;
    }
    // Move left
    if (glfwGetKey(window, GLFW_KEY_A  ) == GLFW_PRESS) {
        camera_position -= right * delta_time * speed;
    }
    /* Camera Zoom */
    fov = initial_fov - 20 * mouse_scroll_dy;
    // Updating the perspective matrices
    projection = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    view = glm::lookAt(camera_position, camera_position+direction, up);

    return 0;
}
