#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#define SCREENWIDTH   800//1920//1024
#define SCREENHEIGHT  600//1080//768
#define SCREENXCENTER 400//960//512
#define SCREENYCENTER 300//540//384
#define HALFPI 1.57f

// Mouse x and y position
double mouse_xpos, mouse_ypos, mouse_scroll_dx, mouse_scroll_dy;
// Initial camera position
// //Diablo style from above
//glm::vec3 initial_camera_position = glm::vec3(10.0f * std::cos(glm::radians(45.0f)),
//                                              10.0f,
//                                              10.0f * std::sin(glm::radians(45.0f)));
glm::vec3 initial_camera_position = glm::vec3(0., 10.0f, 10.);
// camera position
glm::vec3 camera_position = initial_camera_position;
// What the camera should look at
glm::vec3 look_at = glm::vec3(0.0, 0.0, 0.0);
// Horizontal angle of the camera (towards -Z)
float horizontal_angle = 3.186f;
// Vertical angle of the camera (towards the horizon)
float vertical_angle = -0.692f;
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
    look_at,
    up
);

// Model matrix (worldspace == model space for now)
glm::mat4 model = glm::mat4(1.0f);

// The Model * View * Projection matrix
glm::mat4 mvp = projection * view * model;
