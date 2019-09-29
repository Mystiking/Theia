#include <iostream>
#include <GL/glut.h>
#include "../Vector3.h"

void drawLine() {

}

void callback() {
    //std::cout << "Test\n";
}

void draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    // Draw a white 1x1 square centered at origin
    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f); // White
        glVertex2f(-0.5f, -0.5f);    // x-and-y coordiantes
        glVertex2f(0.5f, -0.5f);    // x-and-y coordiantes
        glVertex2f(0.5f, 0.5f);    // x-and-y coordiantes
        glVertex2f(-0.5f, 0.5f);    // x-and-y coordiantes
    glEnd();
    glFlush(); // Render now
    glutPostRedisplay();
}

int main(int argc, char** argv){

    Vector3 v(1.0f, 2.0f, 3.0f);
    Vector3 w(3.0f, 2.0f, 1.0f);

    std::cout << "v = " << v << '\n';
    std::cout << "w = " << w << '\n';
    std::cout << "v + w = " << v + w << '\n';
    std::cout << "v - w = " << v - w << '\n';
    std::cout << "v * w = " << v * w << '\n';
    std::cout << "v * 2 = " << v * 2 << '\n';
    std::cout << "2 * v = " << 2 * v << '\n';
    std::cout << "norm(v) = " << v.norm() << '\n';

    glutInit(&argc, argv);
    glutCreateWindow("OpenGL Test");
    glutInitWindowSize(200, 420);
    glutInitWindowPosition(50, 50);
    glutDisplayFunc(draw);
    glutIdleFunc(callback);
    glutMainLoop();

    return 0;
}
