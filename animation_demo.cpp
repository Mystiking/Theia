#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#ifndef GLINCLUDE
#define GLINCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
// Common includes
//#include "common/texture.hpp"
//#include "common/load_texture.h"
//#include "common/shaders.h"
//#include "common/controls.h"
//#include "common/light.h"
//#include "common/vboindexer.hpp"
//#include "common/objloader.hpp"
//#include "common/tangentspace.hpp"

//include "common/light.h"
//#include "common/camera.h"
#include "common/animation.hpp"
#include "common/render.hpp"
#include "common/controls.h"
#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

void print_mat4(glm::mat4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void print_jt(JointTransform jt) {
    print_mat4(glm::translate(glm::mat4_cast(jt.rotation), jt.position));
}

void print_vec3(glm::vec3 v) {
        std::cout << v.x << " " << v.y << " " << v.z << "\n";
}

void print_quat(glm::quat q) {
        std::cout << q.x << " " << q.y << " " << q.z << " " << q.w << "\n";
}

int main() {
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise Game Window\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x Anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want old opengl

    // Open a window and create its OpenGL context
    GLFWwindow *window; // Global for simplicity
    window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Game Window", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to initialise Game Window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetScrollCallback(window, scroll_callback);

    bool first_person = true;
    if (first_person) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // Reset cursor position for the next frame
        glfwSetCursorPos(window, SCREENXCENTER, SCREENYCENTER);
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Make sure we always draw the nearest triangles first
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwMakeContextCurrent(window); // Initialise GLEW
    //glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint model_shader = LoadShaders( "shaders/demo.vs", "shaders/demo.fs");
    GLuint animated_model_shader = LoadShaders( "shaders/animation_demo.vs", "shaders/animation_demo.fs");

    std::vector<Model*> models;
    std::vector<AnimatedModel*> animated_models;

    //AnimatedModel *demo_object = new AnimatedModel("objects/demo_object.dae", "textures/Green.png", animated_model_shader);
    AnimatedModel *demo_object = new AnimatedModel("objects/torso_waving.dae", "textures/Green.png", animated_model_shader);
    //Animation *animation_0 = new Animation("objects/demo_object.dae");
    Animation *animation_0 = new Animation("objects/torso_waving.dae");
    Animator animator(animation_0, demo_object);

    std::cout << "indices size buffer: " << demo_object->indices.size() << "\n";
    std::cout << "vertices size buffer: " << demo_object->vertices.size() << "\n";
    std::cout << "normals size buffer: " << demo_object->normals.size() << "\n";
    std::cout << "uvs size buffer: " << demo_object->uvs.size() << "\n";
    std::cout << "joint ids buffer: " << demo_object->joint_ids.size() << "\n";
    std::cout << "skinning weights buffer: " << demo_object->skinning_weights.size() << "\n";
    animated_models.push_back(demo_object);

    /*
    std::cout << "# joints = " << demo_object->joint_count << "\n";
    std::cout << "(Root) index = " << demo_object->joint_hierarchy.get_id() << "\n";
    std::cout << "(Root) name = " << demo_object->joint_hierarchy.get_name() << "\n";
    std::cout << "(Root) bind_local_transform =\n";
    print_mat4(demo_object->joint_hierarchy.local_bind_transform);
    std::cout << "(Root) inverse transform =\n";
    print_mat4(demo_object->joint_hierarchy.inverse_bind_transform);
    std::cout << "(Child) index = " << demo_object->joint_hierarchy.children[0].get_id() << "\n";
    std::cout << "(Child) name = " << demo_object->joint_hierarchy.children[0].get_name()<< "\n";
    std::cout << "(Child) bind_local_transform =\n";
    print_mat4(demo_object->joint_hierarchy.children[0].local_bind_transform);
    std::cout << "(Child) inverse transform =\n";
    print_mat4(demo_object->joint_hierarchy.children[0].inverse_bind_transform);
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";
*/
    //Model *demo_object_static = new Model("objects/demo_object.dae", "textures/Green.png", model_shader);
    //models.push_back(demo_object_static);
    /*
    //Model player_model("objects/demo_object_centered.obj", "textures/Red.png", model_shader);
    //Model beam("objects/demo_object_centered.obj", "textures/Green.png", animated_model_shader, glm::mat4(1.0f));
    //Model beam2("objects/demo_object_centered.obj", "textures/Red.png", animated_model_shader, glm::mat4(1.0f));
    Model beam("objects/stick_arm.dae", "textures/Green.png", animated_model_shader, glm::mat4(1.0f));
    //Model beam2("objects/demo_object_centered.dae", "textures/Red.png", animated_model_shader, glm::mat4(1.0f));

    JointTransform k_0_1(glm::vec3(0.0, -5, 0.0), glm::quat(0.0, 0.0, 0.0, 0.0));
    JointTransform k_0_2(glm::vec3(0.0, 5, 0.0), glm::quat(0.0, 0, 0, 0.0));
    glm::mat4 t_0_1 = k_0_1.get_local_transform();
    glm::mat4 t_0_2 = t_0_1 * k_0_2.get_local_transform();


    std::vector<Animation> animations;
    load_animation_data("objects/stick_arm.dae", animations, beam.joint_hierarchy);

    //Animation animation0({k0, k1, k2}, 2.0f);
    Animation animation0 = animations[0];
    //animation0.frames = {animation0.frames[0]};

    glm::vec3 pos_a_f_0 = animations[0].frames[0].joint_transforms["Middle"].position;
    glm::quat rot_a_f_0 = animations[0].frames[0].joint_transforms["Middle"].rotation;

    std::cout << "Frame 0 rotation: ";
    pq(rot_a_f_0);
    std::cout << ", position: ";
    pv3(pos_a_f_0);

    glm::mat4 center0(1.0);
    center0 = glm::translate(center0, glm::vec3(0, -2.5, 0));
    glm::mat4 center1(1.0);
    center1 = glm::translate(center1, glm::vec3(0, 0.0, 0));
    glm::mat4 center2(1.0);
    center2 = glm::translate(center2, glm::vec3(0, 2.5, 0));

    // Keyframe 1
    Model joint_0_0_model("objects/ball.obj", "textures/Gray.png", model_shader, center0);
    Model joint_1_0_model("objects/ball.obj", "textures/Red.png", model_shader, t_0_1 * center1);
    Model joint_2_0_model("objects/ball.obj", "textures/Green.png", model_shader, t_0_2 * center2);

    // Create skinning weights
    std::vector<glm::ivec3> joint_ids;
    std::vector<glm::vec3> skinning_weights;
    for (int i = 0; i < (int)beam.vertices.size(); i++) {
        if (beam.vertices[i].y > 0) {
            joint_ids.push_back(glm::ivec3(0, 1, -1));
            skinning_weights.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            joint_ids.push_back(glm::ivec3(0, 1, -1));
            skinning_weights.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }

    //beam.joint_ids = joint_ids;
    //beam.skinning_weights = skinning_weights;
    beam.is_animated = true;
    //beam.local_joint_transforms = { j_0_1.get_animated_transform(), j_0_1.children[0].get_animated_transform()};
    //beam.joint_count = 2;

    //beam2.joint_ids = joint_ids;
    //beam2.skinning_weights = skinning_weights;
    //beam2.is_animated = true;
    //beam2.local_joint_transforms = { j_1_1.local_bind_transform, j_1_1.children[0].local_bind_transform};
    //beam2.joint_count = 2;

    Animator animator0(&animation0, &beam);
    // Visualize the joints
    models.push_back(&joint_0_0_model);
    models.push_back(&joint_1_0_model);
    models.push_back(&joint_2_0_model);
    //models.push_back(&mesh_vertex);

    //models.push_back(&joint_0_1_model);
    //models.push_back(&joint_1_1_model);
    models.push_back(&beam);
    //models.push_back(&beam2);
    */
    Render render(window, models, animated_models, "shaders/passthrough.vs", "shaders/passthrough.fs", SCREENWIDTH, SCREENHEIGHT);

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do{
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f fps\n", double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        // Game Logic
        if (first_person) {
            first_person_camera(window);
        } else {
            //handle_user_input(window, &player, enemies);
        }
        animator.update(delta_time);
        // Draw scene
        render.draw(projection, view, LightPosition);
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

    render.clean_up();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}
