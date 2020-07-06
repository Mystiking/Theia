#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
// Own libraries
#include "player.h"
#include "light.h"
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

int handle_user_input(GLFWwindow *window, Player* player, std::vector<Enemy*> enemies) {
    // Time since last event
    current_time = float(glfwGetTime());
    delta_time = current_time - last_time;
    last_time = current_time;
    /* User click actions:
     *  - Move to empty spot
     *  - Attack enemy at this spot
     *  - Pick-up item at this spot
    */
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && (frames_since_click >= frames_pr_click)) {
        /*  --Get world location of mouse click--
         *  Get the world coordinates of the mouse click at the near and far plane.
         *  These coordinates are then used to define a ray, and the actual world position of the
         *  cursor is then the intersection between the ground plane and this ray.
         * */
        glfwGetCursorPos(window, &mouse_xpos, &mouse_ypos);
        mouse_xpos = std::max(std::min((int)mouse_xpos, SCREENWIDTH), 0);
        mouse_ypos = SCREENHEIGHT - std::max(std::min((int)mouse_ypos, SCREENHEIGHT), 0);
        // Windows starts at (0, 0) and is of size SCREENWIDTH x SCREENHEIGHT
        glm::vec4 view_port_data = glm::vec4(0, 0, SCREENWIDTH, SCREENHEIGHT);
        glm::vec3 cursor_near = glm::vec3(mouse_xpos, mouse_ypos, 0.0);
        glm::vec3 cursor_far  = glm::vec3(mouse_xpos, mouse_ypos, 1.0);
        glm::vec3 cursor_near_world = glm::unProject(cursor_near, view, projection, view_port_data);
        glm::vec3 cursor_far_world  = glm::unProject(cursor_far, view, projection, view_port_data);
        glm::vec3 ray_direction = cursor_far_world - cursor_near_world;
        ray_direction = ray_direction / glm::length(ray_direction);
        glm::vec3 cursor_world = cursor_near_world - (cursor_near_world.y / ray_direction.y) * ray_direction;

        /* Move towards this location */
        player->set_desired_position(cursor_world);
        frames_since_click = 0;

    }
    // Count up frames since last click
    frames_since_click++;

    player->update(delta_time);
    for (Enemy *enemy : enemies) {
        enemy->update(delta_time);
    }
    /* Update Camera Position */
    camera_position = player->position + initial_camera_position;
    /* Update View Matrix */
    view = glm::lookAt(camera_position, player->position, up);

    return 0;
}
