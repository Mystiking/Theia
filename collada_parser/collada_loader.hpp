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
#include <bits/stdc++.h>

#ifndef ANIMATEDMODELDATAINCLUDE
#define ANIMATEDMODELDATAINCLUDE
#include "data_structures/animated_model_data.hpp"
#endif

#ifndef ANIMATIONDATAINCLUDE
#define ANIMATIONDATAINCLUDE
#include "data_structures/animation_data.hpp"
#endif

#ifndef RAPIDXMLINCLUDE
#define RAPIDXMLINCLUDE
#include "rapidxml-1.13/rapidxml.hpp"
#endif

class ColladaLoader {
    public:

        // Extracts mesh information
        static AnimatedModelData load_collada_model(
            std::string collada_file,
            int max_weights
        ) {
            std::ifstream tmp(collada_file);
            std::string str((std::istreambuf_iterator<char>(tmp)),
                             std::istreambuf_iterator<char>());
            char *cstr = new char[str.size() + 1];
            strcpy(cstr, str.c_str());
            rapidxml::xml_document<> doc;
            doc.parse<0>(cstr);

            rapidxml::xml_node<> *root = doc.first_node("COLLADA");

            std::vector<unsigned int> _vertex_indices;
            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uvs;
            std::vector<glm::vec3> _normals;

            load_mesh_data(_vertex_indices, _vertices, _uvs, _normals, root);

            std::vector<glm::ivec3> _joint_ids;
            std::vector<glm::vec3> _skinning_weights;
            std::vector<std::string> _joint_order;

            load_skinning_data(max_weights, _vertex_indices, _joint_ids, _skinning_weights, _joint_order, root);

            SkeletonData joints = load_skeleton_data(root, _joint_order);

            return AnimatedModelData(joints, _vertices, _uvs, _normals, _joint_ids, _skinning_weights);

        }

        static SkeletonData load_skeleton_data(
            rapidxml::xml_node<> *root,
            std::vector<std::string> joint_order
        ) {

            int joint_counter = 0;
            rapidxml::xml_node<> *library_visual_scenes = root->first_node("library_visual_scenes");
            rapidxml::xml_node<> *visual_scene = library_visual_scenes->first_node("visual_scene");
            rapidxml::xml_node<> *armature = visual_scene->first_node("node");

            rapidxml::xml_node<> *root_node = armature->first_node("node");

            JointData root_joint = load_joint_data(root_node, true, joint_order, joint_counter);

            return SkeletonData(joint_counter, root_joint);
        }

        static JointData load_joint_data(
            rapidxml::xml_node<> *joint_node,
            bool is_root,
            std::vector<std::string> bone_order,
            int & joint_counter
        ) {
            JointData joint = extract_joint_data(joint_node, is_root, bone_order, joint_counter);
            rapidxml::xml_node<> *child_joint_node = joint_node->first_node("node");
            while (child_joint_node) {
                joint.add_child(load_joint_data(child_joint_node, false, bone_order,  joint_counter));
                child_joint_node = child_joint_node->next_sibling("node");
            }
            return joint;
        }

        static JointData extract_joint_data(
            rapidxml::xml_node<> *joint_node,
            bool is_root,
            std::vector<std::string> bone_order,
            int & joint_counter
        ) {
            std::string name_id = joint_node->first_attribute("name")->value();

            const char* name = name_id.c_str();
            const char* last_underscore_location = std::strrchr(name, '_');
            if (last_underscore_location)
                name_id = name_id.substr((int)(last_underscore_location - name) + 1);
            int index = std::find(bone_order.begin(), bone_order.end(), name_id) - bone_order.begin();
            glm::mat4 joint_transform = glm::transpose(load_mat4(joint_node->first_node("matrix")));
            if (is_root) {
           // if (false) {
                glm::mat4 correction = glm::mat4_cast(glm::quat(0.7071068, -0.7071068, 0, 0));
                joint_transform = correction * joint_transform;
            }
            joint_counter++;
            return JointData(index, name_id, joint_transform);

        }

