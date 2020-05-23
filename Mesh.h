#include <cmath>
#include <vector>
#include <ostream>
#include "Triangle.h"

class Mesh {
    public:
        std::vector<Triangle> tris;
        std::vector<Triangle> normals;

        Mesh(std::vector<Triangle> triangles) {
            tris = triangles;
        }

        Mesh() { }

        float area() {
            float a = 0.0f;
            for (int i = 0; i < (int)tris.size(); i++) {
                a += tris[i].area();
            }
            return a;
        }

        friend class Triangle;

        std::vector<Triangle> get_triangles() {
            return tris;
        }
};
