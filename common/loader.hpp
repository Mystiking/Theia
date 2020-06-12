#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

//#include "animation/animated_model.hpp"
#include "rapidxml-1.13/rapidxml.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>

void load_dae(
    const char *file_name,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
) {
    std::ifstream t(file_name);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::ivec3> temp_joint_ids;
    std::vector<glm::vec3> temp_weights;

    // Load geometry
    rapidxml::xml_node<> *root = doc.first_node("COLLADA");
    rapidxml::xml_node<> *library_geometries = root->first_node("library_geometries");
    rapidxml::xml_node<> *geometry= library_geometries->first_node("geometry");
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

    // Load skinning/joint information
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

    // Get all skinning counts
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
    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex ];
        glm::vec2 uv = temp_uvs[ uvIndex ];
        glm::vec3 normal = temp_normals[ normalIndex ];

        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);
    }
}
