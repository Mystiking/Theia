#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "../model.hpp"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>

Model plane(float width, float depth) {
    float x = width / 2.0f;
    float z = depth / 2.0f;

    std::vector<glm::vec3> vertices = {
        glm::vec3(x, 0, z),
        glm::vec3(x, 0, -z),
        glm::vec3(-x, 0, z),
        glm::vec3(-x, 0, -z)
    };

    std::vector<unsigned short> faces = {
        0, 2, 3,
        0, 3, 1
    };


    std::vector<glm::vec2> uvs = {
        glm::vec2(1.0, 0.0f),
        glm::vec2(1.0, 1.0f),
        glm::vec2(0.0, 0.0f),
        glm::vec2(0.0, 1.0f),
    };

    std::vector<glm::vec3> normals;
    for (int vi = 0; vi < vertices.size(); vi++) {
        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        float count = 0.0f;
        for (int fi = 0; fi < faces.size(); fi+=3) {
            for (int i = 0; i < 3; i++) {
                if (faces[fi+i] == vi) {
                    glm::vec3 U = vertices[faces[fi+2]] - vertices[faces[fi+0]];
                    glm::vec3 V = vertices[faces[fi+1]] - vertices[faces[fi+0]];
                    glm::vec3 N = glm::cross(V, U);
                    normals[vi] += N / glm::length(N);
                    count += 1.0f;
                }
            }
        }
        normals[vi] /= count;
    }
    Model model;
    model.indices = faces;
    model.vertices = vertices;
    model.normals = normals;
    model.uvs = uvs;
    model.model_matrix = glm::mat4(1.0f);
    return model;
}

Model cube(float width, float height, float depth) {
    float x = width / 2.0f;
    float y = height / 2.0f;
    float z = depth / 2.0f;

    std::vector<glm::vec3> vertices = {
        glm::vec3(-x, y, z),
        glm::vec3(-x, y, -z),
        glm::vec3(x, y, -z),
        glm::vec3(x, y, z),
        glm::vec3(x, -y, z),
        glm::vec3(x, -y, -z),
        glm::vec3(-x, -y, -z),
        glm::vec3(-x, -y, z),
    };

    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    };

    

    std::vector<unsigned short> faces = {
        3, 1, 0,
        3, 2, 1,
        4, 2, 3,
        4, 5, 2,
        4, 0, 7,
        4, 3, 0,
        7, 5, 4,
        7, 6, 5,
        0, 6, 7,
        0, 1, 6,
        6, 2, 5,
        6, 1, 2,
    };

    std::vector<glm::vec3> normals;
    for (int vi = 0; vi < vertices.size(); vi++) {
        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        float count = 0.0f;
        for (int fi = 0; fi < faces.size(); fi+=3) {
            for (int i = 0; i < 3; i++) {
                if (faces[fi+i] == vi) {
                    glm::vec3 U = vertices[faces[fi+2]] - vertices[faces[fi+0]];
                    glm::vec3 V = vertices[faces[fi+1]] - vertices[faces[fi+0]];
                    glm::vec3 N = glm::cross(V, U);
                    normals[vi] += N / glm::length(N);
                    count += 1.0f;
                }
            }
        }
        normals[vi] /= count;
    }
    Model model;
    model.indices = faces;
    model.vertices = vertices;
    model.normals = normals;
    model.uvs = uvs;
    model.model_matrix = glm::mat4(1.0f);
    return model;
}

void refine_model(Model* model) {
    
}