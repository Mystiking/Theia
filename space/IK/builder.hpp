#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include <string>
#include <iostream>
#include "chain.hpp"


class Builder {
    public:
        static Bone* create_root(
            Skeleton & skeleton,
            float alpha,
            float beta,
            float gamma,
            float tx,
            float ty,
            float tz,
            std::string euler_code = "ZYZ"
        ) {
            /*
            Bone root;
            root.t = glm::vec3(tx, ty, tz);
            root.alpha = alpha;
            root.beta = beta;
            root.gamma = gamma;
            root.euler_code = euler_code;
            root.children = {};
            */
            //skeleton.bones.push_back(new Bone());
            //return root;
            //return skeleton.bones[0];
        }
        /*

        static Bone add_bone(
            Skeleton & skeleton,
            int parent_idx,
            float alpha,
            float beta,
            float gamma,
            float tx,
            float ty,
            float tz,
            std::string euler_code = "ZYZ"
        ) {
            Bone end_effector;
            end_effector.index = skeleton.bones.size();
            end_effector.parent = parent_idx;
            end_effector.t = glm::vec3(tx, ty, tz);
            end_effector.alpha = alpha;
            end_effector.beta = beta;
            end_effector.gamma = gamma;
            end_effector.euler_code = euler_code;

            skeleton.bones[parent_idx].children.push_back(end_effector.index);
            skeleton.bones.push_back(end_effector);

            return end_effector;
        }

        static glm::quat prod(glm::quat qa, glm::quat qb) {
            float a = qa.w;
            float b = qb.w;
            glm::vec3 A = glm::vec3(qa.x, qa.y, qa.z);
            glm::vec3 B = glm::vec3(qb.x, qb.y, qb.z);

            float qs = a * b - glm::dot(A, B);
            glm::vec3 qv = a * B + A * b + glm::cross(A, B);

            return glm::quat(qs, qv.x, qv.y, qv.z);
        }

        static glm::quat conjugate(glm::quat q) {
            return glm::quat(q.w, -q.x, -q.y, -q.z);
        }

        static glm::vec3 rotate(glm::quat q, glm::vec3 r) {
            glm::quat qr = glm::quat(0.0, r.x, r.y, r.z);
            glm::quat q_rotated = prod(prod(q, qr), conjugate(q));
            return glm::vec3(q_rotated.x, q_rotated.y, q_rotated.z);
        }

        static glm::mat4 get_rotation_matrix(glm::quat q) {
            glm::mat4 R =
            {
              {1.0f - 2.0f*q[2]*q[2] - 2.0f*q[3]*q[3],
              2.0f*q[1]*q[2] - 2.0f*q[0]*q[3],
              2.0f*q[1]*q[3] + 2.0f*q[0]*q[2],
              0.0f},
              {
                2.0f*q[1]*q[2] + 2.0f*q[0]*q[3],
                1.0f - 2.0f*q[1]*q[1] - 2.0f*q[3]*q[3],
                2.0f*q[2]*q[3] - 2.0f*q[0]*q[1],
                0.0f
              },
              {
                2.0f*q[1]*q[3] - 2.0f*q[0]*q[2],
                2.0f*q[2]*q[3] + 2.0f*q[0]*q[1],
                1.0f - 2.0f*q[1]*q[1] - 2.0f*q[2]*q[2],
                0.0f
              },
              {
                0.0f,
                0.0f,
                0.0f,
                1.0f
              },
            };
            return R;
        }

        static void update_skeleton(Skeleton & skeleton) {
            __update_bone(skeleton.bones[0], skeleton);
        }

        static void set_angle(int idx, float value, Skeleton & skeleton) {
            int k = idx / 3;
            int offset = idx % 3;
            switch (offset) {
                case 0:
                    skeleton.bones[k].alpha = value;
                    break;
                case 1:
                    skeleton.bones[k].beta = value;
                    break;
                case 2:
                    skeleton.bones[k].gamma = value;
                    break;
                default:
                    break;
            }
        }

        static void set_angle(int idx, glm::vec3 value, Skeleton & skeleton) {
            int k = idx / 3;
            skeleton.bones[k].alpha = value.x;
            skeleton.bones[k].beta = value.y;
            skeleton.bones[k].gamma = value.z;
        }

        static float get_angle(int idx, Skeleton & skeleton) {
            int k = idx / 3;
            int offset = idx % 3;
            switch (offset) {
                case 0:
                    return skeleton.bones[k].alpha;
                case 1:
                    return skeleton.bones[k].beta;
                case 2:
                    return skeleton.bones[k].gamma;
                default:
                    break;
            }
        }

        static void print_skeleton(Skeleton skeleton) {
            std::cout << "skeleton:\n";
            __print_bone("\t", skeleton.bones[0], skeleton);;
        }

        static void __print_bone(std::string indent, Bone bone, Skeleton skeleton) {
            std::cout << indent <<
                "Bone idx : " << bone.index << " " <<
                "Parent idx : " << bone.parent << " " <<
                "Euler angles : (" << bone.alpha << " , " << bone.beta << " ," << bone.gamma << ") " <<
                "Euler code : " << bone.euler_code << " " <<
                "Origin : (" << bone.t_wcs.x << ", " << bone.t_wcs.y << ", " << bone.t_wcs.z << ")\n";
            for (unsigned int idx = 0; idx < bone.children.size(); idx++) {
                __print_bone(indent + "\t", skeleton.bones[bone.children[idx]], skeleton);
            }
        }

        static void __update_bone(
            Bone & bone,
            Skeleton & skeleton
        ) {
            glm::quat q_alpha = bone.get_rotation_alpha();
            glm::quat q_beta = bone.get_rotation_beta();
            glm::quat q_gamma = bone.get_rotation_gamma();
            glm::quat q_bone = prod(q_alpha, prod(q_beta, q_gamma));

            glm::vec3 t_parent = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat q_parent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            if (bone.has_parent()) {
                t_parent = skeleton.bones[bone.parent].t_wcs;
                q_parent = skeleton.bones[bone.parent].q_wcs;
            }
            bone.t_wcs = t_parent + rotate(q_parent, bone.t);
            bone.q_wcs = prod(q_parent, q_bone);

            for (unsigned int idx = 0; idx < bone.children.size(); idx++) {
                __update_bone(skeleton.bones[bone.children[idx]], skeleton);
            }
        }
        */
};
