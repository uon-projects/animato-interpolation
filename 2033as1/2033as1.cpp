// glutwindow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "glut.h"

using namespace std;

// METHOD_INITIALISATION_AREA_START
void display();

void reshape(int w, int h);

void idle();

void mouseClick(int mouseButton, int state, int x, int y);

void mouseMotion(int x, int y);

void mouseMovement(int x, int y);

void writeVertices();

void readVertices();

void initOpenGL();
// METHOD_INITIALISATION_AREA_END

// GLOBAL_VARIABLES_INITIALISATION_AREA_START
// the coordinates of the pentagon d(x, y, z)
float dx = 0.0;
float dy = 0.0;
float dz = 0.0;

// 5 vertices, and x, y coordinates
float vertices[5][2];

// the current scale of the pentagon
float pentagonScale = 1.0;

// the the rotation of the pentagon
// the angle that should be applied to our pentagon
float pentagonAngle = 0.0;

// pi - since the PI number is 3.141592653589793238462643383...
// we are going to use only the first 6 decimals
float pi = 3.141592;

// idle pentagonAngle
float idleAngle = 0.02;

// show data in the debug console
bool debugEnabled = false;

// set to true if the mouse is not clicked
bool idleState = true;

// the coordinate of the mouse when the button was clicked
int clickMouseX, clickMouseY;

// the coordinate of the mouse
int mouseX, mouseY;

// the button that was interacted with
int mouseButton;

// listname
GLuint listname = 1;
// GLOBAL_VARIABLES_INITIALISATION_AREA_END

/**
 * @func main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // initialize glut
    glutInit(&argc, argv);

    // initialize window position
    glutInitWindowPosition(10, 10);

    // window size
    glutInitWindowSize(1200, 600);

    // display mode
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // create a window
    glutCreateWindow("Assignment 1 - Graphics 1 (CSY2033)");

    // connect to the glut functions and override them with ours
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMovement);

    // initialize the OpenGL library
    initOpenGL();

    // glut main loop
    glutMainLoop();

    return 0;
}

/**
 * @func display
 */
void display()
{
    // clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the pentagon
    glPushMatrix();
    glTranslatef(dx, dy, dz);
    glRotatef(pentagonAngle, 0.0, 0.0, 1.0);
    glScalef(pentagonScale, pentagonScale, pentagonScale);
    glCallList(listname);
    glPopMatrix();

    glutSwapBuffers();
}

/**
 * @func reshape
 * @param w
 * @param h
 */
void reshape(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
    {
        h = 1;
    }

    float ratio = 1.0 * w / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            0.0, 0.0, 5.0,     // eye's position
            0.0, 0.0, 1.0,     // center of interest
            0.0f, 1.0f, 0.0f     // up direction
    );
}

/**
 * @func idle
 */
void idle()
{
    if (idleState)
    {
        mouseX < 600 ? (pentagonAngle += idleAngle) : (pentagonAngle -= idleAngle);
    }
    glutPostRedisplay();
}

/**
 * @func mouseClick
 * @param mouseButton // mouseButton = GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, GLUT_MIDDLE_BUTTON
 * @param state
 * @param x
 * @param y
 */
void mouseClick(int mouseButton, int state, int x, int y)
{
    // store the idle state
    idleState = state != GLUT_DOWN;

    // assign the value of the button interacted
    // -1 means there is no button
    (state == GLUT_DOWN) ? (::mouseButton = mouseButton) : (::mouseButton = -1);

    // if the button was pressed
    if (state == GLUT_DOWN)
    {
        // the mouse coordinate is stored
        clickMouseX = x;
        clickMouseY = y;
    }
}

/**
 * @func mouseMovement
 * @param x
 * @param y
 */
void mouseMovement(int x, int y)
{
    // stores the mouse coordinate
    mouseX = x;
    mouseY = y;
}

/**
 * @func mouseMotion
 * @param x
 * @param y
 */
void mouseMotion(int x, int y)
{
    double mvMatrix[16];
    double projMatrix[16];
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

    double curY = double(viewport[3] - y);
    double objCurY, objCurX, objCurZ;
    gluUnProject((double) x, curY, 0.5, mvMatrix, projMatrix, viewport, &objCurX, &objCurY, &objCurZ);

    double preY = double(viewport[3] - clickMouseY);
    double objPreY, objPreX, objPreZ;
    gluUnProject((double) clickMouseX, preY, 0.5, mvMatrix, projMatrix, viewport, &objPreX, &objPreY, &objPreZ);

    switch (mouseButton)
    {
        case GLUT_LEFT_BUTTON:
            dx += (objCurX - objPreX) * 2.5;
            dy -= (objPreY - objCurY) * 2.5;
            break;
        case GLUT_RIGHT_BUTTON:
            pentagonAngle -= (clickMouseX - x);
            break;
        case GLUT_MIDDLE_BUTTON:
            pentagonScale -= (clickMouseX - x) * 0.02;
            break;
    }

    // stores the values of the mouse when it was clicked
    clickMouseX = x;
    clickMouseY = y;
}

/**
 * @func writeVertices
 */
void writeVertices()
{
    // open a file for writing pentagon vertices
    ofstream writeVertices("vertices.txt");

    // the angle of the vertex
    float a = 2 * pi / 5.0;
    float x1 = 0.0, y1 = 1.0;
    float x, y;
    int i;

    for (i = 0; i < 5; i++)
    {
        // calculates the coordinate of the vertex
        x = x1 * cos(a * i) - y1 * sin(a * i);
        y = y1 * cos(a * i) + x1 * sin(a * i);
        writeVertices << x << " " << y << endl;
    }
    writeVertices.close();
}

/**
 * @func readVertices
 */
void readVertices()
{
    ifstream readVertices("vertices.txt");
    int i;

    for (i = 0; i < 5; i++)
    {
        // read the values from the file
        readVertices >> vertices[i][0];
        readVertices >> vertices[i][1];

        if (debugEnabled)
        {
            cout << vertices[i][0] << " " << vertices[i][1] << endl;
        }
    }
}

/**
 * @func initOpenGL
 */
void initOpenGL()
{
    int i;

    glEnable(GL_DEPTH);
    writeVertices();
    readVertices();

    glNewList(listname, GL_COMPILE);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 5; i++)
    {
        glVertex3f(vertices[i][0], vertices[i][1], 0.0);
    }
    glEnd();
    glEndList();
}