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
#include <algorithm>

std::vector<glm::vec3> computeNormals(
    std::vector<glm::vec3> vertices,
    std::vector<unsigned short> faces
) {
    std::vector<glm::vec3> normals;
    for (unsigned int vi = 0; vi < vertices.size(); vi++) {
        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        float count = 0.0f;
        for (unsigned int fi = 0; fi < faces.size(); fi+=3) {
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
    return normals;
}

Model plane(
    float width,
    float depth
) {
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
    for (unsigned int vi = 0; vi < vertices.size(); vi++) {
        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        float count = 0.0f;
        for (unsigned int fi = 0; fi < faces.size(); fi+=3) {
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

Model cube(
    float width,
    float height,
    float depth
) {
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

    std::vector<glm::vec3> normals = computeNormals(vertices, faces);

    Model model;
    model.indices = faces;
    model.vertices = vertices;
    model.normals = normals;
    model.model_matrix = glm::mat4(1.0f);
    return model;
}


// Probably the worst texture unwrapper in existance
void unwrap(
    Model* model
) {
    std::vector<glm::vec3> unwrapped_vertices;
    std::vector<unsigned short> unwrapped_faces;
    std::vector<glm::vec2> uvs;

    struct Triangle {
        glm::vec3 P0, P1, P2;

        Triangle(glm::vec3 Q0, glm::vec3 Q1, glm::vec3 Q2) {
            P0 = Q0; P1 = Q1, P2 = Q2;
        }
    };

    std::vector<Triangle> tris;

    for (uint i = 0; i < model->indices.size(); i+=3)
    {
        Triangle t(model->vertices[model->indices[i]], model->vertices[model->indices[i+1]], model->vertices[model->indices[i+2]]);
        tris.push_back(t);
    }

    uint index = 0;
    for (auto t : tris) {
        unwrapped_vertices.push_back(t.P0);
        unwrapped_vertices.push_back(t.P1);
        unwrapped_vertices.push_back(t.P2);

        unwrapped_faces.push_back(index++);
        unwrapped_faces.push_back(index++);
        unwrapped_faces.push_back(index++);

        uvs.push_back(glm::vec2(0, 0));
        uvs.push_back(glm::vec2(0, 1));
        uvs.push_back(glm::vec2(1, 1));
    }

    std::vector<glm::vec3> normals = computeNormals(unwrapped_vertices, unwrapped_faces);
    model->indices = unwrapped_faces;
    model->vertices = unwrapped_vertices;
    model->uvs = uvs;
    model->normals = normals;
}

uint getVertexIndex(std::vector<glm::vec3> & vertices, glm::vec3 v) {
    auto it = std::find(vertices.begin(), vertices.end(), v);
    if (it != vertices.end()) {
        // The vertex exist
        return (uint)std::distance(vertices.begin(), it);
    } else {
        // The vertex does not exists
        vertices.push_back(v);
        return vertices.size() - 1;
    }
}

void refine_model(Model* model, int num_subdivisions = 1) {
    std::vector<glm::vec3> refined_vertices;
    std::vector<unsigned short> refined_faces;
    std::vector<glm::vec3> refined_normals;


    glm::vec3 v, u, w, t;
    for (int _ = 0; _ < num_subdivisions; _++)
    {
        for (unsigned int i = 0; i < model->indices.size(); i+=3)
        {
            v = model->vertices[model->indices[i]];
            u = model->vertices[model->indices[i+1]];
            w = model->vertices[model->indices[i+2]];
            t = (v + u + w) * (1.0f / 3.0f); // Raw center of triangle points

            // Triangle 0
            refined_faces.push_back(getVertexIndex(refined_vertices, v));
            refined_faces.push_back(getVertexIndex(refined_vertices, u));
            refined_faces.push_back(getVertexIndex(refined_vertices, t));

            // Triangle 1
            refined_faces.push_back(getVertexIndex(refined_vertices, u));
            refined_faces.push_back(getVertexIndex(refined_vertices, w));
            refined_faces.push_back(getVertexIndex(refined_vertices, t));

            // Triangle 2
            refined_faces.push_back(getVertexIndex(refined_vertices, w));
            refined_faces.push_back(getVertexIndex(refined_vertices, v));
            refined_faces.push_back(getVertexIndex(refined_vertices, t));
        }
        model->vertices = refined_vertices;
        model->indices = refined_faces;
        refined_vertices.clear();
        refined_faces.clear();
    }
    model->normals = computeNormals(model->vertices, model->indices);
}

// Assumes that triangles are ordered in pairs corresponding to a face of the cube
// as they are by the "cube" function.
void subdivide_cube(Model* model, int num_subdivisions) {
    std::vector<glm::vec3> refined_vertices;
    std::vector<unsigned short> refined_faces;
    std::vector<glm::vec3> refined_normals;


    glm::vec3 v, u, w, t;
    float e0, e1, e2; // Lengths of the edges E0: v -> u, E1: u -> w, E2: w -> v
    for (int _ = 0; _ < num_subdivisions; _++)
    {
        for (unsigned int i = 0; i < model->indices.size(); i+=3)
        {
            v = model->vertices[model->indices[i]];
            u = model->vertices[model->indices[i+1]];
            w = model->vertices[model->indices[i+2]];
            e0 = glm::length(v - u);
            e1 = glm::length(u - w);
            e2 = glm::length(w - v);

            // Split the triangle on the longest edge
            if (e0 > e1) {
                if (e0 > e2) { // e0 is the largest one
                    t = (v + u) / 2.0f;

                    // Triangle 0
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                    refined_faces.push_back(getVertexIndex(refined_vertices, w));
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));

                    // Triangle 1
                    refined_faces.push_back(getVertexIndex(refined_vertices, v));
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, w));
                } else { // e2 is the largest one
                    t = (w + v) / 2.0f;

                    // Triangle 0
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                    refined_faces.push_back(getVertexIndex(refined_vertices, w));

                    // Triangle 1
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, v));
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                }
            } else {
                if (e1 > e2) { // e1 is largest
                    t = (u + w) / 2.0f;

                    // Triangle 0
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, v));

                    // Triangle 1
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, w));
                    refined_faces.push_back(getVertexIndex(refined_vertices, v));
                } else { // e2 is the largest one
                    t = (w + v) / 2.0f;

                    // Triangle 0
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                    refined_faces.push_back(getVertexIndex(refined_vertices, w));

                    // Triangle 1
                    refined_faces.push_back(getVertexIndex(refined_vertices, t));
                    refined_faces.push_back(getVertexIndex(refined_vertices, v));
                    refined_faces.push_back(getVertexIndex(refined_vertices, u));
                }
            }
        }
        model->vertices = refined_vertices;
        model->indices = refined_faces;
        refined_vertices.clear();
        refined_faces.clear();
    }
    model->normals = computeNormals(model->vertices, model->indices);
}
