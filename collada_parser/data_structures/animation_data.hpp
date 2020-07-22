#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#ifndef KEYFRAMEDATAINCLUDE
#define KEYFRAMEDATAINCLUDE
#include "key_frame_data.hpp"
#endif

#include <string>
#include <vector>

class AnimationData {
    public:
        float duration;
        std::vector<KeyFrameData> key_frames;

        AnimationData(float duration, std::vector<KeyFrameData> key_frames) {
            this->duration = duration;
            this->key_frames = key_frames;
        }
};
