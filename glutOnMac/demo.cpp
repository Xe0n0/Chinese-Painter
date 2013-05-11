#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <png.h>
#include <GLUT/glut.h>
#include <vector>
#include <set>
using namespace std;

const static int LINE_BUF_LEN = 1024;

const static int initPosX = 0, // initial window position relative to 
                 initPosY = 0; // upper-left corner of the screen, 
                                 // in pixels.
const static int initWidth = 1024,   // initial window size,
                 initHeight = 768;  // in pixels.

/* how many mili-seconds to wait for a FPS calculation 
   smaller is more frequent.
*/
const static int FPS_SAMPLE_INTERVAL = 1000;

static GLfloat eyeX = 0.0, eyeY = -10.0, eyeZ = 10.0;
/* light 0 */
const static GLfloat ambientLight0[] = { 0.1, 0.1, 0.1, 1.0 };
const static GLfloat diffuseLight0[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat specularLight0[] = { 0.3, 0.3, 0.3, 1.0 };
const static GLfloat lightPosition0[] = { eyeX, eyeY, eyeZ, 0};

/* material */
const static GLfloat ambientMaterial[] = { 1, 1, 1, 1.0 };
const static GLfloat diffuseMaterial[] = { 0.3, 0.3, 0.3, 1.0 };
const static GLfloat specularMaterial[] = { 1.0, 1.0, 1.0, 1.0 };
const static GLfloat noEmission[] = { 0.0, 0.0, 0.0, 1.0};
const static GLfloat emission[] = { 0.3, 0.1, 0.1, 1.0};
const static GLfloat shininess = 0.7;


const static GLfloat PI = 3.141592653589793;
const static GLfloat PROFILE_ANGLE = 85;
const static GLfloat PROFILE_DOTP = cos(PROFILE_ANGLE*PI/180.0);

static png_image png;


static GLint frameCnt = 0;
static unsigned int filenameCnt = 0;
static GLint lastFPSTime = 0, lastTime = 0, currentTime = 0;
static GLfloat frameRate;

static GLfloat xangle = -90.0, yangle = 0.0, zangle = 0.0;
static GLfloat xt = -2.0, yt = -1.0, zt = 0.0;

static int windowWidth = initWidth, windowHeight = initHeight;

static png_byte * row_pointers[initHeight];
static png_byte bitmap[initWidth * initWidth * 3], 
       convertedBitmap[initWidth * initHeight * 3];

const static int VERTEX_FACE_ARRAY_LEN = 8;
typedef struct {
    int vid;
    int faceCnt;
    int arrayLen;
    int *faces;
}VertexRef;

inline static void normalize3fv(GLfloat *v){
    static GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (d == 0.0){
        fprintf(stderr, "invalid normal vector!\n");
        exit(1);
    }
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

inline static GLfloat *getNormal3Pts(const GLfloat *p1, const GLfloat *p2, const GLfloat *p3){
    static GLfloat norm[3], v1[3], v2[3];
    for (int i = 0; i < 3; ++i){
        v1[i] = p2[i] - p1[i];
        v2[i] = p3[i] - p2[i];
    }
    norm[0] = v1[1] * v2[2] - v1[2] * v2[1];
    norm[1] = v1[2] * v2[0] - v1[0] * v2[2];
    norm[2] = v1[0] * v2[1] - v1[1] * v2[0];
    //normalize3fv(norm);
    // without normalize, norm is propotional to area of the triangle. 
    return norm;
}

typedef struct{
    int faceN;
    int vertexN;
    int normalN;
    float scale;
    GLfloat *vertex3fPtr;
    GLfloat *normal3fPtr;
    GLint *face6iPtr;
    VertexRef *vrefPtr;
    // grey-scale of vertices
    GLfloat *vtGreyPtr;
    GLfloat *avgNormal3fPtr;
}Model;
static Model *globalModel;

inline static GLfloat absDotProduct(GLfloat *va, GLfloat *vb){
    return fabs(va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2]);
}

set<int> globalProfiles;

static void getVertexOnProfile(set<int> &profilePts, Model *model, GLfloat* eye3fv){
    static GLfloat sight[3];
    static GLfloat *vptr = model->vertex3fPtr;
    static GLfloat *nptr = model->normal3fPtr;
    static GLint *fptr = model->face6iPtr;
    profilePts.clear();
    for (int fi = 0; fi < model->faceN; fi++){
        // calculate dot-product of sight-vector and normal vector
        for (int i = 0; i < 3; i++){
            int vi = fptr[6*fi+2*i];
            int ni = fptr[6*fi+2*i+1];
            sight[0] = vptr[3*vi] - eye3fv[0];
            sight[1] = vptr[3*vi+1] - eye3fv[1];
            sight[2] = vptr[3*vi+2] - eye3fv[2];
            normalize3fv(sight);
            float dotp = absDotProduct(sight, nptr+3*ni);
            if (dotp <= PROFILE_DOTP){
                profilePts.insert(vi);
            }
        }
    }
}

static void displayTransform(){
    glLoadIdentity();
    glTranslatef(0.0,0.0,-3.0);
    glTranslatef(xt,yt,zt);
    glRotatef(xangle,1.0,0.0,0.0);
    glRotatef(yangle,0.0,1.0,0.0);
    glRotatef(zangle,0.0,0.0,1.0);
}

static GLfloat *windowCenterCoord(){
    static GLint viewport[4];
    static GLdouble modelview[16];
    static GLdouble projection[16];
    static GLdouble winX, winY, winZ;
    static GLdouble posX, posY, posZ;
    static GLfloat obj[3];
    glPushMatrix();
    displayTransform();
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glPopMatrix();
    winX = windowWidth / 2;
    winY = windowHeight / 2;
    winZ = 0;
    bool res = gluUnProject(winX, winY, winZ, modelview, 
        projection, viewport, &posX, &posY, &posZ);
    if (!res){
        fprintf(stderr, "failed converting coordinates.\n");
        exit(1);
    }
    obj[0] = posX;
    obj[1] = posY;
    obj[2] = posZ;
    return obj;
}

static Model *loadOBJModel(const char* filename){

    static char line_buf[LINE_BUF_LEN];
    const static char FACE_NUM[] = "faces:";
    const static char VERTEX_NUM[] = "vertices:";
    const static char NORMAL_NUM[] = "normals:";
    const static char SCALE_FACTOR[] = "scale-factor:";
    
    int faceN = 0, vertexN = 0, normalN = 0;
    float scale = 1.0;
    FILE *obj = fopen(filename, "r");
    // read comments, get number of vertices/faces
    while (fgets(line_buf, LINE_BUF_LEN, obj) != NULL && line_buf[0] == '#'){
        if (strncmp(line_buf+1, FACE_NUM, strlen(FACE_NUM)) == 0){
            sscanf(line_buf+1+strlen(FACE_NUM), "%d", &faceN);
        }
        else if (strncmp(line_buf+1, VERTEX_NUM, strlen(VERTEX_NUM)) == 0){
            sscanf(line_buf+1+strlen(VERTEX_NUM), "%d", &vertexN);
        }
        else if (strncmp(line_buf+1, SCALE_FACTOR, strlen(SCALE_FACTOR)) == 0){
            sscanf(line_buf+1+strlen(SCALE_FACTOR), "%f", &scale);
        }
        else if (strncmp(line_buf+1, NORMAL_NUM, strlen(NORMAL_NUM)) == 0){
            sscanf(line_buf+1+strlen(NORMAL_NUM), "%d", &normalN);
        }
    }

    if (faceN == 0 || vertexN == 0 || normalN == 0){
        fprintf(stderr, "missing face/vertex/normal count in OBJ file.\n");
        exit(0);
    }

    // *** vertex ref starts from 1 ***
    // *** vi, fi are array subscript, starts from 0 ***
    int vi = 0, fi = 0, ni = 0;
    static VertexRef *vrefPtr = new VertexRef[vertexN];
    GLfloat *vertex3fPtr = new GLfloat[3 * vertexN];
    GLfloat *normal3fPtr = new GLfloat[3 * normalN];
    GLint *face6iPtr = new GLint[6 * faceN];
    GLfloat *avgNormal3fPtr = new GLfloat[3 * vertexN];
    GLfloat *vptr = vertex3fPtr;
    GLfloat *nptr = normal3fPtr;
    GLint *fptr = face6iPtr;

    memset(vrefPtr, 0, sizeof(VertexRef)*vertexN);
    memset(avgNormal3fPtr, 0, sizeof(float)*3*vertexN);

    fprintf(stderr, "f:%d v:%d\n", faceN, vertexN);

    while (true){
        if (line_buf[0] == '#'){
            goto read_new_line;
        }
        if (line_buf[0] == 'v' && line_buf[1] == ' '){
            sscanf(line_buf+2, "%f%f%f", vptr, vptr+1, vptr+2);
            vrefPtr[vi].vid = vi+1;
            vrefPtr[vi].arrayLen = VERTEX_FACE_ARRAY_LEN;
            vrefPtr[vi].faces = new int[VERTEX_FACE_ARRAY_LEN];
            vptr += 3;
            vi++;
        }
        else if (line_buf[0] == 'v' && line_buf[1] == 'n'){
            sscanf(line_buf+2, "%f%f%f", nptr, nptr+1, nptr+2);
            nptr += 3;
            ni++;
        }
        else if (line_buf[0] == 'f' && line_buf[1] == ' '){
            sscanf(line_buf+2, "%d//%d %d//%d %d//%d", fptr, fptr+1, fptr+2, fptr+3, fptr+4, fptr+5);
            fptr[0]--;
            fptr[1]--;
            fptr[2]--;
            fptr[3]--;
            fptr[4]--;
            fptr[5]--;
            // normal vector of this triangle
            // GLfloat *normal = getNormal3Pts(vertex3fPtr+3*fptr[0],
            //     vertex3fPtr+3*fptr[1], vertex3fPtr+3*fptr[2]);
            for (int i = 0; i < 3; i++){
                VertexRef *vfp = vrefPtr + fptr[i*2];
                if (vfp->faceCnt == vfp->arrayLen){
                    int *newArray = new int[vfp->arrayLen + VERTEX_FACE_ARRAY_LEN];
                    memcpy(newArray, vfp->faces, vfp->arrayLen * sizeof(int));
                    delete vfp->faces;
                    vfp->arrayLen += VERTEX_FACE_ARRAY_LEN;
                    vfp->faces = newArray;
                }
                vfp->faces[vfp->faceCnt] = fi;
                vfp->faceCnt++;
                // add face normal to vertex normal
                GLfloat *avgnptr = avgNormal3fPtr + fptr[i*2]*3;
                GLfloat *nptr = normal3fPtr + fptr[i*2+1];
                avgnptr[0] += nptr[0];
                avgnptr[1] += nptr[1];
                avgnptr[2] += nptr[2];
            }
            fptr += 6;
            fi++;
        }
    read_new_line:
        if (fgets(line_buf, LINE_BUF_LEN, obj) == NULL){
            break;
        }
    }
    for (vi = 0; vi < vertexN; vi++){
        // calculate average vertex normal        
        avgNormal3fPtr[3*vi] /= vrefPtr[vi].faceCnt;
        avgNormal3fPtr[3*vi+1] /= vrefPtr[vi].faceCnt;
        avgNormal3fPtr[3*vi+2] /= vrefPtr[vi].faceCnt;
    }
    static Model model = {faceN, vertexN, normalN, scale, vertex3fPtr, 
        normal3fPtr, face6iPtr, vrefPtr, NULL, avgNormal3fPtr};
    return &model;
}

static void changeView(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*eyes at (eyeX, eyeY, eyeZ) looking at (0, 0, 0), up is (0, 0, 1)*/
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 视角, 长宽比, 近点, 远点
    gluPerspective(60.0, (GLfloat)width/height, 0.1, 15.0);
    changeView();
}

static void initGLUT(Model *model)
{    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    // glEnableClientState(GL_VERTEX_ARRAY);
    // glVertexPointer(3, GL_FLOAT, 0, model->vertex3fPtr);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
    glLightfv(GL_LIGHT0, GL_POSITION,lightPosition0);
    
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    reshape(initWidth, initHeight);
    lastTime = lastFPSTime = glutGet(GLUT_ELAPSED_TIME);
}


static bool testOnProfiles(GLfloat *v){
    return false;
}

static void drawSomething(Model *model){
    static GLfloat *vptr = model->vertex3fPtr;
    static GLfloat *vnptr = model->normal3fPtr;
    static GLint *fptr;
    static GLint a, b, c;
    GLfloat *eye3fv;
    eye3fv = windowCenterCoord();
    getVertexOnProfile(globalProfiles, globalModel, eye3fv);
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(4.0);
    glBegin(GL_POINTS);{
        for (set<int>::iterator it = globalProfiles.begin(); 
            it != globalProfiles.end(); it++){
            glVertex3fv(vptr+3*(*it));
        }
    }
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    {
        for (int i = 0; i < model->faceN; ++i){
            fptr = model->face6iPtr + 6*i;
            glNormal3fv(vnptr+3*fptr[1]);
            glVertex3fv(vptr+3*fptr[0]);

            glNormal3fv(vnptr+3*fptr[3]);
            glVertex3fv(vptr+3*fptr[2]);
            
            glNormal3fv(vnptr+3*fptr[5]);
            glVertex3fv(vptr+3*fptr[4]);
        }
    }
    glEnd();
    // fprintf(stderr, "points on profile %ld\n", globalProfiles.size());
}


static void drawAxis(){
    glLineWidth(2);
    glBegin(GL_LINES);
    {
        // z red 
        glColor3f(1.0, 0.0, 0.0);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 0, 1);
        // y green
        glColor3f(0.0, 1.0, 0.0);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 1, 0);
        // x blue
        glColor3f(0.0, 0.0, 1.0);
        glVertex3i(0, 0, 0);
        glVertex3i(1, 0, 0);
    }
    glEnd();
}


