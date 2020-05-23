#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Mesh.h"


void callback() {
    //std::cout << "Test\n";
}

void drawTriangles() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glBegin(GL_LINES);

    glEnd();

    // Draw a white 1x1 square centered at origin
        glVertex2f(-0.5f, -0.5f);    // x-and-y coordiantes
        glVertex2f(0.5f, -0.5f);    // x-and-y coordiantes
        glVertex2f(0.5f, 0.5f);    // x-and-y coordiantes
        glVertex2f(-0.5f, 0.5f);    // x-and-y coordiantes
    glFlush(); // Render now
    glutPostRedisplay();
    glutSwapBuffers();
}

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


int main(int argc, char** argv){

    GLuint vertexbuffer, vertexshader, fragmentshader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Setting shaders
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertexshader);

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragmentshader);

    program = glCreateProgram();
    glAttachShader(program, vertexshader);
    glAttachShader(program, fragmentshader);
    glLinkProgram(program);

    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    Vector3 v000(0.0f, 0.0f, 0.0f); // 6
    Vector3 v010(0.0f, 1.0f, 0.0f); // 1
    Vector3 v100(1.0f, 0.0f, 0.0f); // 5
    Vector3 v110(1.0f, 1.0f, 0.0f); // 2
    Vector3 v001(0.0f, 0.0f, 1.0f); // 7
    Vector3 v011(0.0f, 1.0f, 1.0f); // 0
    Vector3 v101(1.0f, 0.0f, 1.0f); // 4
    Vector3 v111(1.0f, 1.0f, 1.0f); // 3

    std::vector<Triangle> tris = {
        Triangle(v111, v010, v011), // [3, 1, 0]
        Triangle(v111, v110, v010), // [3, 2, 1]
        Triangle(v101, v110, v111), // [4, 2, 3]
        Triangle(v101, v100, v110), // [4, 5, 2]
        Triangle(v101, v011, v001), // [4, 0, 7]
        Triangle(v101, v111, v011), // [4, 3, 0]
        Triangle(v001, v100, v101), // [7, 5, 4]
        Triangle(v001, v000, v100), // [7, 6, 5]
        Triangle(v011, v000, v001), // [0, 6, 7]
        Triangle(v011, v010, v000), // [0, 1, 6]
        Triangle(v000, v110, v100), // [6, 2, 5]
        Triangle(v000, v010, v110), // [6, 1, 2]
    };
    Mesh m(tris);

    glutInit(&argc, argv);
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glutInitWindowSize(200, 420);
    glutInitWindowPosition(50, 50);
    glutDisplayFunc(drawTriangles);
    glutIdleFunc(callback);
    glutMainLoop();

    return 0;
}
