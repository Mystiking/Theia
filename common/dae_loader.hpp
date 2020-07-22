#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif
#include "rapidxml-1.13/rapidxml.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <map>
#include <iostream>
#ifndef JOINTTRANSFORMINCLUDE
#define JOINTTRANSFORMINCLUDE
#include "animation/joint_transform.hpp"
#endif
#ifndef JOINTINCLUDE
#define JOINTINCLUDE
#include "animation/joint.hpp"
#endif
#ifndef KEYFRAMEINCLUDE
#define KEYFRAMEINCLUDE
#include "animation/key_frame.hpp"
#endif
#ifndef ANIMATIONINCLUDE
#define ANIMATIONINCLUDE
#include "animation/animation.hpp"
#endif

void load_dae() {

}

void load_mesh_data(
    const char * file_name,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
) {
    std::ifstream tmp(file_name);
    std::string str((std::istreambuf_iterator<char>(tmp)),
                     std::istreambuf_iterator<char>());
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    rapidxml::xml_node<> *root = doc.first_node("COLLADA");
    rapidxml::xml_node<> *library_geometries = root->first_node("library_geometries");
    rapidxml::xml_node<> *geometry = library_geometries->first_node("geometry");
    rapidxml::xml_node<> *mesh = geometry->first_node("mesh");
    rapidxml::xml_node<> *source_positions = mesh->first_node("source");
    rapidxml::xml_node<> *source_normals   = source_positions->next_sibling();
    rapidxml::xml_node<> *source_uvs       = source_normals->next_sibling("source");
    rapidxml::xml_node<> *source_indices   = source_uvs->next_sibling("triangles");

    // Vertex positions
    rapidxml::xml_node<> *vertex_positions = source_positions->first_node("float_array");
    int num_vertices = std::stoi(vertex_positions->first_attribute("count")->value());
    char *position_data = vertex_positions->value();
    std::istringstream position_stream(position_data);
    for (int i = 0; i < num_vertices / 3; i++) {
        float* v = new float[3];
        for (int j = 0; j < 3; j++) {
            position_stream >> position_data;
            v[j] = std::stof(position_data);
        }
        temp_vertices.push_back(glm::vec3(v[0], v[1], v[2]));
    }

    // Vertex normals
    rapidxml::xml_node<> *vertex_normals = source_normals->first_node("float_array");
    int num_normals = std::stoi(vertex_normals->first_attribute("count")->value());
    char *normal_data = vertex_normals->value();
    std::istringstream normal_stream(normal_data);
    for (int i = 0; i < num_normals / 3; i++) {
        float* n = new float[3];
        for (int j = 0; j < 3; j++) {
            normal_stream >> normal_data;
            n[j] = std::stof(normal_data);
        }
        temp_normals.push_back(glm::vec3(n[0], n[1], n[2]));
    }

    // UV coordinates
    rapidxml::xml_node<> *vertex_uvs = source_uvs->first_node("float_array");
    int num_uvs = std::stoi(vertex_uvs->first_attribute("count")->value());
    char *uv_data = vertex_uvs->value();
    std::istringstream uv_stream(uv_data);
    for (int i = 0; i < num_uvs / 2; i++) {
        float* uv = new float[2];
        for (int j = 0; j < 2; j++) {
            uv_stream >> uv_data;
            uv[j] = std::stof(uv_data);
        }
        temp_uvs.push_back(glm::vec2(uv[0], uv[1]));
    }

    // Indices
    rapidxml::xml_node<> *indices = source_indices->first_node("p");
    int num_triangles = std::stoi(source_indices->first_attribute("count")->value());
    char *indices_data = indices->value();
    std::istringstream indices_stream(indices_data);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = 0; j < 3; j++) {
            int* indices = new int[3];
            for (int k = 0; k < 3; k++) {
                indices_stream >> indices_data;
                indices[k] = std::stoi(indices_data);
            }
            vertexIndices.push_back(indices[0]);
            normalIndices.push_back(indices[1]);
            uvIndices.push_back(indices[2]);
        }
    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex     = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex          = temp_vertices[ vertexIndex ];
        glm::vec2 uv              = temp_uvs[ uvIndex ];
        glm::vec3 normal          = temp_normals[ normalIndex ];

        // Put the attributes in buffers
        out_vertices        .push_back(vertex);
        out_uvs             .push_back(uv);
        out_normals         .push_back(normal);
    }
}

