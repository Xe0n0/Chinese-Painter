//
//  render.h
//  glutOnMac
//
//  Created by William Yang on 28/03/2013.
//  Copyright (c) 2013 William Yang. All rights reserved.
//

#ifndef __glutOnMac__render__
#define __glutOnMac__render__

#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

void init();
void display();
void reshape(int, int);
void mouse(int, int, int, int);

#endif /* defined(__glutOnMac__render__) */
