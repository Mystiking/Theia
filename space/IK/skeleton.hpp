#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include "bone.hpp"

class Skeleton {
    public:
        std::vector<Bone> bones;  // Bones of the skeleton

        bool has_root() {
            return (bones.size() > 0);
        }

        bool has_bone(int idx) {
            return (idx >= 0) && (this->bones.size() > (unsigned int)idx);
        }
};