void load_skinning_data(
    const char * file_name,
    std::vector<glm::ivec3> & out_joint_ids,
    std::vector<glm::vec3> & out_skinning_weights
){
    std::ifstream tmp(file_name);
    std::string str((std::istreambuf_iterator<char>(tmp)),
                     std::istreambuf_iterator<char>());
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr);

    std::vector<unsigned int> vertexIndices;
    std::vector<glm::ivec3> temp_joint_ids;
    std::vector<glm::vec3> temp_skinning_weights;

    rapidxml::xml_node<> *root = doc.first_node("COLLADA");
    rapidxml::xml_node<> *library_geometries = root->first_node("library_geometries");
    rapidxml::xml_node<> *geometry = library_geometries->first_node("geometry");
    rapidxml::xml_node<> *mesh = geometry->first_node("mesh");
    rapidxml::xml_node<> *source_indices = mesh->first_node("triangles");

    // Indices
    rapidxml::xml_node<> *indices = source_indices->first_node("p");
    int num_triangles = std::stoi(source_indices->first_attribute("count")->value());
    char *indices_data = indices->value();
    std::istringstream indices_stream(indices_data);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = 0; j < 3; j++) {
            int* indices = new int[3];
            for (int k = 0; k < 3; k++) {
                indices_stream >> indices_data;
                indices[k] = std::stoi(indices_data);
            }
            vertexIndices.push_back(indices[0]);
        }
    }

    // Skinning and joint information
    rapidxml::xml_node<> *library_controllers = root->first_node("library_controllers");
    rapidxml::xml_node<> *controller = library_controllers->first_node("controller");
    rapidxml::xml_node<> *skin = controller->first_node("skin");
    rapidxml::xml_node<> *skinning_weights = skin->first_node("source")->next_sibling("source")->next_sibling("source")->first_node("float_array");

    rapidxml::xml_node<> *vertex_weight_source = skin->first_node("vertex_weights");
    rapidxml::xml_node<> *vcount = vertex_weight_source->first_node("vcount");
    rapidxml::xml_node<> *v = vertex_weight_source->first_node("v");

    std::vector<float> skin_weights;
    std::vector<int> skin_vcount;
    std::vector<int> skin_v;

    int num_skinning_weights = std::stoi(skinning_weights->first_attribute("count")->value());
    int num_skinning_vertices = std::stoi(vertex_weight_source->first_attribute("count")->value());

    // Get all skinning weights
    char *skinning_weights_data = skinning_weights->value();
    std::istringstream skinning_weights_stream(skinning_weights_data);
    for (int i = 0; i < num_skinning_weights; i++) {
        skinning_weights_stream >> skinning_weights_data;
        skin_weights.push_back(std::stof(skinning_weights_data));
    }

    // Get all skinning counts
    char *skinning_vcount_data = vcount->value();
    std::istringstream skinning_vcount_stream(skinning_vcount_data);
    for (int i = 0; i < num_skinning_vertices; i++) {
        skinning_vcount_stream >> skinning_vcount_data;
        skin_vcount.push_back(std::stoi(skinning_vcount_data));
    }

    // Get all skinning weight information
    char *skinning_v_data = v->value();
    std::istringstream skinning_v_stream(skinning_v_data);
    for (int vc : skin_vcount)  {
        int *joint_ids = new int[3];
        float *weights = new float[3];
        for (int i = 0; i < std::min(vc, 3); i++) {
            // Get Joint id
            skinning_v_stream >> skinning_v_data;
            int joint_id = std::stoi(skinning_v_data);
            // Get Weight id
            skinning_v_stream >> skinning_v_data;
            int weight_id = std::stoi(skinning_v_data);

            joint_ids[i] = joint_id;
            weights[i] = skin_weights[weight_id];
        }
        for (int i = vc; i < 3; i++) {
            joint_ids[i] = -1;
            weights[i] = 0.0f;
        }

        // Flush remaining joint data (we save at most 3)
        for (int i = 3; i < vc; i++) {
                skinning_v_stream >> skinning_v_data;
                skinning_v_stream >> skinning_v_data;
        }
        temp_joint_ids.push_back(glm::ivec3(joint_ids[0], joint_ids[1], joint_ids[2]));
        temp_skinning_weights.push_back(glm::vec3(weights[0], weights[1], weights[2]));
    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];

        // Get the attributes thanks to the index
        glm::ivec3 joint_ids      = temp_joint_ids[ vertexIndex ];
        glm::vec3 skinning_weight = temp_skinning_weights[ vertexIndex ];

        // Put the attributes in buffers
        out_joint_ids       .push_back(joint_ids);
        out_skinning_weights.push_back(skinning_weight);
    }

}

