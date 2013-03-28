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
    glutInitWindowPosition(250, 250);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout << glGetString(GL_VENDOR) << std::endl;
    int i;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &i);
    std::cout<<"max texture units:"<<i<<std::endl;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
    std::cout<<"max texture size:"<<i<<std::endl;
    glutMainLoop();
    return 0;
}

