#include <cmath>
#include <vector>
#include <ostream>
#include "triangle.h"
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif

class Mesh {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        Mesh() { }



};
