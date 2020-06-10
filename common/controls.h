#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
// Own libraries
#include "player.h"
#include "light.h"
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
#define SCREENWIDTH   800//1920//1024
#define SCREENHEIGHT  600//1080//768
#define SCREENXCENTER 400//960//512
#define SCREENYCENTER 300//540//384
#define HALFPI 1.57f
// Mouse x and y position
double mouse_xpos, mouse_ypos, mouse_scroll_dx, mouse_scroll_dy;
// Initial camera position
glm::vec3 initial_camera_position = glm::vec3(10.0f * std::cos(glm::radians(45.0f)),
                                              10.0f,
                                              10.0f * std::sin(glm::radians(45.0f)));
// camera position
glm::vec3 camera_position = initial_camera_position;
// Horizontal angle of the camera (towards -Z)
float horizontal_angle = 3.14f;
// Vertical angle of the camera (towards the horizon)
float vertical_angle = 0.0f;
// Initial Field of View
float initial_fov = 90.0f;
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

std::vector<Drawable> tiles;
std::vector<int> tile_ids;


// Projection matrix
glm::mat4 projection = glm::perspective(
    glm::radians(fov),
    aspect_ratio,
    near,
    far
);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

// View matrix
glm::mat4 view = glm::lookAt(
    camera_position,
    player.position,
    up
);

// Model matrix (worldspace == model space for now)
glm::mat4 model = glm::mat4(1.0f);

// The Model * View * Projection matrix
glm::mat4 mvp = projection * view * model;

// Used for normal mapping
glm::mat3 mv3x3 = glm::mat3(view * model);

// How many frames do we want between each click?
int frames_pr_click = 10;
int frames_since_click = frames_pr_click;

void scroll_callback(GLFWwindow *window, double dx, double dy) {
    mouse_scroll_dx = dx;
    mouse_scroll_dy = dy;
}

int handle_user_input(GLFWwindow * window) {
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
        player.set_desired_position(cursor_world);
        frames_since_click = 0;

    }
    // Count up frames since last click
    frames_since_click++;

    player.update(delta_time);
    enemy.update(delta_time);
    /* Update Camera Position */
    camera_position = player.position + initial_camera_position;
    /* Update View Matrix */
    view = glm::lookAt(camera_position, player.position, up);

    return 0;
}
