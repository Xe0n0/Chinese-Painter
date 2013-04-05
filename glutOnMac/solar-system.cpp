#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <png.h>
#include <GLUT/glut.h>

const static int initPosX = 0, // initial window position relative to 
                 initPosY = 0; // upper-left corner of the screen, 
                                 // in pixels.
const static int initWidth = 1024,   // initial window size,
                 initHeight = 768;  // in pixels.

/* how many mili-seconds to wait for a FPS calculation 
   smaller is more frequent.
*/
const static int FPS_SAMPLE_INTERVAL = 1000;

/* light 0 */
const static GLfloat ambientLight0[] = { 0.1, 0.0, 0.1, 1.0 };
const static GLfloat diffuseLight0[] = { 1.0, 1.0, 0.7, 1.0 };
const static GLfloat specularLight0[] = { 0.3, 0.3, 0.3, 1.0 };
const static GLfloat lightPosition0[] = { 10.0, 10.0, 10.0, 0};

/* light 1 */
const static GLfloat ambientLight1[] = { 0.05, 0.05, 0.05, 1.0 };
const static GLfloat diffuseLight1[] = { 0.1, 0.05, 0.05, 1.0 };
const static GLfloat specularLight1[] = { 0.1, 0.2, 0.1, 1.0 };
const static GLfloat lightPosition1[] = { -10.0, -10.0, -10.0, 0};

/* material */
const static GLfloat ambientMaterial[] = { 0.1, 0.1, 0.1, 1.0 };
const static GLfloat diffuseMaterial[] = { 0.3, 0.3, 0.3, 1.0 };
const static GLfloat specularMaterial[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat noEmission[] = { 0.0, 0.0, 0.0, 1.0};
const static GLfloat emission[] = { 0.3, 0.1, 0.1, 1.0};
const static GLfloat shininess = 0.7;
/* period of revolution and rotation (miliseconds for 360 degrees)*/
const static GLfloat YEAR_LEN1 = 3000, DAY_LEN1 = 1000,
                     YEAR_LEN2 = 7000, DAY_LEN2 = 3000,
                     YEAR_LEN3 = 2000;

const static unsigned char BMP_HEADER[54] = {0x42, 0x4d, 0x6, 0x6d,
    0x25, 0x0, 0x0, 0x0, 0x0, 0x0, 0x36, 0x0, 0x0, 0x0, 0x28, 0x0,
    0x0, 0x0, 0xbd, 0x3, 0x0, 0x0, 0x56, 0x3, 0x0, 0x0, 0x1, 0x0,
    0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd0, 0x6c, 0x25, 0x0, 0xc2,
    0x1e, 0x0, 0x0, 0xc2, 0x1e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0};

static png_image png;

static bool paused = false;

/* current position of planets and their satelites, in degrees*/
static GLfloat year1 = 0, day1 = 0, year2 = 20, day2 = 0, year3 = 90;

static GLint frameCnt = 0;
static unsigned int filenameCnt = 0;
static GLint lastFPSTime = 0, lastTime = 0, currentTime = 0;
static GLfloat frameRate;

static GLfloat eyeX = 0.0, eyeY = -7.0, eyeZ = 3.0;

static png_byte * row_pointers[initHeight];
static png_byte bitmap[initWidth * initWidth * 3], 
       convertedBitmap[initWidth * initHeight * 3];

static void changeView(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*eyes at (eyeX, eyeY, eyeZ) looking at (0, 0, 0), up is (0, 0, 1)*/
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 100.0);
    changeView();
}

static void initGLUT(GLfloat width, GLfloat height)
{    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
    glLightfv(GL_LIGHT1, GL_POSITION,lightPosition1);
    
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    reshape(width, height);
    lastTime = lastFPSTime = glutGet(GLUT_ELAPSED_TIME);
}

static void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    
    /* sun */
    glutSolidSphere(0.8, 60, 45);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
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
    /*
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
    */
    case 'p':
        paused = true;
        break;
    case 'P':
        paused = false;
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
        eyeZ -= 0.2;
        break;
    /*
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
        break;*/
    default: return;
    }
    changeView();
    glutPostRedisplay();
}

static void writeBMP(int id){
    glPixelStorei(GL_UNPACK_ALIGNMENT, sizeof(int));
    glReadPixels(0, 0, initWidth, initHeight, GL_BGR, GL_UNSIGNED_BYTE, bitmap);
    static char filename[32];
    sprintf(filename, "output/%06d.bmp", id);
    FILE *fp = fopen(filename, "wb");
    fwrite(BMP_HEADER, sizeof(BMP_HEADER), 1, fp);
    fseek(fp, 0x0012, SEEK_SET);
    fwrite(&initWidth, sizeof(int), 1, fp);
    fwrite(&initHeight, sizeof(int), 1, fp);
    fseek(fp, 0, SEEK_END);
    fwrite(bitmap, sizeof(bitmap), 1, fp);
    fclose(fp);
} 

static void initPNG(){
    for (int i = 0; i < initHeight; i++)
        row_pointers[initHeight - i - 1] = bitmap + initWidth * i * 3;
    // memset(&png, 0, sizeof(png));
    // png.version = PNG_IMAGE_VERSION;
    // png.width = initWidth;
    // png.height = initHeight;
    // png.format = PNG_FORMAT_RGB;
}

static void writePNG(int id){
    glReadPixels(0, 0, initWidth, initHeight, GL_RGB, GL_UNSIGNED_BYTE, bitmap);
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct (png_ptr);
    png_set_IHDR (png_ptr, info_ptr, initWidth, initHeight, 8,
                  PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    static char filename[32];
    sprintf(filename, "output/%06d.png", id);
    // png_image_write_to_file(&png, filename, 0, bitmap, 3 * initWidth, NULL);
    FILE *fp = fopen(filename, "wb");
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    fclose(fp);
}

static void idle(){
    writePNG(filenameCnt);
    frameCnt ++;
    filenameCnt ++;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastFPSTime >= FPS_SAMPLE_INTERVAL){
        frameRate = (GLfloat) frameCnt / (currentTime - lastFPSTime) * 1000.0;
        printf("FPS: %.2f\n", frameRate);
        frameCnt = 0;
        lastFPSTime = currentTime;
    }
    GLint diff = currentTime - lastTime;
    lastTime = currentTime;
    if (paused)
        return;
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
    glutKeyboardFunc(keyboard);
    initPNG();
    glutMainLoop();
    return 0;
}