void pm4(glm::mat4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void pv3(glm::vec3 v)   {
    std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}

void pv4(glm::vec4 v)   {
    std::cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << "\n";
}

void pq(glm::quat q)   {
    std::cout << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << "\n";
}

Joint create_joint_hierarchy(
    rapidxml::xml_node<> * node,
    std::map<std::string, int> joint_map,
    glm::mat4 parent_transform
) {
    std::string name = node->first_attribute("name")->value();
    int id = joint_map[name];
    glm::mat4 transform;

    // Load in transform
    char *transform_data = node->first_node("matrix")->value();
    std::istringstream transform_data_stream(transform_data);
    for (int i = 0; i < 4; i++) {
        glm::vec4 row;
        for (int j = 0; j < 4; j++) {
            transform_data_stream >> transform_data;
            row[j] = std::stof(transform_data);
        }
        transform[i] = row;//glm::vec4(row[0], -row[2], row[1], row[3]);
    }
    transform = glm::transpose(transform);
    transform[3] = glm::vec4(transform[3][0], transform[3][2], -transform[3][1], transform[3][3]);

    glm::mat4 bind_transform = parent_transform * transform;
    std::cout << "Bind transform:\n";
    pm4(bind_transform);
    std::cout << "transform:\n";
    pm4(transform);
    std::cout << "parent transform:\n";
    pm4(parent_transform);

    Joint joint(id, name, bind_transform);

    //glm::vec4 joint_bind_position = bind_transform[3] - parent_transform[3];
    glm::vec3 scaled_position = glm::mat3(parent_transform) * glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
    glm::vec4 joint_bind_position = parent_transform[3] + glm::vec4(scaled_position[0], scaled_position[1], scaled_position[2], 1.0);//glm::vec4(scaled_position[0], scaled_position[1], scaled_position[2], 1.0) + parent_transform[3];
    glm::quat joint_bind_rotation = glm::quat_cast(glm::mat3(transform));
    joint.local_transform = JointTransform(glm::vec3(joint_bind_position[0], joint_bind_position[1], joint_bind_position[2]), joint_bind_rotation);
    joint.joint_bind_position = joint_bind_position;
    joint.joint_bind_rotation = joint_bind_rotation;

    rapidxml::xml_node<>* child = node->first_node("node");
    while (child) {
        joint.add_child(create_joint_hierarchy(child, joint_map, bind_transform));
        child = child->next_sibling("node");
    }

    return joint;
}


void load_skeleton_data(
    const char * file_name,
    int & num_joints,
    Joint & root_joint
) {
    std::ifstream t(file_name);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr);

    rapidxml::xml_node<> *root = doc.first_node("COLLADA");
    rapidxml::xml_node<> *library_controllers = root->first_node("library_controllers");
    rapidxml::xml_node<> *controller = library_controllers->first_node("controller");
    rapidxml::xml_node<> *skin = controller->first_node("skin");
    rapidxml::xml_node<> *joints_array = skin->first_node("source")->first_node("Name_array");
    std::map<std::string, int> joint_map;
    std::map<int, std::string> joint_name_map;
    num_joints = std::stoi(joints_array->first_attribute("count")->value());

    char *joints_array_data = joints_array->value();
    std::istringstream joints_array_stream(joints_array_data);
    for (int i = 0; i < num_joints; i++) {
        joints_array_stream >> joints_array_data;
        // Get next joint name
        joint_map.insert({std::string(joints_array_data), i});
        joint_name_map.insert({i, std::string(joints_array_data)});
    }

    rapidxml::xml_node<> *library_visual_scenes = root->first_node("library_visual_scenes");
    rapidxml::xml_node<> *visual_scene = library_visual_scenes->first_node("visual_scene");
    rapidxml::xml_node<> *armature = visual_scene->first_node("node");

    std::string name = armature->first_attribute("name")->value();
    glm::mat4 transform;
    char *transform_data = armature->first_node("matrix")->value();
    std::istringstream transform_data_stream(transform_data);
    for (int i = 0; i < 4; i++) {
        glm::vec4 row;
        for (int j = 0; j < 4; j++) {
            transform_data_stream >> transform_data;
            row[j] = std::stof(transform_data);
        }
        transform[i] = row;//glm::vec4(row[0], -row[2], row[1], row[3]);
    }
    transform = glm::transpose(transform);
    transform[3] = glm::vec4(transform[3][0], transform[3][2], -transform[3][1], transform[3][3]);
    //transform[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);

    rapidxml::xml_node<> *root_joint_node = armature->first_node("node");
    root_joint = create_joint_hierarchy(root_joint_node, joint_map, transform);
}

