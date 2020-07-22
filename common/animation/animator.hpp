#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#ifndef ANIMATIONINCLUDE
#define ANIMATIONINCLUDE
#include "animation.hpp"
#endif
#ifndef ANIMATEDMODELINCLUDE
#define ANIMATEDMODELINCLUDE
#include "../animated_model.hpp"
#endif
#ifndef JOINTINCLUDE
#define JOINTINCLUDE
#include "joint.hpp"
#endif

class Animator {
    public:
        Animation *current_animation;
        float animation_time = 0.0f;

        AnimatedModel *model;

        Animator() {}

        Animator(Animation *animation, AnimatedModel *model) {
            this->current_animation = animation;
            this->model = model;
        }

        void update(float delta_time) {
            this->increase_animation_time(delta_time);
            std::pair<KeyFrame, KeyFrame> frames = this->get_current_and_next_frame();
            float progression = this->compute_progression(frames.first, frames.second);
            std::map<std::string, JointTransform> current_pose = this->interpolate_poses(frames.first, frames.second, progression);
            apply_pose(current_pose, &this->model->joint_hierarchy, this->model->model_matrix);
        }

        void increase_animation_time(float delta_time) {
            this->animation_time += delta_time;
            if (this->animation_time > this->current_animation->duration) {
                this->animation_time = std::fmod(this->animation_time, this->current_animation->duration);
            }
        }

        std::pair<KeyFrame, KeyFrame> get_current_and_next_frame() {
            std::vector<KeyFrame> key_frames = this->current_animation->frames;
            KeyFrame first = key_frames[0];
            KeyFrame second = key_frames[0];
            for (int i = 0; i < (int)key_frames.size(); i++) {
                second = key_frames[i];
                if (second.time_stamp > this->animation_time) {
                    break;
                }
                first = key_frames[i];
            }
            return std::pair<KeyFrame, KeyFrame>(first, second);
        }

        std::map<std::string, JointTransform> interpolate_poses(KeyFrame first, KeyFrame second, float progression) {
            std::map<std::string, JointTransform> current_pose;
            for (std::string joint_name : extract_keys(first.pose)) {
                JointTransform first_transform = first.pose[joint_name];
                JointTransform second_transform = second.pose[joint_name];

                JointTransform current_transform = JointTransform::interpolate(first_transform, second_transform, progression);
                current_pose.insert({joint_name, current_transform});
            }
            return current_pose;
        }

        std::vector<std::string> extract_keys(std::map<std::string, JointTransform> const& input_map) {
            std::vector<std::string> retval;
            for (auto const& element : input_map) {
                retval.push_back(element.first);
            }
            return retval;
        }

        void apply_pose(std::map<std::string, JointTransform> pose, Joint *joint, glm::mat4 parent_transform) {
            glm::mat4 current_local_transform = pose[joint->get_name()].get_local_transform();
            glm::mat4 current_transform = parent_transform * current_local_transform;
            for (int i = 0; i < (int)joint->children.size(); i++) {
                apply_pose(pose, &joint->children[i], current_transform);
            }
            current_transform = current_transform * joint->inverse_bind_transform;
            joint->set_animated_transform(current_transform);
        }

        float compute_progression(KeyFrame first, KeyFrame second) {
            float total_time = second.time_stamp - first.time_stamp;
            return total_time > 0.0f ?(animation_time - first.time_stamp) / total_time : 0.0f;
        }
};