        static glm::mat4 load_mat4(
            rapidxml::xml_node<> *matrix_node
        ) {
            glm::mat4 transform;
            char *transform_data = matrix_node->value();
            std::istringstream transform_data_stream(transform_data);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    transform_data_stream >> transform_data;
                    transform[i][j] = std::stof(transform_data);
                }
            }
            return transform;
        }

        static void load_mesh_data(
            std::vector<unsigned int> & vertexIndices,
            std::vector<glm::vec3> & out_vertices,
            std::vector<glm::vec2> & out_uvs,
            std::vector<glm::vec3> & out_normals,
            rapidxml::xml_node<> *root
        ) {
            rapidxml::xml_node<> *library_geometries = root->first_node("library_geometries");
            rapidxml::xml_node<> *geometry = library_geometries->first_node("geometry");
            rapidxml::xml_node<> *mesh = geometry->first_node("mesh");
            rapidxml::xml_node<> *source_positions = mesh->first_node("source");
            rapidxml::xml_node<> *source_normals   = source_positions->next_sibling();
            rapidxml::xml_node<> *source_uvs       = source_normals->next_sibling("source");
            rapidxml::xml_node<> *source_indices   = source_uvs->next_sibling("triangles");

            //std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
            std::vector<unsigned int> uvIndices, normalIndices;
            std::vector<glm::vec3> temp_vertices;
            std::vector<glm::vec2> temp_uvs;
            std::vector<glm::vec3> temp_normals;

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
            indices_stream.clear();

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

        static void load_skinning_data(
            int max_weights,
            std::vector<unsigned int> vertexIndices,
            std::vector<glm::ivec3> & out_joint_ids,
            std::vector<glm::vec3> & out_skinning_weights,
            std::vector<std::string> & out_joint_order,
            rapidxml::xml_node<> *root
        ) {

            std::vector<glm::ivec3> temp_joint_ids;
            std::vector<glm::vec3> temp_skinning_weights;

            // Skinning and joint information
            rapidxml::xml_node<> *library_controllers = root->first_node("library_controllers");
            rapidxml::xml_node<> *controller = library_controllers->first_node("controller");
            rapidxml::xml_node<> *skin = controller->first_node("skin");

            rapidxml::xml_node<> *joints_array = skin->first_node("source")->first_node("Name_array");
            std::map<std::string, int> joint_map;
            std::map<int, std::string> joint_name_map;
            int num_joints = std::stoi(joints_array->first_attribute("count")->value());
            out_joint_order.resize(num_joints);

            char *joints_array_data = joints_array->value();
            std::istringstream joints_array_stream(joints_array_data);
            for (int i = 0; i < num_joints; i++) {
                joints_array_stream >> joints_array_data;
                out_joint_order[i] = std::string(joints_array_data);
            }


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
                for (int i = 0; i < std::min(vc, max_weights); i++) {
                    // Get Joint id
                    skinning_v_stream >> skinning_v_data;
                    int joint_id = std::stoi(skinning_v_data);
                    // Get Weight id
                    skinning_v_stream >> skinning_v_data;
                    int weight_id = std::stoi(skinning_v_data);

                    joint_ids[i] = joint_id;
                    weights[i] = skin_weights[weight_id];
                }
                for (int i = vc; i < max_weights; i++) {
                    joint_ids[i] = -1;
                    weights[i] = 0.0f;
                }

                // Flush remaining joint data
                for (int i = max_weights; i < vc; i++) {
                        skinning_v_stream >> skinning_v_data;
                        skinning_v_stream >> skinning_v_data;
                }
                //std::cout << "join ids: " << joint_ids[0] <<  " " << joint_ids[1] << " " <<  joint_ids[2] << "\n";
                temp_joint_ids.push_back(glm::ivec3(joint_ids[0], joint_ids[1], joint_ids[2]));
                temp_skinning_weights.push_back(glm::vec3(weights[0], weights[1], weights[2]));
            }


            // For each vertex of each triangle
            for( unsigned int i=0; i<vertexIndices.size(); i++ ){

                // Get the indices of its attributes
                unsigned int vertexIndex = vertexIndices[i];


                // Get the attributes thanks to the index
                glm::ivec3 _joint_ids      = temp_joint_ids[ vertexIndex ];
                glm::vec3 _skinning_weight = temp_skinning_weights[ vertexIndex ];

                // Put the attributes in buffers
                out_joint_ids       .push_back(_joint_ids);
                out_skinning_weights.push_back(_skinning_weight);
            }
        }

        static AnimationData load_animation(
            std::string collada_file
        ) {
            std::ifstream tmp(collada_file);
            std::string str((std::istreambuf_iterator<char>(tmp)),
                             std::istreambuf_iterator<char>());
            char *cstr = new char[str.size() + 1];
            strcpy(cstr, str.c_str());
            rapidxml::xml_document<> doc;
            doc.parse<0>(cstr);

            rapidxml::xml_node<> *root = doc.first_node("COLLADA");
            rapidxml::xml_node<> *animation_data = root->first_node("library_animations")->first_node("animation");
            rapidxml::xml_node<> *joint_hierarchy_data = root->first_node("library_visual_scenes");

            std::string root_joint = find_root_node(joint_hierarchy_data);
            std::vector<float> key_frame_times;
            // Assumes that all joints have the same key frame timings
            char *time_data = animation_data->first_node("animation")->first_node("source")->first_node("float_array")->value();
            std::istringstream time_data_stream(time_data);
            while (time_data_stream >> time_data) {
                key_frame_times.push_back(std::stof(time_data));
            }
            float duration = key_frame_times.back();
            std::vector<KeyFrameData> key_frames;
            for (int i = 0; i < (int)key_frame_times.size(); i++) {
                key_frames.push_back(KeyFrameData(key_frame_times[i]));
            }
            rapidxml::xml_node<> *animation_node = animation_data->first_node("animation");
            while(animation_node) {
                load_joint_transforms(key_frames, animation_node, root_joint);
                animation_node = animation_node->next_sibling("animation");
            }
            return AnimationData(duration, key_frames);

        }

        static std::string find_root_node(
            rapidxml::xml_node<> *joint_hierarchy_data
        ) {
            rapidxml::xml_node<> *skeleton = joint_hierarchy_data->first_node("visual_scene")->first_node("node");
            return skeleton->first_node("node")->first_attribute("id")->value();
        }

        static void load_joint_transforms(
            std::vector<KeyFrameData> & key_frames,
            rapidxml::xml_node<> *joint_data,
            std::string root_node_id
        ) {
            std::string joint_name_id = get_joint_name(joint_data);
            // Make name conform to "name" field in the joint nodes
            const char* name = joint_name_id.c_str();
            const char* last_underscore_location = std::strrchr(name, '_');
            if (last_underscore_location)
                joint_name_id = joint_name_id.substr((int)(last_underscore_location - name) + 1);
            std::string data_id = get_data_id(joint_data);

            rapidxml::xml_node<> *data_node = joint_data->first_node("source");
            while (data_node) {
                if (data_node->first_attribute("id")->value() == data_id) {
                    break;
                } else {
                    data_node = data_node->next_sibling("source");
                }
            }

            bool is_root = joint_name_id == root_node_id;
            rapidxml::xml_node<> *transform_node = data_node->first_node("float_array");
            char *transform_data = transform_node->value();
            std::istringstream transform_data_stream(transform_data);
            for (int k = 0; k < (int)key_frames.size(); k++)
            {
                glm::mat4 joint_transform;
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        transform_data_stream >> transform_data;
                        joint_transform[i][j] = std::stof(transform_data);
                    }
                }
                joint_transform = glm::transpose(joint_transform);
                if (is_root) {
                //if (false) {
                    glm::mat4 correction = glm::mat4_cast(glm::quat(0.7071068, -0.7071068, 0, 0));
                    joint_transform = correction * joint_transform;
                }
                key_frames[k].add_joint_transform(JointTransformData(joint_name_id, joint_transform));
            }
        }

        static std::string get_joint_name(
            rapidxml::xml_node<> *joint_data
        ) {
            rapidxml::xml_node<> *channel_node = joint_data->first_node("channel");
            std::string data = channel_node->first_attribute("target")->value();
            return data.substr(0, data.find("/"));
        }

        static std::string get_data_id(
            rapidxml::xml_node<> *joint_data
        ) {
            rapidxml::xml_node<> *sampler_node = joint_data->first_node("sampler")->first_node("input");
            while(sampler_node) {
                if (std::string(sampler_node->first_attribute("semantic")->value()) == "OUTPUT") {
                    break;
                } else {
                    sampler_node = sampler_node->next_sibling("input");
                }
            }
            std::string data = sampler_node->first_attribute("source")->value();
            return data.substr(1);
        }

};
