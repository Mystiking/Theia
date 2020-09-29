#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include "skeleton.hpp"

class Chain {
    public:
        std::vector<int> bones;  // Indices of all bones that are part of the chain
        Skeleton skeleton;       // Reference to skeleton holding the bones
};
