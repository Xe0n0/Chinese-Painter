//
//  render.cpp
//  glutOnMac
//
//  Created by William Yang on 28/03/2013.
//  Copyright (c) 2013 William Yang. All rights reserved.
//

#include "render.h"

static GLfloat spin;

void init(){
    glClearColor(0.5, 0.3, 0.1, 0);
    glShadeModel(GL_FLAT);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glRotatef(spin, 0, 0, 1);
    glColor3f(1, 0.8, 0.3);
    glRectf(-25, -25, 25, 25);
    glPopMatrix();
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
            if (state == GLUT_DOWN)
                glutIdleFunc(spinDisplay);
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(NULL);
            break;
        default:
            break;
    }
}
