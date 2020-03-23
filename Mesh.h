#include <cmath>
#include <vector>
#include <ostream>
#include "Triangle.h"

class Mesh {
    private:
        std::vector<Triangle> tris;
    public:
        Mesh(std::vector<Triangle> triangles) {
            tris = triangles;
        }

        float area() {
            float a = 0.0f;
            for (int i = 0; i < (int)tris.size(); i++) {
                a += tris[i].area();
            }
            return a;
        }

        friend class Triangle;
};
