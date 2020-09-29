#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// OpenGL includes
#ifndef GLINCLUDE
#define GLINCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
#include "render.hpp"
#ifndef MODELINCLUDE
#define MODELINCLUDE
#include "model.hpp"
#endif

#ifndef GLMINCLUDE
#define GLMINCLUDE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#endif

#include "factory/shapes.hpp"
#include "factory/sdfs.hpp"
#include "factory/marching_cubes.hpp"

#ifndef IKINCLUDE
#define IKINCLUDE
#include "IK/builder.hpp"
#endif

#include "animations/skinning.hpp"

void print_mat4(glm::mat4 m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void print_vec3(glm::vec3 v) {
        std::cout << v.x << " " << v.y << " " << v.z << "\n";
}

void print_quat(glm::quat q) {
        std::cout << q.x << " " << q.y << " " << q.z << " " << q.w << "\n";
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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
    glClearColor(0.133f, 0.133f, 0.133f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint model_shader = LoadShaders( "shaders/shadows.vs", "shaders/shadows.fs");
    GLuint d_shader = LoadShaders( "shaders/depthshader.vs", "shaders/depthshader.fs");
    GLuint q_shader = LoadShaders( "shaders/quad_depth_debug.vs", "shaders/quad_depth_debug.fs");

    Model cube_0 = cube(8, 8, 8);
    Model cube_1 = cube(8, 8, 8);
    subdivide_cube(&cube_0, 3);
    refine_model(&cube_1, 3);
    unwrap(&cube_0);
    unwrap(&cube_1);
    Model plane_ = cube(200, 1, 200);
    GLuint texture = load_texture("textures/uvtemplate.bmp", 0);
    cube_0.texture = texture;
    cube_1.texture = texture;
    cube_0.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 3.0, 0.0));
    cube_1.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(-5, 3.0, 0.0));
    plane_.model_matrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -4.0, 0.0));


    /* Marching Cubes demo : A box of size (2, 2, 2) at location (0, 3, 0) */
    /*
    glm::vec3 box_centre = glm::vec3(0, 0, 0);
    glm::vec3 box_size = glm::vec3(2, 2, 2);
    float size = 1.0f;
    glm::vec3 starting_point = glm::vec3(-10, -10, -10);
    glm::vec3 end_point = glm::vec3(10, 10, 10);
    std::vector<GridCell> grid;
    for (int x = 0; starting_point.x + size * x < end_point.x; x++) {
        for (int y = 0; starting_point.y + size * y < end_point.y; y++) {
            for (int z = 0; starting_point.z + size * z < end_point.z; z++) {
                GridCell grid_cell;
                grid_cell.p[0] = glm::vec3(starting_point.x           , starting_point.y           , starting_point.z + size * z);
                grid_cell.p[1] = glm::vec3(starting_point.x + size * x, starting_point.y           , starting_point.z + size * z);
                grid_cell.p[2] = glm::vec3(starting_point.x + size * x, starting_point.y           , starting_point.z);
                grid_cell.p[3] = glm::vec3(starting_point.x           , starting_point.y           , starting_point.z);
                grid_cell.p[4] = glm::vec3(starting_point.x           , starting_point.y + size * y, starting_point.z + size * z);
                grid_cell.p[5] = glm::vec3(starting_point.x + size * x, starting_point.y + size * y, starting_point.z + size * z);
                grid_cell.p[6] = glm::vec3(starting_point.x + size * x, starting_point.y + size * y, starting_point.z);
                grid_cell.p[7] = glm::vec3(starting_point.x           , starting_point.y + size * y, starting_point.z);


                grid_cell.v[0] = sphere(grid_cell.p[0], box_centre, 2.0f);
                grid_cell.v[1] = sphere(grid_cell.p[1], box_centre, 2.0f);
                grid_cell.v[2] = sphere(grid_cell.p[2], box_centre, 2.0f);
                grid_cell.v[3] = sphere(grid_cell.p[3], box_centre, 2.0f);
                grid_cell.v[4] = sphere(grid_cell.p[4], box_centre, 2.0f);
                grid_cell.v[5] = sphere(grid_cell.p[5], box_centre, 2.0f);
                grid_cell.v[6] = sphere(grid_cell.p[6], box_centre, 2.0f);
                grid_cell.v[7] = sphere(grid_cell.p[7], box_centre, 2.0f);

                grid.push_back(grid_cell);
            }
        }
    }

    Model sdf_generated_cube;
    for (GridCell gc : grid) {
        add_grid_cell_triangles(gc, 1.0f, sdf_generated_cube.vertices, sdf_generated_cube.uvs, sdf_generated_cube.indices);
    }
    */


    //Render render(window, {&cube_0, &cube_1, &plane_}, d_shader, q_shader, model_shader, SCREENWIDTH, SCREENHEIGHT);
    //std::cout << "!!!\n";
    Render render(window, {&cube_1, &cube_0, &plane_}, d_shader, q_shader, model_shader, SCREENWIDTH, SCREENHEIGHT);
    //Render render(window, {&sdf_generated_cube}, d_shader, q_shader, model_shader, SCREENWIDTH, SCREENHEIGHT);
    //IK Stuff
    Skeleton skeleton;
    Bone b0 = Builder::create_root(skeleton,
                                   glm::radians(90.0f),
                                   0.0f,
                                   0.0f,
                                   1.0f,
                                   0.0f,
                                   0.0f);
    Bone b1 = Builder::add_bone(skeleton, b0.index, glm::radians(90.0f), 0.0, 0.0, 1.0, 0.0, 0.0);
    Bone b2 = Builder::add_bone(skeleton, b1.index, glm::radians(-90.0f), 0.0, 0.0, 1.0, 0.0, 0.0);


    // Print using transformation matrix instead
    // Bone b0
    b0.t_wcs = b0.t;
    std::cout << "Bone index : " << b0.index << " " <<
                 "Origin : (" << b0.t_wcs.x << ", " << b0.t_wcs.y << ", " << b0.t_wcs.z << ")\n";
    glm::quat b0_q_alpha = b0.get_rotation_alpha();
    glm::quat b0_q_beta = b0.get_rotation_beta();
    glm::quat b0_q_gamma = b0.get_rotation_gamma();
    glm::quat b0_q_b0 = Builder::prod(b0_q_alpha, Builder::prod(b0_q_beta, b0_q_gamma));

    //b1.t_wcs = b0.t_wcs + glm::mat3(Builder::get_rotation_matrix(b0_q_b0)) * b1.t;
    //b1.t_wcs = b0.t_wcs + glm::vec3((Builder::get_rotation_matrix(b0_q_b0)) * glm::vec4(b1.t, 1.0));
    b1.t_wcs = b0.t_wcs + glm::vec3((glm::mat4_cast(b0_q_b0)) * glm::vec4(b1.t, 1.0));
    std::cout << "Bone index : " << b1.index << " " <<
                 "Origin : (" << b1.t_wcs.x << ", " << b1.t_wcs.y << ", " << b1.t_wcs.z << ")\n";

    glm::quat b1_q_alpha = b1.get_rotation_alpha();
    glm::quat b1_q_beta = b1.get_rotation_beta();
    glm::quat b1_q_gamma = b1.get_rotation_gamma();
    glm::quat b1_q_b1 = Builder::prod(b1_q_alpha, Builder::prod(b1_q_beta, b1_q_gamma));
    b1_q_b1 = b0_q_b0 * b1_q_b1;

    b2.t_wcs = b1.t_wcs + glm::vec3((glm::mat4_cast(b1_q_b1)) * glm::vec4(b2.t, 1.0));
    std::cout << "Bone index : " << b2.index << " " <<
                 "Origin : (" << b2.t_wcs.x << ", " << b2.t_wcs.y << ", " << b2.t_wcs.z << ")\n";


    Builder::update_skeleton(skeleton);
    Builder::print_skeleton(skeleton);
    std::cout << "\n\n";


    /*
    Builder::set_angle(b1.index * 3 + 1, glm::radians(45.0f), skeleton);
    Builder::print_skeleton(skeleton);
    std::cout << "\n\n";
    Builder::update_skeleton(skeleton);
    Builder::print_skeleton(skeleton);
    std::cout << "\n\n";
    */
    skin_mesh(cube_0, skeleton);

    //exit(0);



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
        /*
         * Game Logic
         */
        // Step 1: Move camera
        first_person_camera(window);
        // Step 2: Update animations
        // TODO: Add animations

        // Step 3: Render scene
        render.renderScene();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

