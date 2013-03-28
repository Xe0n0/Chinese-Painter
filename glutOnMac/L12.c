/* L12.c
           OpenGL Framework using GLUT 3.7 

           Rob Fletcher  2001

           Draw a Glut object and apply lighting
           and also keep the x,y,z rotations

*/

#include <stdio.h>
#include <stdlib.h>     /* For "exit" prototype */
#include <GLUT/glut.h>    /* Header File For The GLUT Library */


/* ASCII code for the escape key. */
#define ESCAPE 27

GLint window;           /* The number of our GLUT window */
GLint Xsize=400;
GLint Ysize=400;

GLfloat xangle=0.0,yangle=0.0,zangle=0.0;   /* axis angles */


/* Simple window transformation routine */
GLvoid Transform(GLfloat Width, GLfloat Height)
{
  glViewport(0, 0, Width, Height);              /* Set the viewport */
  glMatrixMode(GL_PROJECTION);                  /* Select the projection matrix */
  glLoadIdentity();				/* Reset The Projection Matrix */
  gluPerspective(45.0,Width/Height,0.1,100.0);  /* Calculate The Aspect Ratio Of The Window */
  glMatrixMode(GL_MODELVIEW);                   /* Switch back to the modelview matrix */
}


/* A general OpenGL initialization function.  Sets all of the initial parameters. */
GLvoid InitGL(GLfloat Width, GLfloat Height)	
{

GLfloat LightAmbient[] = { 0.8, 0.0, 0.0, 1.0 };  /* reddish ambient light  */
GLfloat LightDiffuse[] = { 0.1, 0.1, 0.9, 1.0 };  /* bluish  diffuse light. */
GLfloat LightPosition[] = { 1.5, 1.5, 1.5, 0.0 };    /* position */

  glClearColor(0.0, 0.0, 0.0, 0.0);		/* This Will Clear The Background Color To Black */
  glShadeModel(GL_SMOOTH);

  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);  /*  add lighting. (ambient) */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);  /*  add lighting. (diffuse). */
  glLightfv(GL_LIGHT0, GL_POSITION,LightPosition); /*  set light position. */
  glEnable(GL_LIGHT0);                             /*  turn light 0 on. */

  Transform( Width, Height );                   /* Perform the transformation */
}

/* The function called when our window is resized  */
GLvoid ReSizeGLScene(GLint Width, GLint Height)
{
  if (Height==0)    Height=1;                   /* Sanity checks */
  if (Width==0)     Width=1;
  Transform( Width, Height );                   /* Perform the transformation */
}


/* The main drawing function

   In here we put all the OpenGL and calls to routines which manipulate
   the OpenGL state and environment.

   This is the function which will be called when a "redisplay" is requested.
*/

GLvoid DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	/* Clear The Screen And The Depth Buffer */

  glPushMatrix();
  glLoadIdentity();

  glTranslatef(0.0,0.0,-6.0);
  glRotatef(xangle,1.0,0.0,0.0);
  glRotatef(yangle,0.0,1.0,0.0);
  glRotatef(zangle,0.0,0.0,1.0);

    //glutSolidCube(2.0);

/* Other GLUT objects you may wish to try */

//  glutSolidSphere(2.0,20,20);
//  glutSolidCone(1.0,2.5,20,20);
//  glutSolidTorus(0.4,1.0,20,20);
//  glutSolidDodecahedron();
//  glutSolidOctahedron();
//  glutSolidTetrahedron();
    glutSolidIcosahedron();
//glutSolidTeapot(1.0);

  glPopMatrix();
  glFlush(); 

}


/*  The function called whenever a "normal" key is pressed. */
void NormalKey(GLubyte key, GLint x, GLint y) 
{
    switch ( key )    { 
     case ESCAPE :
        printf("escape pressed. exit.\n");
	glutDestroyWindow(window);	/* Kill our window */
	exit(0); 			/* Very dirty exit */                  
        break;				/* Do we need this??? */


     case 'x':
        xangle += 5.0;
        glutPostRedisplay();
        break;
     case 'X':
        xangle -= 5.0;
        glutPostRedisplay();
        break;

     case 'y':
        yangle += 5.0;
        glutPostRedisplay();
        break;
     case 'Y':
        yangle -= 5.0;
        glutPostRedisplay();
        break;

     case 'z':
        zangle += 5.0;
        glutPostRedisplay();
        break;
     case 'Z':
        zangle -= 5.0;
        glutPostRedisplay();
        break;

     case 'd':
        glEnable(GL_DEPTH_TEST);
        glutPostRedisplay();
        break;

     case 'D':
        glDisable(GL_DEPTH_TEST);
        glutPostRedisplay();
        break;

     case 'l':
       glEnable(GL_LIGHTING);  /* enable lighting */  
       glutPostRedisplay();
        break;

     case 'L':
       glDisable(GL_LIGHTING);  /* enable lighting */  
       glutPostRedisplay();
        break;
 
     default:
	break;
    }

}

/*************************** Main ***************************************************************/

int main(int argc, char **argv) 
{  

/* Initialisation and window creation */

  glutInit(&argc, argv);               /* Initialize GLUT state. */

  glutInitDisplayMode(GLUT_RGBA |      /* RGB and Alpha */
                      GLUT_SINGLE|     /* Single buffer */
                      GLUT_DEPTH);     /* Z buffer (depth) */

  glutInitWindowSize(Xsize,Ysize);         /* set initial window size. */
  glutInitWindowPosition(0,0);         /* upper left corner of the screen. */

  window = glutCreateWindow("L12");     /* Open a window with a title. */ 
  InitGL(Xsize,Ysize);                     /* Initialize our window. */

/* Now register the various callback functions */

  glutDisplayFunc(DrawGLScene);        /* Function to do all our OpenGL drawing. */
  glutReshapeFunc(ReSizeGLScene);
  glutKeyboardFunc(NormalKey);         /*Normal key is pressed */

/* Now drop into the event loop from which we never return */

  glutMainLoop();                      /* Start Event Processing Engine. */  
  return 1;
}
