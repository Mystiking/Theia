#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#ifndef KEYFRAMEINCLUDE
#define KEYFRAMEINCLUDE
#include "key_frame.hpp"
#endif

#ifndef COLLADALOADERINCLUDE
#define COLLADALOADERINCLUDE
#include "../../collada_parser/collada_loader.hpp"
#endif

class Animation {
    public:
        float duration; // Length in seconds
        std::vector<KeyFrame> frames;

        Animation() {}

        Animation(std::vector<KeyFrame> frames, float duration) {
            this->frames = frames;
            this->duration = duration;
        }

        Animation(std::string animation_file) {
            ColladaLoader *collada_loader = new ColladaLoader();

            AnimationData animation_data = collada_loader->load_animation(animation_file);
            /* Prepare the key frame buffer */
            this->frames.resize(animation_data.key_frames.size());
            this->duration = animation_data.duration;
            /* Load in key frames */
            for (int i = 0; i < (int)animation_data.key_frames.size(); i++) {
                KeyFrameData key_frame_data = animation_data.key_frames[i];
                std::vector<JointTransformData> pose_data = key_frame_data.joint_transforms;
                std::map<std::string, JointTransform> pose;
                for (auto joint_transform : pose_data) {
                    glm::vec3 joint_position = glm::vec3(joint_transform.joint_local_transform[3][0],
                                                         joint_transform.joint_local_transform[3][1],
                                                         joint_transform.joint_local_transform[3][2]);
                    glm::quat joint_rotation = glm::quat_cast(glm::mat3(joint_transform.joint_local_transform));
                    pose.insert({joint_transform.joint_name_id, JointTransform(joint_position, joint_rotation)});
                }
                this->frames[i] = KeyFrame(key_frame_data.time_stamp, pose);
            }


            delete collada_loader;
        }
};
