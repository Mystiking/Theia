#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <vector>
#include <iostream>
#include "bone.hpp"

class Skeleton {
    public:
        int max_bones = 50;


        std::vector<glm::mat4> bonetransforms;
        std::vector<Bone> bones;  // Bones of the skeleton

        Skeleton() {
            this->bones.resize(0);
            this->bonetransforms.resize(max_bones);
        }


        bool has_root() {
            return (bones.size() > 0);
        }

        bool has_bone(int idx) {
            return (idx >= 0) && (this->bones.size() > (unsigned int)idx);
        }

        void add_root(
            float alpha,
            float beta,
            float gamma,
            float tx,
            float ty,
            float tz,
            std::string euler_code = "ZYZ"
        ){
            Bone root;
            root.t = glm::vec3(tx, ty, tz);
            root.alpha = alpha;
            root.beta = beta;
            root.gamma = gamma;
            root.euler_code = euler_code;
            root.index = 0;
            this->bones.push_back(root);
        }

        void add_bone(
            int parent_idx,
            float alpha,
            float beta,
            float gamma,
            float tx,
            float ty,
            float tz,
            std::string euler_code = "ZYZ"
        ){
            Bone end_effector;
            end_effector.index = this->bones.size();
            end_effector.parent = parent_idx;
            end_effector.t = glm::vec3(tx, ty, tz);
            end_effector.alpha = alpha;
            end_effector.beta = beta;
            end_effector.gamma = gamma;
            end_effector.euler_code = euler_code;

            this->bones[parent_idx].children.push_back(end_effector.index);
            this->bones.push_back(end_effector);
        }

        glm::quat prod(glm::quat qa, glm::quat qb) {
            float a = qa.w;
            float b = qb.w;

            glm::vec3 A = glm::vec3(qa.x, qa.y, qa.z);
            glm::vec3 B = glm::vec3(qb.x, qb.y, qb.z);

            float qs = a * b - glm::dot(A, B);
            glm::vec3 qv = a * B + A * b + glm::cross(A, B);

            return glm::quat(qs, qv.x, qv.y, qv.z);
        }

        glm::quat conjugate(glm::quat q) {
            return glm::quat(q.w, -q.x, -q.y, -q.z);
        }

        glm::vec3 rotate(glm::quat q, glm::vec3 r) {
            glm::quat qr = glm::quat(0.0, r.x, r.y, r.z);
            glm::quat q_rotated = prod(prod(q, qr), conjugate(q));
            return glm::vec3(q_rotated.x, q_rotated.y, q_rotated.z);
        }


        glm::mat4 get_rotation_matrix(glm::quat q) {
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

        void set_angle(int idx, glm::vec3 value) {
            int k = idx / 3;
            this->bones[k].alpha = value.x;
            this->bones[k].beta = value.y;
            this->bones[k].gamma = value.z;
        }

        float get_angle(int idx) {
            int k = idx / 3;
            int offset = idx % 3;
            switch (offset) {
                case 0:
                    return this->bones[k].alpha;
                case 1:
                    return this->bones[k].beta;
                case 2:
                    return this->bones[k].gamma;
                default:
                    return -100;
            }
        }

        void update() {
            // Begin updating from the root
            __update_bone(0);
            // Populate bonetransforms vector
            create_bone_transforms();
        }

        void __update_bone(
            int boneidx
        ) {
            glm::quat q_alpha = this->bones[boneidx].get_rotation_alpha();
            glm::quat q_beta = this->bones[boneidx].get_rotation_beta();
            glm::quat q_gamma = this->bones[boneidx].get_rotation_gamma();
            glm::quat q_bone = prod(q_alpha, prod(q_beta, q_gamma));

            glm::vec3 t_parent = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat q_parent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            if (this->bones[boneidx].has_parent()) {
                t_parent = this->bones[this->bones[boneidx].parent].t_wcs;
                q_parent = this->bones[this->bones[boneidx].parent].q_wcs;
            }
            this->bones[boneidx].t_wcs = t_parent + rotate(q_parent, this->bones[boneidx].t);
            this->bones[boneidx].q_wcs = prod(q_parent, q_bone);

            for (unsigned int idx = 0; idx < this->bones[boneidx].children.size(); idx++) {
                __update_bone(this->bones[boneidx].children[idx]);
            }
        }

        void print() {
            std::cout << "Skeleton\n";
            __print_bone("", 0);
        }

        void __print_bone(
                std::string indent,
                int boneidx
        ) {
            Bone bone = this->bones[boneidx];
            std::cout << indent <<
                "Bone idx : " << bone.index << " " <<
                "Parent idx : " << bone.parent << " " <<
                "Euler angles : (" << bone.alpha << " , " << bone.beta << " ," << bone.gamma << ") " <<
                "Euler code : " << bone.euler_code << " " <<
                "Origin : (" << bone.t_wcs.x << ", " << bone.t_wcs.y << ", " << bone.t_wcs.z << ")\n";
            for (unsigned int idx = 0; idx < bone.children.size(); idx++) {
                __print_bone(indent + "\t", bone.children[idx]);
            }
        }

        void create_bone_transforms() {
            __create_bone_transform(0);
        }

        void __create_bone_transform(
            int boneidx
        ) {
            Bone bone = this->bones[boneidx];

            glm::vec3 t_parent = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat q_parent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            if (bone.has_parent()) {
                t_parent = this->bones[bone.parent].t_wcs;
                q_parent = this->bones[bone.parent].q_wcs;
            }
            //glm::quat q_bone_transformed = prod(q_parent, q_bone);
            glm::mat4 bone_transform = glm::mat4_cast(q_parent);
            bone_transform[3] = glm::vec4(t_parent.x, t_parent.y, t_parent.z, 1.0);
            this->bonetransforms[boneidx] = bone_transform;

            for (unsigned int idx = 0; idx < bone.children.size(); idx++) {
                __create_bone_transform(bone.children[idx]);
            }
        }

};
