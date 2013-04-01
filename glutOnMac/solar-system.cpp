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
const static GLfloat ambientLight0[] = { 1.0, 0.85, 0.35, 1.0 };
const static GLfloat diffuseLight0[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat lightPosition0[] = { 0.0, 0.0, 10.0, 0};

static GLint year1 = 0, day1 = 0, year2 = 20, day2 = 0, year3 = 0;


static void transformGLUT(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*eyes at (0, -10, 5) looking at (0, 0, 0), up is (0, 0, 1)*/
    gluLookAt(0.0, -10.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    
    glutSolidSphere(1.0, 60, 45);

    glPushMatrix();{
        glRotatef(year1, 0.0, 0.0, 1.0);
        glTranslatef(2.0, 0.0, 0.0);
        glRotatef(day1, 0.0, 0.0, 1.0);
        glutSolidSphere(0.2, 15, 12);
    }glPopMatrix();
    
    glPushMatrix();{
        glRotatef(year2, 0.0, 0.0, 1.0);
        glTranslatef(5, 0.0, 0.0);
        glPushMatrix();{
            glRotatef(day2, 0.0, 0.0, 1.0);
            glutSolidSphere(0.5, 30, 15);
        }glPopMatrix();
        glPushMatrix();{
            glRotatef(year3, 0.0, 0.0, 1.0);
            glTranslatef(2, 0.0, 0.0);
            glutSolidSphere(0.1, 10, 8);
        }glPopMatrix();
    }glPopMatrix();

    glutSwapBuffers();
}

static void keyboard(GLubyte key, GLint x, GLint y)
{
    switch (key){
    case '1':
        year1 = (year1 + 5) % 360;
        break;
    case '2':
        year2 = (year2 + 10) % 360;
        break;
    case '3':
        day1 = (day1 + 10) % 360;
        break;
    case '4':
        day2 = (day2 + 15) % 360;
        break;
    case '5':
        year3 = (year3 + 12) % 360;
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
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
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