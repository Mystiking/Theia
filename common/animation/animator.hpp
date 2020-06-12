#ifndef ANIMATOR
#define ANIMATOR
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include "animation.hpp"
#include "animated_model.hpp"

class Animator{
    public:
        AnimatedModel *animated_model;

        Animation *current_animation;
        float animation_time = 0.0f;

        bool animation = false;

        Animator() {}

        Animator(AnimatedModel *animated_model) {
            this->animated_model = animated_model;
        }

        void update(float delta_time) {
            if (!this->animation) {
                return;
            }

            increase_animation_time(delta_time);
            std::map<std::string, glm::mat4> currentPose = get_current_pose();
            apply_pose_to_joint(currentPose, &animated_model->root, animated_model->model_matrix);
        }

        void increase_animation_time(float delta_time) {
            this->animation_time += delta_time;
            if (this->animation_time > this->current_animation->duration) {
                this->animation_time = std::fmod(this->animation_time, this->current_animation->duration);
            }
        }

        std::map<std::string, glm::mat4> get_current_pose() {
            std::pair<KeyFrame, KeyFrame> frames = get_current_and_next_frame();
            float progression = compute_progression(frames.first, frames.second);
            return interpolate_poses(frames.first, frames.second, progression);
        }

        float compute_progression(KeyFrame first, KeyFrame second) {
            float total_time = second.time_stamp - first.time_stamp;
            return (animation_time - first.time_stamp) / total_time;
        }

        std::map<std::string, glm::mat4> interpolate_poses(KeyFrame first, KeyFrame second, float progression) {
            std::map<std::string, glm::mat4> current_pose;
            for (std::string joint_name : extract_keys(first.joint_transforms)) {
                JointTransform first_transform = first.joint_transforms[joint_name];
                JointTransform second_transform = second.joint_transforms[joint_name];
                JointTransform current_transform = JointTransform::interpolate(first_transform, second_transform, progression);
                current_pose.insert({joint_name, current_transform.get_local_transform()});
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

        std::pair<KeyFrame, KeyFrame> get_current_and_next_frame() {
            std::vector<KeyFrame> key_frames = current_animation->frames;
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

        void apply_pose_to_joint(std::map<std::string, glm::mat4> current_pose, Joint *joint, glm::mat4 parent_transform) {
            glm::mat4 current_local_transform = current_pose[joint->get_name()];
            glm::mat4 current_transform = parent_transform * current_local_transform;

            for (Joint child_joint : joint->get_children())  {
                apply_pose_to_joint(current_pose, &child_joint, current_transform);
            }

            current_transform = current_transform * joint->get_inverse_bind_tranform();
            joint->set_animated_transform(current_transform);
        }

        void do_animation(Animation *animation) {
            this->animation_time = 0.0f;
            this->animation = true;
            this->current_animation = animation;
        }
};
#endif
