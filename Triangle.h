#include "Vector3.h"

class Triangle {
    private:
        Vector3 p[3];

    public:
        Triangle(Vector3 &v, Vector3 &w, Vector3 &u) {
            p[0] = v; p[1] = w; p[2] = u;
        };

        friend float area() {
            Vector3 u = *p[1] - *p[0];
        }
};
