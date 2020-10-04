#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "../model.hpp"
#endif
#include <vector>
#include <utility>
#include <map>

#ifndef IKINCLUDE
#define IKINCLUDE
#include "../IK/builder.hpp"
#endif

/*
 * Simple implementation using a single bone for each vertex.
 */
void skin_mesh(
    Model & model,
    Skeleton skeleton
){
    // For each bone, compute its start and end point
    std::map<int, std::pair<glm::vec3, glm::vec3>> bone_end_points;
    for (Bone bone : skeleton.bones) {
        if (bone.is_end_effector())
            continue;
        bone_end_points[bone.index] = { bone.t_wcs, skeleton.bones[bone.children[0]].t_wcs};
        std::cout << "Bone " << bone.index << ", from (" <<
            bone_end_points[bone.index].first.x << ", " <<
            bone_end_points[bone.index].first.y << ", " <<
            bone_end_points[bone.index].first.z << ") to (" <<
            bone_end_points[bone.index].second.x << ", " <<
            bone_end_points[bone.index].second.y << ", " <<
            bone_end_points[bone.index].second.z << ")\n";
    }

    model.bone_ids.resize(model.vertices.size());
    model.bone_weights.resize(model.vertices.size());
    for (unsigned int i = 0; i < model.vertices.size(); i++) {
        glm::vec3 v = model.vertices[i];
        // Find closest bone
        int closest_bone;
        float closest_dist = -1.0f;
        for (Bone bone : skeleton.bones) {
            if (bone.is_end_effector())
                continue;
            if (closest_dist < 0.0f) {
                closest_dist = glm::distance(bone_end_points[bone.index].first, v);
                closest_bone = bone.index;
            } else {
                if (glm::distance(bone_end_points[bone.index].first, v) < closest_dist) {
                    closest_dist = glm::distance(bone_end_points[bone.index].first, v);
                    closest_bone = bone.index;
                }
            }
        }
        model.bone_ids[i] = glm::ivec3(closest_bone, -1, -1);
        model.bone_weights[i] = glm::vec3(1.0f, 0.0f, 0.0f);
    }
}
