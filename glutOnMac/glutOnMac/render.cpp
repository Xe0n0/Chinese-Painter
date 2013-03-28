//
//  render.cpp
//  glutOnMac
//
//  Created by William Yang on 28/03/2013.
//  Copyright (c) 2013 William Yang. All rights reserved.
//

#include "render.h"
#include <cmath>

const static float PI = 3.14159265359;
const static int circle_points = 50;

const static GLint vertices[] = {
    25, 25,
    100, 325,
    175, 25,
    175, 325,
    250, 25,
    325, 325
};

const static GLint real_vertices[] = {
    0, -25,
    25, 50,
    -25, 70
};
const static GLubyte indices[] = {0, 1, 2};

const static GLfloat colors[] ={
    1, 0.2, 0.2,
    0.2, 0.2, 1,
    0.8, 1, 0.2,
    0.75, 0.75, 0.75,
    0.35, 0.35, 0.35,
    0.5, 0.5, 0.5
};

static GLfloat spin;

void init(){
    glClearColor(1, 1, 1, 1);
    glShadeModel(GL_FLAT);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(2, GL_INT, 0, real_vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    /*glBegin(GL_TRIANGLES);
    {
        glArrayElement(0);
        glArrayElement(1);
        glArrayElement(2);
    }
    glEnd();*/
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, indices);
    glPushMatrix();
    glRotatef(spin, 0, 0, 1);
    glColor3f(1, 0.8, 0.3);
    glRectf(-25, -25, 25, 25);
    glPopMatrix();
    glColor3f(1, 0.2, 0.1);
    glLineStipple(1, 0xFFFF);
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    {
        for (int i = 0; i < circle_points; i++) {
            float angle = 2 * PI * i / circle_points;
            glVertex2f(50 * cos(angle), 50 * sin(angle));
        }
    }
    glEnd();
    glColor3f(0.1, 0.8, 0.1);
    glLineStipple(1, 0x0F0F);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        glVertex2f(-30, -30);
        glVertex2f(30, 30);
    }
    glEnd();
    
    glutSwapBuffers();
}

void reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50, 50, -50, 50, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spinDisplay(){
    spin += 2;
    if (spin > 360)
        spin -= 360;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN){
                glutIdleFunc(spinDisplay);
//                glColor3f(0, 0, 0);
//                glBegin(GL_POINTS);
//                {
//                    glVertex2f(x, y);
//                }
//                glEnd();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(NULL);
            break;
        default:
            break;
    }
}
