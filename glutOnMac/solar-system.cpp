#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <GLUT/glut.h>

const static int initPosX = 0, // initial window position relative to 
                 initPosY = 0; // upper-left corner of the screen, 
                                 // in pixels.
const static int initWidth = 1000,   // initial window size,
                 initHeight = 600;  // in pixels.

/* how many mili-seconds to wait for a FPS calculation 
   smaller is more frequent.
*/
const static int FPS_SAMPLE_INTERVAL = 1000;

/* light 0 */
const static GLfloat ambientLight0[] = { 1.0, 0.85, 0.35, 1.0 };
const static GLfloat diffuseLight0[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat lightPosition0[] = { 0.0, 0.0, 10.0, 0};

/* period of revolution and rotation (miliseconds for 360 degrees)*/
const static GLfloat YEAR_LEN1 = 3000, DAY_LEN1 = 1000,
                     YEAR_LEN2 = 5000, DAY_LEN2 = 3000,
                     YEAR_LEN3 = 2000;

/* current position of planets and their satelites, in degrees*/
static GLfloat year1 = 0, day1 = 0, year2 = 20, day2 = 0, year3 = 90;

static GLint frameCnt = 0;
static GLint lastFPSTime = 0, lastTime = 0, currentTime = 0;
static GLfloat frameRate;

static GLfloat eyeX = 0.0, eyeY = -7.0, eyeZ = 3.0;

static void changeView(){
    /*eyes at (eyeX, eyeY, eyeZ) looking at (0, 0, 0), up is (0, 0, 1)*/
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    changeView();
}

static void initGLUT(GLfloat width, GLfloat height)
{    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition0);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    reshape(width, height);
    lastTime = lastFPSTime = glutGet(GLUT_ELAPSED_TIME);
}


static void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);
    
    /* sun */
    glutSolidSphere(0.8, 60, 45);

    /* entering a new coordinate system
       planet 1
    */
    glPushMatrix();{
        /* revolution 公转 */
        glRotatef(year1, 0.0, 0.0, 1.0);
        /* move planet 1 to its orbit */
        glTranslatef(2.0, 0.0, 0.0);
        /* rotation 自转, axis is 45 degrees to the orbit plane */
        glRotatef(day1, 0.0, 0.0, 1.0);
        glutSolidSphere(0.2, 24, 15);
    }glPopMatrix();
    
    /* entering another new coordinate system
       planet 2
    */
    glPushMatrix();{
        /* revolution */
        glRotatef(year2, 0.0, 0.0, 1.0);
        /* planet 2's orbit */
        glTranslatef(4, 0.0, 0.0);
        /* entering a sub-coordinate system of planet 2 */
        glPushMatrix();{
            /* rotation of planet 2*/
            glRotatef(day2, 0.0, 0.0, 1.0);
            glutSolidSphere(0.5, 45, 24);
        }glPopMatrix();
        /* entering a sub-coordinate system of planet 2's satelite */
        glPushMatrix();{
            /* revolution of the satelite */
            glRotatef(year3, 0.0, 0.0, 1.0);
            glTranslatef(1, 0.0, 0.0);
            /* the satelite has no rotation*/
            glutSolidSphere(0.1, 15, 12);
        }glPopMatrix();
    }glPopMatrix();

    glutSwapBuffers();
}

static void keyboard(GLubyte key, GLint x, GLint y)
{
    switch (key){
    /*case '1':
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
    case 'x':
        eyeX += 0.2;
        break;
    case 'X':
        eyeX -= 0.2;
        break;
    case 'y':
        eyeY += 0.2;
        break;
    case 'Y':
        eyeY -= 0.2;
        break;
    case 'z':
        eyeZ += 0.2;
        break;
    case 'Z':
        eyeY -= 0.2;
        break;
    case 'd':
        glEnable(GL_DEPTH_TEST);
        break;
    case 'D':
        glDisable(GL_DEPTH_TEST);
        break;
    case 'l':
        glEnable(GL_LIGHTING);
        break;
    case 'L':
        glDisable(GL_LIGHTING);
        break;
    default: return;*/
    }
    changeView();
    glutPostRedisplay();
}

static void idle(){
    frameCnt ++;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastFPSTime >= FPS_SAMPLE_INTERVAL){
        frameRate = (GLfloat) frameCnt / (currentTime - lastFPSTime) * 1000.0;
        printf("FPS: %.2f\n", frameRate);
        frameCnt = 0;
        lastFPSTime = currentTime;
    }
    GLint diff = currentTime - lastTime;
    lastTime = currentTime;
    year1 += 360 / YEAR_LEN1 * diff;
    day1 += 360 / DAY_LEN1 * diff;
    year2 += 360 / YEAR_LEN2 * diff;
    day2 += 360 / DAY_LEN2 * diff;
    year3 += 360 / YEAR_LEN3 * diff;
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
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    //glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}