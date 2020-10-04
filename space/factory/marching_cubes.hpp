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
#include <vector>

// The triangle configuration table from the original "Polygonise" article by Paul Bourke
#include "marching_cubes_tri_table.hpp"

struct GridCell {
    glm::vec3 p[8];
    float v[8];
    /* Ordering:
     *  0: (x, y, z + size)
     *  1: (x + size, y, z + size)
     *  2: (x + size, y, z)
     *  3: (x, y, z)
     *  4: (x, y + size, z + size)
     *  5: (x + size, y + size, z + size)
     *  6: (x + size, y + size, z)
     *  7: (x, y + size, z)
     *
     */
};

int triangle_index(
    GridCell grid,
    float surface_level
) {
    int configuration = 0;
    for (int i = 0; i < 8; i++) {
        if (grid.v[i] <= surface_level) {
            configuration |= 1 << i;
        }
    }
    return configuration;
}

void add_grid_cell_triangles(
    GridCell grid,
    float surface_level,
    std::vector<glm::vec3> & vertices,
    std::vector<glm::vec2> & uvs,
    std::vector<unsigned short> & indices
) {
    int configuration = triangle_index(grid, surface_level);

    // Cube is completely contained / outside of grid cell
    if (edge_table[configuration] == 0)
        return;

    glm::vec3 vert_list[12];
    /* Find the vertices where the surface intersects the cube */
    if (edge_table[configuration] & 1)
        vert_list[0] = grid.p[0]  + (surface_level - grid.v[0]) / (grid.v[1] - grid.v[0]) * (grid.p[1] - grid.p[0]);
    if (edge_table[configuration] & 2)
        vert_list[1] = grid.p[1]  + (surface_level - grid.v[1]) / (grid.v[2] - grid.v[1]) * (grid.p[2] - grid.p[1]);
    if (edge_table[configuration] & 4)
        vert_list[2] = grid.p[2]  + (surface_level - grid.v[2]) / (grid.v[3] - grid.v[2]) * (grid.p[3] - grid.p[2]);
    if (edge_table[configuration] & 8)
        vert_list[3] = grid.p[3]  + (surface_level - grid.v[3]) / (grid.v[0] - grid.v[3]) * (grid.p[0] - grid.p[3]);
    if (edge_table[configuration] & 16)
        vert_list[4] = grid.p[4]  + (surface_level - grid.v[4]) / (grid.v[5] - grid.v[4]) * (grid.p[5] - grid.p[4]);
    if (edge_table[configuration] & 32)
        vert_list[5] = grid.p[5]  + (surface_level - grid.v[5]) / (grid.v[6] - grid.v[5]) * (grid.p[6] - grid.p[5]);
    if (edge_table[configuration] & 64)
        vert_list[6] = grid.p[6]  + (surface_level - grid.v[6]) / (grid.v[7] - grid.v[6]) * (grid.p[7] - grid.p[6]);
    if (edge_table[configuration] & 128)
        vert_list[7] = grid.p[7]  + (surface_level - grid.v[7]) / (grid.v[4] - grid.v[7]) * (grid.p[4] - grid.p[7]);
    if (edge_table[configuration] & 256)
        vert_list[8] = grid.p[0]  + (surface_level - grid.v[0]) / (grid.v[4] - grid.v[0]) * (grid.p[4] - grid.p[0]);
    if (edge_table[configuration] & 512)
        vert_list[9] = grid.p[1]  + (surface_level - grid.v[1]) / (grid.v[5] - grid.v[1]) * (grid.p[5] - grid.p[1]);
    if (edge_table[configuration] & 1024)
        vert_list[10] = grid.p[2] + (surface_level - grid.v[2]) / (grid.v[6] - grid.v[2]) * (grid.p[6] - grid.p[2]);
    if (edge_table[configuration] & 2048)
        vert_list[11] = grid.p[3] + (surface_level - grid.v[3]) / (grid.v[7] - grid.v[3]) * (grid.p[7] - grid.p[3]);

    unsigned int index = vertices.size();
    // Compute triangles
    for (int i = 0; tri_table[configuration][i] != -1; i+=3) {
        // Compute edge list indices
        glm::vec3 p = vert_list[tri_table[configuration][i]];
        glm::vec3 q = vert_list[tri_table[configuration][i+1]];
        glm::vec3 u = vert_list[tri_table[configuration][i+2]];

        vertices.push_back(p);
        vertices.push_back(q);
        vertices.push_back(u);

        indices.push_back(++index);
        indices.push_back(++index);
        indices.push_back(++index);

        uvs.push_back(glm::vec2(0.0, 0.0));
        uvs.push_back(glm::vec2(0.0, 1.0));
        uvs.push_back(glm::vec2(1.0, 1.0));
    }
}
