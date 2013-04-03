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
const static GLfloat diffuseLight0[] = { 1.0, 0.5, 0.5, 1.0 };
const static GLfloat lightPosition0[] = { 2.0, 2.0, 2.0, 0};

/* light 1 */
const static GLfloat ambientLight1[] = { 0.0, 0.0, 0.0, 1.0 };
const static GLfloat diffuseLight1[] = { 0.0, 0.3, 0.15, 1.0 };
const static GLfloat lightPosition1[] = { -5.0, -5.0, 5.0, 0};


GLfloat xangle = 0.0, yangle = 0.0, zangle = 0.0;
GLfloat xt = 0.0, yt = 0.0, zt = 0.0;

/*
const static GLint num_buffers = 2;
const static GLint vertex_buf = 0;
const static GLint index_buf = 0;
static GLuint buffers[num_buffers];

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

static bool divide = false;

/* icosahedron */
const static GLfloat X = 0.525731112119133606, Z = 0.850650808352039932;
const static GLfloat vertices[12][3] = {
    { -X, 0.0, Z }, { X, 0.0, Z }, { -X, 0.0, -Z }, { X, 0.0, -Z },
    { 0.0, Z, X }, { 0.0, Z, -X }, { 0.0, -Z, X }, { 0.0, -Z, -X },
    { Z, X, 0.0 }, { -Z, X, 0.0 }, { Z, -X, 0.0 }, { -Z, -X, 0.0 },
};
const static GLuint indices[20][3] = {
    {1, 4, 0}, {4, 9 ,0}, {4, 5, 9}, {8, 5, 4}, {1, 8, 4},
    {1, 10, 8}, {10, 3, 8}, {8, 3, 5}, {3, 2, 5}, {3, 7, 2},
    {3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11}, {6, 1, 0},
    {10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7}
};

static void normalize3fv(GLfloat *v){
    static GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (d == 0.0){
        printf("invalid normal vector!\n");
        exit(1);
    }
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

static GLfloat* getNormal3pv(const GLfloat *p1, const GLfloat *p2, const GLfloat *p3){
    static GLfloat norm[3], v1[3], v2[3], d;
    for (int i = 0; i < 3; ++i){
        v1[i] = p2[i] - p1[i];
        v2[i] = p3[i] - p2[i];
    }
    norm[0] = v1[1] * v2[2] - v1[2] * v2[1];
    norm[1] = v1[2] * v2[0] - v1[0] * v2[2];
    norm[2] = v1[0] * v2[1] - v1[1] * v2[0];
    normalize3fv(norm);
    return norm;
}

static void drawTriangle3pv(const GLfloat *p1, const GLfloat *p2, const GLfloat *p3){
    glBegin(GL_TRIANGLES);
    glNormal3fv(getNormal3pv(p1, p2, p3));
    glVertex3fv(p1);
    glVertex3fv(p2);
    glVertex3fv(p3);
    glEnd();
}

static void drawDividedTriangles(const GLuint *indices){
    static GLfloat v01[3], v02[3], v12[3];
    for (int i = 0; i < 3; i++){
        v01[i] = (vertices[indices[0]][i] + vertices[indices[1]][i])/2;
        v02[i] = (vertices[indices[0]][i] + vertices[indices[2]][i])/2;
        v12[i] = (vertices[indices[1]][i] + vertices[indices[2]][i])/2;
    }
    normalize3fv(v01);
    normalize3fv(v02);
    normalize3fv(v12);
    drawTriangle3pv(vertices[indices[0]], v01, v02);
    drawTriangle3pv(v01, vertices[indices[1]], v12);
    drawTriangle3pv(v12, vertices[indices[2]], v02);
    drawTriangle3pv(v01, v12, v02);
}

static void drawSomething(){
    //glMultiDrawElements(GL_QUADS, vertexCnt, GL_UNSIGNED_BYTE, (GLvoid **)faceIndices, 6);
    // bloody hell, multidraw causes seg fault!
     for (int i = 0; i < 20; ++i){
        if (divide){
            drawDividedTriangles(indices[i]);
        }else{
            glNormal3fv(getNormal3pv(vertices[indices[i][0]], 
                vertices[indices[i][1]], vertices[indices[i][2]]));
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices[i]);
        }
     }
    //glDrawElements(GL_QUADS, 4*6, GL_UNSIGNED_BYTE, (GLubyte*)NULL);
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
    // glEnableClientState(GL_NORMAL_ARRAY);
    // glVertexPointer(3, GL_INT, 0, vertices);
    // glEnableClientState(GL_COLOR_ARRAY);
    // glColorPointer(3, GL_FLOAT, 0, colors);
    // glGenBuffers(num_buffers, buffers);
    // glBindBuffer(GL_ARRAY_BUFFER, buffers[vertex_buf]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    //    GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    // glNormalPointer(GL_FLOAT, 0, vertices);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[index_buf]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices,
    //     GL_STATIC_DRAW);

    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition0);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
    glLightfv(GL_LIGHT1, GL_POSITION,lightPosition1);
    glEnable(GL_LIGHT1);


    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_MULTISAMPLE);

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
    // glutSolidIcosahedron();
    //glutSolidTorus(0.4,1.0,20,20);
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
    case ',':
        divide = true;
        break;
    case '.':
        divide = false;
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