static void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    

    glPushMatrix();

    displayTransform();
    
    drawAxis();

    drawSomething(globalModel);

    //glutSolidTorus(0.4,1.0,20,20);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
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
    case 'l':
        glEnable(GL_LIGHTING);
        break;
    case 'L':
        glDisable(GL_LIGHTING);
        break;
    default: return;
    }
    changeView();
    glutPostRedisplay();
    // GLfloat *coord = windowCenterCoord();
    // fprintf(stderr, "center: %f %f %f\n", coord[0], coord[1], coord[2]);
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
    //writePNG(filenameCnt);
    frameCnt ++;
    filenameCnt ++;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastFPSTime >= FPS_SAMPLE_INTERVAL){
        frameRate = (GLfloat) frameCnt / (currentTime - lastFPSTime) * 1000.0;
        // printf("FPS: %.2f\n", frameRate);
        frameCnt = 0;
        lastFPSTime = currentTime;
    }
    GLint diff = currentTime - lastTime;
    lastTime = currentTime;
    glutPostRedisplay();
}

static void calcAvgVF()
{
    float avg = 0;
    for (int i = 0; i < globalModel->vertexN; i++){
        avg += globalModel->vrefPtr[i].faceCnt;
    }
    fprintf(stderr, "avg v/f:%f\n", avg/globalModel->vertexN);
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(initWidth, initHeight);
    glutInitWindowPosition(initPosX, initPosY);
    glutCreateWindow(argv[0]);
    globalModel = loadOBJModel("clean-test.obj");
    initGLUT(globalModel);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    initPNG();
    // glutHideWindow();
    glutMainLoop();
    return 0;
}