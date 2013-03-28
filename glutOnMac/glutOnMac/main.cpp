//
//  main.cpp
//  glutOnMac
//
//  Created by William Yang on 28/03/2013.
//  Copyright (c) 2013 William Yang. All rights reserved.
//

#include <iostream>
#include "render.h"

int main(int argc, char * argv[])
{
    std::cout << "Hello, World!\n";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(250, 250);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}

