#include <math.h>
#define SCREENWIDTH 1024
#define SCREENHEIGHT 768

#include <glm/gtc/matrix_transform.hpp>

float fNear = 0.1f;
float fFar = 100.0f;
float fFov = 90.0f;
float fAspectRatio = (float)SCREENWIDTH / (float)SCREENHEIGHT;
float fFovrad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

glm::mat4 Projection = glm::perspective(
    fFovrad,
    fAspectRatio,
    fNear,
    fFar
);

glm::vec3 CameraLocation = glm::vec3(0, 0, 25);

glm::mat4 View = glm::lookAt(
    CameraLocation, // Camera location in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down
);

glm::mat4 Model = glm::mat4(1.0f);

glm::mat4 mvp = Projection * View * Model;
