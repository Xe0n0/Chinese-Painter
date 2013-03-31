#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <GLUT/glut.h>

const static int initPosX = 250, // initial window position relative to 
                 initPosY = 250; // upper-left corner of the screen, 
                                 // in pixels.
const static int initWidth = 500,   // initial window size,
                 initHeight = 500;  // in pixels.

/* light 0 */
const static GLfloat ambientLight0[] = { 0.7, 0.25, 0.25, 1.0 };
const static GLfloat diffuseLight0[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat lightPosition0[] = { 3.0, 3.0, 3.0, 0};

static GLint year = 0, day = 0;


static void transformGLUT(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static void initGLUT(GLfloat width, GLfloat height)
{    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition0);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    //transformGLUT(width, height);
}


static void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glutWireSphere(1.0, 20, 16);
    glRotatef(year, 0.0, 0.0, 1.0);
    glTranslatef(2.0, 0.0, 0.0);
    glRotatef(day, 0.0, 0.0, 1.0);
    glutWireSphere(0.2, 10, 8);
    glPopMatrix();
    glutSwapBuffers();
}

static void keyboard(GLubyte key, GLint x, GLint y)
{
    switch (key){
    case 'y':
        year = (year + 5) % 360;
        break;
    case 'Y':
        year = (year - 5) % 360;
        break;
    case 'd':
        day = (day + 10) % 360;
        break;
    case 'D':
        day = (day - 10) % 360;
        break;
    case 'l':
        glEnable(GL_LIGHTING);  /* enable lighting */  
        break;
    case 'L':
        glDisable(GL_LIGHTING);  /* enable lighting */  
        break;
    
    default: return;
    }
    glutPostRedisplay();
}



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(initWidth, initHeight);
    glutInitWindowPosition(initPosX, initPosY);
    glutCreateWindow(argv[0]);
    initGLUT(initWidth, initHeight);
    glutDisplayFunc(display);
    glutReshapeFunc(transformGLUT);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}