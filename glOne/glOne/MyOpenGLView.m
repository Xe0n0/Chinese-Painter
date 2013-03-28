//
//  MyOpenGLView.m
//  glOne
//
//  Created by William Yang on 27/03/2013.
//  Copyright (c) 2013 William Yang. All rights reserved.
//

#import "MyOpenGLView.h"
#include <OpenGL/gl.h>

@implementation MyOpenGLView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawAnObject();
    glFlush();
    // Drawing code here.
}

static void drawAnObject(){
    glColor3f(1.0f, 0.85f, 0.35f);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f( 0.0,  0.6, 0);
        glVertex3f(-0.2, -0.3, 0);
        glVertex3f( 0.2, -0.3, 0);
    }
    glEnd();
}

@end
