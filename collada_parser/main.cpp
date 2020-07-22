#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <map>
#include <iostream>
#include <algorithm>

#ifndef COLLADALOADERINCLUDE
#define COLLADALOADERINCLUDE
#include "collada_loader.hpp"
#endif

#include "../common/animated_model.hpp"

void print_mat4(glm::mat4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void print_mat3(glm::mat3 m) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void print_vec3(glm::vec3 v) {
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

void print_vec4(glm::vec4 v) {
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

void print_quat(glm::quat q) {
    std::cout << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")\n";
}

int main() {

    std::string collada_file = "../objects/demo_object.dae";

    ColladaLoader collada_loader;

    AnimatedModelData animation_model_data = collada_loader.load_collada_model(collada_file, 3);

    AnimatedModel animated_model(collada_file, "../textures/Blue.png", 0);

    SkeletonData joints = animation_model_data.joints;
    int joint_count = joints.joint_count;
    JointData joint_data = joints.root_joint;

    std::cout << "# joints = " << joint_count << "\n";
    std::cout << "(Root) index = " << joint_data.index << "\n";
    std::cout << "(Root) name = " << joint_data.name_id << "\n";
    std::cout << "(Root) bind_local_transform =\n";
    print_mat4(joint_data.bind_local_transform);
    std::cout << "(Child) index = " << joint_data.children[0].index << "\n";
    std::cout << "(Child) name = " << joint_data.children[0].name_id << "\n";
    std::cout << "(Child) bind_local_transform =\n";
    print_mat4(joint_data.children[0].bind_local_transform);

    std::cout << "# joints = " << animated_model.joint_count << "\n";
    std::cout << "(Root) index = " << animated_model.joint_hierarchy.get_id() << "\n";
    std::cout << "(Root) name = " << animated_model.joint_hierarchy.get_name() << "\n";
    std::cout << "(Root) bind_local_transform =\n";
    print_mat4(animated_model.joint_hierarchy.local_bind_transform);
    std::cout << "(Child) index = " << animated_model.joint_hierarchy.children[0].get_id() << "\n";
    std::cout << "(Child) name = " << animated_model.joint_hierarchy.children[0].get_name()<< "\n";
    std::cout << "(Child) bind_local_transform =\n";
    print_mat4(animated_model.joint_hierarchy.children[0].local_bind_transform);
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";

    AnimationData animation_data = collada_loader.load_animation(collada_file);

    std::cout << "# key frames = " << animation_data.key_frames.size() << "\n";
    std::cout << "# joint transforms (Frame x) = " << animation_data.key_frames[0].joint_transforms.size() << "\n";
    std::cout << "--joint transforms (Frame 1)--\n";
    std::cout << "joint name id = " << animation_data.key_frames[0].joint_transforms[0].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[0].joint_transforms[0].joint_local_transform);
    std::cout << "joint name id = " << animation_data.key_frames[0].joint_transforms[1].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[0].joint_transforms[1].joint_local_transform);

    std::cout << "--joint transforms (Frame 25)--\n";
    std::cout << "joint name id = " << animation_data.key_frames[24].joint_transforms[0].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[24].joint_transforms[0].joint_local_transform);
    std::cout << "joint name id = " << animation_data.key_frames[24].joint_transforms[1].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[24].joint_transforms[1].joint_local_transform);

    std::cout << "--joint transforms (Frame 50)--\n";
    std::cout << "joint name id = " << animation_data.key_frames[49].joint_transforms[0].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[49].joint_transforms[0].joint_local_transform);
    std::cout << "joint name id = " << animation_data.key_frames[49].joint_transforms[1].joint_name_id << "\n";
    print_mat4(animation_data.key_frames[49].joint_transforms[1].joint_local_transform);
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";

    // Head of root joint
    glm::vec3 reference_0 = glm::vec3(0, -5, 0);
    glm::vec4 reference_0_v4 = glm::vec4(reference_0.x, reference_0.y, reference_0.z, 1.0);
    // Head of child joint
    glm::vec3 reference_1 = glm::vec3(0, 0, 0);
    glm::vec4 reference_1_v4 = glm::vec4(reference_1.x, reference_1.y, reference_1.z, 1.0);
    // Tail of child joint
    glm::vec3 reference_2 = glm::vec3(0, 5, 0);
    glm::vec4 reference_2_v4 = glm::vec4(reference_2.x, reference_2.y, reference_2.z, 1.0);

    // All transforms are from the perspective of frame 25
    JointTransformData root_joint = animation_data.key_frames[24].joint_transforms[0];
    JointTransformData child_joint = animation_data.key_frames[24].joint_transforms[1];

    glm::mat4 root_current_pose_transform = root_joint.joint_local_transform;
    glm::mat4 child_current_pose_transform = child_joint.joint_local_transform;

    // Bind Local Transforms
    glm::mat4 root_bind_local_transform = joint_data.bind_local_transform;
    glm::mat4 child_bind_local_transform = joint_data.children[0].bind_local_transform;

    // Inverse bind transforms
    glm::mat4 inv_root_bind_local_transform = glm::inverse(root_bind_local_transform);
    //glm::mat4 inv_root_bind_local_transform = inverse(root_bind_local_transform);
    //inv_root_bind_local_transform[3] = -inv_root_bind_local_transform[3];
    glm::mat4 inv_child_bind_local_transform = glm::inverse(root_bind_local_transform * child_bind_local_transform);
    //glm::mat4 inv_child_bind_local_transform = inverse(root_bind_local_transform * child_bind_local_transform);

    std::cout << "root local bind transform:\n";
    print_mat4(root_bind_local_transform);
    std::cout << "inverse root local bind transform:\n";
    print_mat4(inv_root_bind_local_transform);
    std::cout << "inverse sanity check: inv * pos:\n";
    print_vec4(inv_root_bind_local_transform * reference_0_v4);
    std::cout << "child local bind transform:\n";
    print_mat4(child_bind_local_transform);
    std::cout << "inverse child local bind transform:\n";
    print_mat4(inv_child_bind_local_transform);
    std::cout << "inverse sanity check: inv * pos:\n";
    print_vec4(inv_child_bind_local_transform * reference_1_v4);

    // Reference 0 should stay at (0, -5, 0)
    std::cout << "---Reference 0---\n";
    glm::mat4 current_transform = root_current_pose_transform;
    current_transform = current_transform * inv_root_bind_local_transform;
    std::cout << "Before transform = ";
    print_vec3(reference_0);
    std::cout << "After  transform = ";
    reference_0_v4 = current_transform * reference_0_v4;
    print_vec4(reference_0_v4);
    // Reference 1 should end at (0, 0, -5) (or (0, 0, 5))
    std::cout << "---Reference 1---\n";
    current_transform = root_current_pose_transform;
    current_transform = current_transform * inv_root_bind_local_transform;
    std::cout << "Before transform = ";
    print_vec3(reference_1);
    std::cout << "After  transform = ";
    reference_1_v4 = current_transform * reference_1_v4;
    print_vec4(reference_1_v4);
    // Reference 2 should end at (0, 5, 5) (or (0, 5, -5))
    std::cout << "---Reference 2---\n";
    current_transform = root_current_pose_transform * child_current_pose_transform;
    current_transform = current_transform * inv_child_bind_local_transform;
    std::cout << "Before transform = ";
    print_vec3(reference_2);
    std::cout << "After  transform = ";
    reference_2_v4 = current_transform * reference_2_v4;
    print_vec4(reference_2_v4);

    return 0;
}