// Only works for joints with one layer of kids
Joint find_joint(Joint j, std::string name) {
    if (j.get_name() == name) {
        return j;
    }

    for (int i = 0; i < (int)j.children.size(); i++) {
        if (j.children[i].get_name() == name) {
            return j.children[i];
        }
    }
}

void load_animation_data(
    const char * file_name,
    std::vector<Animation> & animations,
    Joint root_joint
){
    std::ifstream t(file_name);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr);


    rapidxml::xml_node<> *root = doc.first_node("COLLADA");

    rapidxml::xml_node<> *library_controllers = root->first_node("library_controllers");
    rapidxml::xml_node<> *controller = library_controllers->first_node("controller");
    rapidxml::xml_node<> *skin = controller->first_node("skin");
    rapidxml::xml_node<> *joints_array = skin->first_node("source")->first_node("Name_array");
    std::map<std::string, int> joint_map;
    std::map<int, std::string> joint_name_map;
    int num_joints = std::stoi(joints_array->first_attribute("count")->value());

    char *joints_array_data = joints_array->value();
    std::istringstream joints_array_stream(joints_array_data);
    for (int i = 0; i < num_joints; i++) {
        joints_array_stream >> joints_array_data;
        // Get next joint name
        joint_map.insert({std::string(joints_array_data), i});
        joint_name_map.insert({i, std::string(joints_array_data)});
    }

    rapidxml::xml_node<> *library_animations = root->first_node("library_animations");
    rapidxml::xml_node<> *armature_animation = library_animations->first_node("animation");
    rapidxml::xml_node<> *animation_node = armature_animation->first_node("animation");

    std::vector<std::vector<float>> time_stamps;
    std::vector<std::vector<JointTransform>> joint_transforms;
    int animation_counter = 0;
    while(animation_node) {
        std::vector<float> tmp_time_stamps;
        std::vector<JointTransform> tmp_joint_transforms;
        int num_keyframes = std::stoi( animation_node->first_node("source")->first_node("float_array")->first_attribute("count")->value() );
        char *key_frame_time_stamp_data = animation_node->first_node("source")->first_node("float_array")->value();
        char *key_frame_transform_data = animation_node->first_node("source")->next_sibling("source")->first_node("float_array")->value();

        Joint current_joint = find_joint(root_joint, joint_name_map[animation_counter]);
        std::cout << current_joint.get_name() << "\n";

        std::istringstream key_frame_time_stamp_stream(key_frame_time_stamp_data);
        std::istringstream key_frame_transform_stream(key_frame_transform_data);
        for (int i = 0; i < num_keyframes; i++) {
            key_frame_time_stamp_stream >> key_frame_time_stamp_data;
            float time_stamp = std::stof(key_frame_time_stamp_data);
            tmp_time_stamps.push_back(time_stamp);
            glm::mat4 transform;

            for (int j = 0; j < 4; j++) {
                glm::vec4 row;
                for (int k = 0; k < 4; k++) {
                    key_frame_transform_stream >> key_frame_transform_data;
                    row[k] = std::stof(key_frame_transform_data);
                }
                transform[j] = glm::vec4(row[0], -row[2], row[1], row[3]);
            }
            JointTransform joint_transform(current_joint.joint_bind_position + glm::vec3(transform[3]), glm::quat_cast(transform));
            tmp_joint_transforms.push_back(joint_transform);
        }
        time_stamps.push_back(tmp_time_stamps);
        joint_transforms.push_back(tmp_joint_transforms);
        // Load next animation
        animation_node = animation_node->next_sibling("animation");
        animation_counter++;
    }

    // Translate joint transforms into a key frame
    std::vector<KeyFrame> key_frames;
    for (int i = 0; i < (int)time_stamps[0].size(); i++) {
        std::map<std::string, JointTransform> joint_transform_map;
        for (int j = 0; j < num_joints; j++) {
            if (i == 20) {

                std::cout << joint_name_map[j] << ":\n";
                pv3(joint_transforms[j][i].position);
                pq(joint_transforms[j][i].rotation);
            }
            joint_transform_map.insert({joint_name_map[j], joint_transforms[j][i]});
        }
        key_frames.push_back(KeyFrame(time_stamps[0][i], joint_transform_map));
    }
    animations.push_back(Animation(key_frames, time_stamps[0].back()));
}
