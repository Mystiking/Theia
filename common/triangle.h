#include <cmath>
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
using namespace glm;
#endif

class Triangle {
    public:
        glm::vec3 p[3];

        Triangle(glm::vec3 &v, glm::vec3 &w, glm::vec3 &u) {
            p[0] = v; p[1] = w; p[2] = u;
        };

        float area() {
            return 0.5f * sqrt(pow((p[1][1] - p[0][1]) * (p[2][2] - p[0][2]) -
                                   (p[1][2] - p[0][2]) * (p[2][1] - p[0][2]), 2) +
                               pow((p[1][2] - p[0][2]) * (p[2][0] - p[0][0]) -
                                   (p[1][0] - p[0][0]) * (p[2][2] - p[0][2]), 2) +
                               pow((p[1][0] - p[0][0]) * (p[2][1] - p[0][1]) -
                                   (p[1][1] - p[0][1]) * (p[2][0] - p[0][0]), 2));
        }

        glm::vec3 normal() {
            glm::vec3 U = p[1] - p[0];
            glm::vec3 V = p[2] - p[0];

            glm::vec3 N;
            N[0] = U[1] * V[2] - U[2] * V[1];
            N[1] = U[2] * V[0] - U[0] * V[2];
            N[2] = U[0] * V[1] - U[1] * V[0];

            return N / (sqrt(N[0]*N[0] + N[1]*N[1] + N[2]*N[2]));;
        }
};
