#include <cstdio>
#include <GLUT/glut.h>

const static int initPosX = 250, // initial window position relative to 
                 initPosY = 250; // upper-left corner of the screen, 
                                 // in pixels.

const static int initWidth = 500,   // initial window size,
                 initHeight = 500;  // in pixels.


const static GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
const static GLfloat diffuseLight[] = { 1.0, 0.5, 0.5, 1.0 };
const static GLfloat lightPosition[] = { 2.0, 2.0, 2.0, 0};

GLfloat xangle = 0.0, yangle = 0.0, zangle = 0.0;
GLfloat xt = 0.0, yt = 0.0, zt = 0.0;


const static GLint l = 1;

const static GLint vertices[] = {
    l, l, l,
    l, l, -l,
    l, -l, l,
    l, -l, -l,
    -l, -l, l,
    -l, -l, -l,
    -l, l, l,
    -l, l, -l
};

const static GLubyte faceIndices[6][4] = {
    { 0, 2, 3, 1 },
    { 2, 3, 5, 4 },
    { 2, 0, 6, 4 },
    { 3, 5, 7, 1 },
    { 1, 7, 6, 0 },
    { 5, 4, 6, 7 }
};

const static GLsizei vertexCnt[] = { 4, 4, 4, 4, 4, 4 };

/*
const static GLfloat colors[] = {
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3,
    1.0, 0.8, 0.3
};
*/
static void drawSomething(){
    //glMultiDrawElements(GL_QUADS, vertexCnt, GL_UNSIGNED_BYTE, (GLvoid **)faceIndices, 6);
    for (int i = 0; i < 6; ++i)
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, faceIndices[i]);
}


static void transformGLUT(GLfloat width, GLfloat height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,width/height,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void initGLUT(GLfloat width, GLfloat height)
{    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_INT, 0, vertices);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glColorPointer(3, GL_FLOAT, 0, colors);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition);
    glEnable(GL_LIGHT0);
    transformGLUT(width, height);
}

static void reshape(GLint width, GLint height)
{
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    transformGLUT(width, height);
    //printf("transform to %d, %d\n", width, height);
}

static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0,0.0,-6.0);
    glTranslatef(xt,yt,zt);
    glRotatef(xangle,1.0,0.0,0.0);
    glRotatef(yangle,0.0,1.0,0.0);
    glRotatef(zangle,0.0,0.0,1.0);
    //glutSolidIcosahedron();
    glutSolidTorus(0.4,1.0,20,20);
    drawSomething();
    glPopMatrix();
    glFlush();
}

static void keyboard(GLubyte key, GLint x, GLint y)
{
    switch (key){
    case 'x':
        xangle += 5.0;
        break;
    case 'X':
        xangle -= 5.0;
        break;
    case 'y':
        yangle += 5.0;
        break;
    case 'Y':
        yangle -= 5.0;
        break;
    case 'z':
        zangle += 5.0;
        break;
    case 'Z':
        zangle -= 5.0;
        break;
    case 'd':
        glEnable(GL_DEPTH_TEST);
        break;
    case 'D':
        glDisable(GL_DEPTH_TEST);
        break;
    case 'l':
        glEnable(GL_LIGHTING);  /* enable lighting */  
        break;
    case 'L':
        glDisable(GL_LIGHTING);  /* enable lighting */  
        break;
    case '1':
        xt += 0.2;
        break;
    case '2':
        xt -= 0.2;
        break;
    case '3':
        yt += 0.2;
        break;
    case '4':
        yt -= 0.2;
        break;
    case '5':
        zt += 0.2;
        break;
    case '6':
        zt -= 0.2;
        break;
    default: return;
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(initWidth, initHeight);
    glutInitWindowPosition(initPosX, initPosY);
    glutCreateWindow(argv[0]);
    initGLUT(initWidth, initHeight);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}