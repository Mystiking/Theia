#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif

#ifndef MESHINCLUDE
#define MESHINCLUDE
#include "mesh.h"
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <algorithm>

Mesh
create_cube_mesh(float width, float height, float depth, glm::vec3 center) {
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

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] += center;
    }

    std::vector<unsigned int> faces = {
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
                    Triangle tri(vertices[faces[fi]],vertices[faces[fi+1]],vertices[faces[fi+2]]);
                    normals[vi] += tri.normal();
                    count += 1.0f;
                }
            }
        }
        normals[vi] /= count;
    }
    std::cout << "normals generated...\n";

    std::vector<Triangle> tris;
    std::vector<Triangle> tri_normals;
    for (int f = 0; f < faces.size(); f += 3) {
        Triangle tri(vertices[faces[f]], vertices[faces[f+1]], vertices[faces[f+2]]);
        Triangle normal(normals[faces[f]], normals[faces[f+1]], normals[faces[f+2]]);
        tri_normals.push_back(normal);
        tris.push_back(tri);
    }

    Mesh mesh;
    mesh.tris = tris;
    mesh.normals = tri_normals;
    mesh.indices = faces;
    mesh.vertices = vertices;
    return mesh;
}


Mesh
create_sphere_mesh(glm::vec3 center, float radius, unsigned short refine = 1) {
    float t = (1.0f + sqrt(5.0f)) / 2.0f; // The golden ratio

    std::vector<glm::vec3> vertices = {
        glm::vec3(-radius, t*radius, 0),
        glm::vec3(radius, t*radius, 0),
        glm::vec3(-radius, -t*radius, 0),
        glm::vec3(radius, -t*radius, 0),

        glm::vec3(0, -radius, t*radius),
        glm::vec3(0, radius, t*radius),
        glm::vec3(0, -radius, -t*radius),
        glm::vec3(0, radius, -t*radius),

        glm::vec3(t*radius, 0, -radius),
        glm::vec3(t*radius, 0, radius),
        glm::vec3(-t*radius, 0, -radius),
        glm::vec3(-t*radius, 0, radius),
    };

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] += center;
    }

    std::vector<unsigned int> faces = {
        0, 11, 5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,
        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,
        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,
        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    for (int i = 0; i < refine; i++) {
        std::vector<unsigned int> faces_refined;
        std::vector<glm::vec3> vertices_refined;

        glm::vec3 v0, v1, v2, v3, v4, v5;
        float v3_len, v4_len, v5_len;
        for (int j = 0; j < faces.size(); j+=3) {
            v0 = vertices[faces[j]];
            v1 = vertices[faces[j+1]];
            v2 = vertices[faces[j+2]];

            v3 = glm::vec3((v0[0] + v1[0]) / 2.0, (v0[1] + v1[1]) / 2.0, (v0[2] + v1[2]) / 2.0);
            v3_len = sqrt(v3[0]*v3[0] + v3[1]*v3[1] + v3[2]*v3[2]);
            v3[0] = v3[0] * sqrt(t*t+1)*radius / v3_len;
            v3[1] = v3[1] * sqrt(t*t+1)*radius / v3_len;
            v3[2] = v3[2] * sqrt(t*t+1)*radius / v3_len;

            v4 = glm::vec3((v1[0] + v2[0]) / 2.0, (v1[1] + v2[1]) / 2.0, (v1[2] + v2[2]) / 2.0);
            v4_len = sqrt(v4[0]*v4[0] + v4[1]*v4[1] + v4[2]*v4[2]);
            v4[0] = v4[0] * sqrt(t*t+1)*radius / v4_len;
            v4[1] = v4[1] * sqrt(t*t+1)*radius / v4_len;
            v4[2] = v4[2] * sqrt(t*t+1)*radius / v4_len;

            v5 = glm::vec3((v2[0] + v0[0]) / 2.0, (v2[1] + v0[1]) / 2.0, (v2[2] + v0[2]) / 2.0);
            v5_len = sqrt(v5[0]*v5[0] + v5[1]*v5[1] + v5[2]*v5[2]);
            v5[0] = v5[0] * sqrt(t*t+1)*radius / v5_len;
            v5[1] = v5[1] * sqrt(t*t+1)*radius / v5_len;
            v5[2] = v5[2] * sqrt(t*t+1)*radius / v5_len;

            vertices_refined.push_back(v0);
            vertices_refined.push_back(v1);
            vertices_refined.push_back(v2);
            vertices_refined.push_back(v3);
            vertices_refined.push_back(v4);
            vertices_refined.push_back(v5);

            unsigned short idx = j / 3 * 6;

            faces_refined.push_back(idx);
            faces_refined.push_back(idx + 3);
            faces_refined.push_back(idx + 5);

            faces_refined.push_back(idx + 1);
            faces_refined.push_back(idx + 4);
            faces_refined.push_back(idx + 3);

            faces_refined.push_back(idx + 2);
            faces_refined.push_back(idx + 5);
            faces_refined.push_back(idx + 4);

            faces_refined.push_back(idx + 3);
            faces_refined.push_back(idx + 4);
            faces_refined.push_back(idx + 5);
        }
        faces = faces_refined;
        vertices = vertices_refined;
    }

    std::cout << "faces and vertices generated...\n";
    std::vector<glm::vec3> normals;
    for (int vi = 0; vi < vertices.size(); vi++) {
        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        float count = 0.0f;
        for (int fi = 0; fi < faces.size(); fi+=3) {
            for (int i = 0; i < 3; i++) {
                if (faces[fi+i] == vi) {
                    Triangle tri(vertices[faces[fi]],vertices[faces[fi+1]],vertices[faces[fi+2]]);
                    normals[vi] += tri.normal();
                    count += 1.0f;
                }
            }
        }
        normals[vi] /= count;
    }
    std::cout << "normals generated...\n";

    std::vector<Triangle> tris;
    std::vector<Triangle> tri_normals;
    for (int f = 0; f < faces.size(); f += 3) {
        Triangle tri(vertices[faces[f]], vertices[faces[f+1]], vertices[faces[f+2]]);
        Triangle normal(normals[faces[f]], normals[faces[f+1]], normals[faces[f+2]]);
        tri_normals.push_back(normal);
        tris.push_back(tri);
    }

    Mesh mesh;
    mesh.tris = tris;
    mesh.normals = tri_normals;
    mesh.indices = faces;
    mesh.vertices = vertices;
    std::cout << "meshing done\n";
    return mesh;
}

std::pair<std::vector<unsigned int>, std::vector<glm::vec3>>
compute_indices_and_vertices(std::vector<Mesh> meshes, unsigned int index_count) {
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;

    for (int mi = 0; mi < meshes.size(); mi++) {
        for (int t = 0; t < meshes[mi].tris.size(); t++) {
            for (int i = 0; i < 3; i++) {
                glm::vec3 v = meshes[mi].tris[t].p[i];
                int vi;
                for (vi = 0; vi < vertices.size(); vi++) {
                    if (vertices[vi] == v) {
                        break;
                    }
                }
                std::cout << "vi: " << vi << "\n";
                indices.push_back(vi);
                if (vi == vertices.size()) {
                    vertices.push_back(v);
                }
            }
        }
    }
    std::cout << "vertices.size() = " << vertices.size() << "\n";
    std::cout << "indices.size() = " << indices.size() << "\n";

    return std::pair<std::vector<unsigned int>, std::vector<glm::vec3>>(indices, vertices);
}
