#include "Vector3.h"

class Triangle {
    private:
        Vector3 p[3];

    public:
        Triangle(Vector3 &v, Vector3 &w, Vector3 &u) {
            p[0] = v; p[1] = w; p[2] = u;
        };

        float area() {
            return 0.5f * sqrt(pow((p[1].y - p[0].y) * (p[2].z - p[0].z) -
                                   (p[1].z - p[0].z) * (p[2].y - p[0].z), 2) +
                               pow((p[1].z - p[0].z) * (p[2].x - p[0].x) -
                                   (p[1].x - p[0].x) * (p[2].z - p[0].z), 2) +
                               pow((p[1].x - p[0].x) * (p[2].y - p[0].y) -
                                   (p[1].y - p[0].y) * (p[2].x - p[0].x), 2));
        }
};
