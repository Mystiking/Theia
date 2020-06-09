#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
// Own libraries
#include "drawable.h"
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


glm::vec3 monkey_pos = glm::vec3(0.0f, 0.0f, 0.0f);

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
    monkey_pos,
    up
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

int update_camera_first_person(GLFWwindow *window) {
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

    /* Camera position update */
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

    // Updating the perspective matrices
    projection = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    view = glm::lookAt(camera_position, camera_position+direction, up);
    mvp = projection * view * model;

    mv3x3 = glm::mat3(view * model);

    return 0;
}

// A third-person camera following the player
int update_camera_third_person(GLFWwindow *window) {
    /* Camera Orientation */
    current_time = float(glfwGetTime());
    delta_time = current_time - last_time;
    last_time = current_time;

    // Get mouse cursor position
    glfwGetCursorPos(window, &mouse_xpos, &mouse_ypos);
    // Limit cursor within current window area
    mouse_xpos = std::max(std::min((int)mouse_xpos, SCREENWIDTH), 0);
    mouse_ypos = std::max(std::min((int)mouse_ypos, SCREENHEIGHT), 0);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

        glm::vec4 view_port_data = glm::vec4(0, 0, SCREENWIDTH, SCREENHEIGHT);
        glm::vec3 p_near = glm::vec3(mouse_xpos, SCREENHEIGHT - mouse_ypos - 1.0, 0.0);
        glm::vec3 un_near = glm::unProject(p_near, view, projection, view_port_data);
        glm::vec3 p_far= glm::vec3(mouse_xpos, SCREENHEIGHT - mouse_ypos - 1.0, 1.0);
        glm::vec3 un_far = glm::unProject(p_far, view, projection, view_port_data);
        glm::vec3 un_dir = un_far - un_near;
        float w = - un_near.y / un_dir.y;
        glm::vec3 un_int = un_near + un_dir * w;
        un_int.z *= -1;
        std::cout << "World cursor coord=(" << un_int.x << ", " << un_int.y << ", " << un_int.z << ")\n";

        glm::vec4 pos_projected = mvp * glm::vec4(monkey_pos.x, monkey_pos.y, monkey_pos.z, 1.0);
        float p_screenx = (pos_projected.x * 0.5 + 0.5) * SCREENWIDTH;
        float p_screeny = (pos_projected.y * 0.5 + 0.5) * SCREENHEIGHT;
        std::cout << "Player pos=(" << p_screenx << ", " << p_screeny << ")\n";
        std::cout << "Mouse pos=(" << mouse_xpos << ", " << mouse_ypos << ")\n";

        glm::vec2 mouse_dir = glm::vec2((float)mouse_xpos, (float)mouse_ypos) - glm::vec2(p_screenx, p_screeny);
        mouse_dir /= glm::length(mouse_dir);
        glm::vec2 rot_mouse_dir = glm::vec2(std::cos(glm::radians(-45.0f)) * mouse_dir.x -
                                            std::sin(glm::radians(-45.0f)) * mouse_dir.y,
                                            std::sin(glm::radians(-45.0f)) * mouse_dir.x +
                                            std::cos(glm::radians(-45.0f)) * mouse_dir.y);

        glm::vec3 pos_update = glm::vec3(rot_mouse_dir.x, 0, rot_mouse_dir.y);
        //monkey_pos += pos_update * delta_time * speed;
        //Lightcamera_position += pos_update * delta_time * speed;
        camera_position = monkey_pos + initial_camera_position;
    }


    // Updating the view matrix
    view = glm::lookAt(camera_position, monkey_pos, up);
    mvp = projection * view * model;

    mv3x3 = glm::mat3(view * model);

    return 0;
}
