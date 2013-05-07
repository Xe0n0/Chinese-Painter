#include <cstdio>
#include <cmath>
#include <png.h>
#include <cstdlib>
#include <sys/time.h>
//Windows:#include <GL/glut.h>
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>

const static int initPosX = 0, // initial window position relative to 
                 initPosY = 0; // upper-left corner of the screen, 
                                 // in pixels.
const static int initWidth = 1024,   // initial window size,
                 initHeight = 768;  // in pixels.

/* how many mili-seconds to wait for a FPS calculation 
   smaller is more frequent.
*/
const static int FPS_SAMPLE_INTERVAL = 1000;

/* fixed FPS for video output */
const static GLfloat FIXED_FPS = 24.0;

const static CGLPixelFormatAttribute pixelattrib[15] = {
    kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
    kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
    kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
    kCGLPFAAccelerated,
    kCGLPFAMultisample,
    kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
    kCGLPFASamples,       (CGLPixelFormatAttribute)4,
    (CGLPixelFormatAttribute)0
};

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

const static GLenum drawbufs[1] = {GL_COLOR_ATTACHMENT0};

/* period of revolution and rotation (seconds for 360 degrees)*/
const static GLfloat YEAR_LEN1 = 3, DAY_LEN1 = 1,
                     YEAR_LEN2 = 7, DAY_LEN2 = 3,
                     YEAR_LEN3 = 2;

static bool paused = false;

/* current position of planets and their satelites, in degrees*/
static GLfloat year1 = 0, day1 = 0, year2 = 20, day2 = 0, year3 = 90;

static GLint frameCnt = 0;
static GLint lastFPSTime = 0, lastTime = 0, currentTime = 0;
static GLfloat frameRate;

static GLfloat eyeX = 0.0, eyeY = -7.0, eyeZ = 3.0;

static GLuint framebuffer, colorbuffer, depthbuffer;

static CGLPixelFormatObj pixelformat;
static CGLContextObj context;

static png_byte * row_pointers[initHeight];
static png_byte bitmap[initWidth * initWidth * 3];

static unsigned int getMilisec(){
// UNIX
    timeval t;
    gettimeofday(&t, NULL);
    return (int)t.tv_sec * 1000 + (int)t.tv_usec / 1000;
// Windows
// not implemented
}

static void setView(GLsizei width, GLsizei height){
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*eyes at (eyeX, eyeY, eyeZ) looking at (0, 0, 0), up is (0, 0, 1)*/
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}


static void initFBO(GLfloat width, GLfloat height){
    // frame buffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // color buffer
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorbuffer);
    // depth buffer
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,  GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    //
    glDrawBuffers(1, drawbufs);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=  GL_FRAMEBUFFER_COMPLETE){
        printf("OK, we're in trouble\n");
        exit(0);
    }
}

static void initLight(){
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
    
    GLenum glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after setting lights: 0x%x\n", glerr);
        exit(0);
    }
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after enable lighting: 0x%x\n", glerr);
        exit(0);
    }

    glEnable(GL_DEPTH_TEST);

    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after enable depth test: 0x%x\n", glerr);
        exit(0);
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}

static void initGL(GLfloat width, GLfloat height)
{
    GLint npix;
    CGLError error = CGLChoosePixelFormat(pixelattrib, &pixelformat, &npix);
    if (error != kCGLNoError){
        printf("CGLError: %s\n", CGLErrorString(error));
        exit(0);
    }
    error = CGLCreateContext(pixelformat, NULL, &context);
    if (error != kCGLNoError){
        printf("CGLError: %s\n", CGLErrorString(error));
        exit(0);
    }
    error = CGLSetCurrentContext(context);
    if (error != kCGLNoError){
        printf("CGLError: %s\n", CGLErrorString(error));
        exit(0);
    }
    // CGLLockContext(context);
    initFBO(width, height);
    GLenum glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after initFBO: 0x%x\n", glerr);
        exit(0);
    }
    glClearColor(1.0, 1.0, 0.0, 0.0);
    //glShadeModel(GL_SMOOTH);
    //initLight();
    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after initLight: 0x%x\n", glerr);
        exit(0);
    }
    //setView(width, height);
    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after setView: 0x%x\n", glerr);
        exit(0);
    }
    lastFPSTime = getMilisec();
    printf("finished initialisation.\n");
}

static void cleanup(){
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &colorbuffer);
    glDeleteRenderbuffers(1, &depthbuffer);
    CGLSetCurrentContext(NULL);
    CGLDestroyPixelFormat(pixelformat);
    CGLDestroyContext(context);
}

static void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(1.0, 0.0, 0.0);

    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    
    /* sun */
    glutSolidSphere(0.8, 60, 45);

    /* entering a new coordinate system
       planet 1
    */
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
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

    /* wait until finish */
    glFinish();
}


static void sample_display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLenum glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after glClear: 0x%x\n", glerr);
        exit(0);
    }
    glEnd();
    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error before glBegin: 0x%x\n", glerr);
        exit(0);
    }
    glBegin(GL_TRIANGLES);
    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after glBegin: 0x%x\n", glerr);
        exit(0);
    }
    //
    glEnd();
    glerr = glGetError();
    if (glerr != GL_NO_ERROR){
        printf("OpenGL Error after glEnd: 0x%x\n", glerr);
        exit(0);
    }
}

static void oneFrame(){
    display();
    //sample_display();
    frameCnt ++;
    currentTime = getMilisec();
    if (currentTime - lastFPSTime >= FPS_SAMPLE_INTERVAL){
        frameRate = (GLfloat) frameCnt / (currentTime - lastFPSTime) * 1000.0;
        printf("rendering FPS: %.2f\n", frameRate);
        frameCnt = 0;
        lastFPSTime = currentTime;
    }
    /*
    GLint diff = currentTime - lastTime;
    lastTime = currentTime;
    if (paused)
        return;
    */
    year1 += 360.0 / YEAR_LEN1 / FIXED_FPS;
    day1 += 360.0 / DAY_LEN1 / FIXED_FPS;
    year2 += 360.0 / YEAR_LEN2 / FIXED_FPS;
    day2 += 360.0 / DAY_LEN2 / FIXED_FPS;
    year3 += 360.0 / YEAR_LEN3 / FIXED_FPS;
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
    FILE *fp = fopen(filename, "wb");
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    /*glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(initWidth, initHeight);
    glutInitWindowPosition(initPosX, initPosY);
    glutCreateWindow(argv[0]);*/

    if (argc != 2){
        printf("Usage: %s seconds_of_video_to_render\n", argv[0]);
        exit(0);
    }

    float secs = atof(argv[1]);
    int framesToRender = ceil(secs * FIXED_FPS);
    
    printf("starting! %d frames to render!\n", framesToRender);
    initGL(initWidth, initHeight);

    for (int i = 0; i < initHeight; i++)
        row_pointers[initHeight - i - 1] = bitmap + initWidth * i * 3;

    for (int i = 0; i < framesToRender; i++){
        oneFrame();
        writePNG(i);
    }
    cleanup();
    return 0;
}