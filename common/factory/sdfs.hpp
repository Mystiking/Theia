#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <algorithm>


float sphere(glm::vec3 p, glm::vec3 centre, float radius) {
    return glm::length(p-centre) - radius;
}

float box(glm::vec3 p, float w, float h, float d) {
    glm::vec3 q = glm::vec3(std::abs(p[0])-w/2.0f, std::abs(p[1])-h/2.0f, std::abs(p[2])-d/2.0f);
    if (q[0] > 0.0f && q[1] > 0.0f && q[2] > 0.0f) {
        return std::sqrt(glm::dot(q, q));
    } else {
        return std::max(q[0], std::max(q[1], q[2]));
    }
}


float box(glm::vec3 p, glm::vec3 centre, glm::vec3 size) {
    glm::vec3 offset = glm::abs(p-centre) - size;
    float unsigned_dist = glm::length(glm::max(offset, glm::vec3(0)));
    glm::vec3 inside_box_dists = glm::min(offset, glm::vec3(0));
    return unsigned_dist + std::max(inside_box_dists.x, std::max(inside_box_dists.y, inside_box_dists.z));
}

float sdf_union(float dist_a, float dist_b) {
    return std::min(dist_a, dist_b);
}

float sdf_intersection(float dist_a, float dist_b) {
    return std::max(dist_a, dist_b);
}

float sdf_difference(float dist_a, float dist_b) {
    return std::max(dist_a, -dist_b);
}
