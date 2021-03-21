/**
 * Trabalho Computação Gráfica
 * Universidade Federal de Lavras
 * 
 * Autor: Vitor Oriel de Castro Nunes Borges
 *        Renato Figueiredo Pereira
 * 
 * Turma: 10A
 */

#include <GL/glut.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*	Create checkerboard texture	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

#ifdef GL_VERSION_1_1
static GLuint texName;
#endif

static bool lightOn = false;
static int rotateZaxis = 0, rotateXaxis = 0;

struct Vertex {
    GLfloat x, y, z;

    Vertex() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    Vertex(GLfloat x, GLfloat y, GLfloat z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

class ComponentInterface {
    public:
        virtual ~ComponentInterface() {}
        virtual void draw() = 0;
};

class Propeller : ComponentInterface {
    friend class MainSupport;
    public:
        Propeller();
        void setPropeller(Vertex position, GLfloat rotate);
        virtual void draw();
    private:
        Vertex position;
        GLfloat rotate;
        Vertex scaleBody;
};

Propeller::Propeller() {
    this->position = Vertex();
    this->rotate = 0.0f;
    this->scaleBody = Vertex(3.2f, 0.5f, 0.1f);
}

void Propeller::setPropeller(Vertex position, GLfloat rotate) {
    this->position = position;
    this->rotate = rotate;
}

void Propeller::draw() {
    Vertex pPos = this->position;
    Vertex pBody = this->scaleBody;
    glPushMatrix();
        int angle = static_cast<int>(this->rotate);
        GLfloat xAngle, yAngle;
        switch(angle) {
            case 0:
                xAngle = 1.0f;
                yAngle = 0.0f;
                break;
            case 90:
                xAngle = 0.0f;
                yAngle = 1.0f;
                break;
            case 180:
                xAngle = -1.0f;
                yAngle = 0.0f;
                break;
            case 270:
                xAngle = 0.0f;
                yAngle = -1.0f;
                break;
        }
        glColor3f(0.2f, 0.05f, 0.0f);
        glRotatef(10.0f, xAngle, yAngle, 0.0f);
        glRotatef(this->rotate, 0.0f, 0.0f, 1.0f);
        glTranslatef(pPos.x, pPos.y, pPos.z);
        glScalef(pBody.x, pBody.y, pBody.z);
        glutSolidCube(0.5);
        glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

class Circle : ComponentInterface {
    friend class MainSupport;
    public:
        Circle();
        void setPosition(Vertex position);
        virtual void draw();
    private:
        Vertex position;
        GLdouble base;
        GLdouble height;
        GLint slices;
        GLint stacks;
};

Circle::Circle() {
    this->position = Vertex(0.0f, 0.0f, 0.0f);
    this->base = 0.5;
    this->height = 0;
    this->slices = 35;
    this->stacks = 35;
}

void Circle::setPosition(Vertex position) {
    this->position = position;
}

void Circle::draw() {
    Vertex p = this->position;
    glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glutSolidCone(this->base, this->height, this->slices, this->stacks);
    glPopMatrix();
}

class Cylinder : ComponentInterface {
    friend class MainSupport;
    friend class UpperSupport;
    public:
        Cylinder(GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks);
        GLdouble getHeight();
        GLdouble getRadius();
        void setPosition(Vertex position);
        virtual void draw();
    private:
        Vertex position;
        GLdouble baseRadius;
        GLdouble topRadius;
        GLdouble height;
        GLint slices;
        GLint stacks;
};

Cylinder::Cylinder(GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks) {
    this->position = Vertex();
    this->baseRadius = baseRadius;
    this->topRadius = topRadius;
    this->height = height;
    this->slices = slices;
    this->stacks = stacks;
}

GLdouble Cylinder::getHeight() {
    return this->height;
}

GLdouble Cylinder::getRadius() {
    return this->baseRadius;
}

void Cylinder::setPosition(Vertex position) {
    this->position = position;
}

void Cylinder::draw() {
    Vertex p = this->position;
    glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        gluCylinder(reinterpret_cast<GLUquadricObj*>(gluNewQuadric()), this->baseRadius, this->topRadius, this->height, this->slices, this->stacks);
    glPopMatrix();
}

class MainSupport : ComponentInterface {
    public:
        MainSupport();
        ~MainSupport();
        Cylinder* getBody();
        virtual void draw();
    private:
        void defineBases();
        void definePropellers();
        Vertex position;
        Cylinder* body;
        Circle* bases;
        Propeller* propellers;
};

MainSupport::MainSupport() {
    this->position = Vertex(0.0f, 0.0f, 0.0f);
    this->body = new Cylinder(0.5f, 0.5f, 0.7f, 32, 1);
    this->defineBases();
    this->definePropellers();
}

MainSupport::~MainSupport() {
    delete this->body;
    delete[] this->bases;
    delete[] this->propellers;
}

Cylinder* MainSupport::getBody() {
    return this->body;
}

void MainSupport::draw() {
    Vertex p = this->position;
    glRotatef(static_cast<GLfloat>(rotateZaxis), 0.0f, 0.0f, 1.0f);
    glTranslatef(p.x, p.y, p.z);
    glColor3f(0.5f, 0.3f, 0.0f);
    this->body->draw();
    glPushMatrix();
        for (int i = 0; i < 2; ++i) {
            if (i%2 != 0)
                glColor3f(1.0f, 1.0f, 1.0f);
            else
                glColor3f(0.5f, 0.3f, 0.0f);
            this->bases[i].draw();
        }
        for (int i = 0; i < 4; ++i)
            this->propellers[i].draw();
    glPopMatrix();
}

void MainSupport::defineBases() {
    Vertex p = this->position;
    this->bases = new Circle[2];
    Vertex basePos = Vertex(0.0f, 0.0f, (p.z + static_cast<GLfloat>(this->body->getHeight())));
    this->bases[1].setPosition(basePos);
}

void MainSupport::definePropellers() {
    Vertex p = this->position;
    this->propellers = new Propeller[4];
    GLfloat height = static_cast<GLfloat>(this->body->getHeight());
    GLfloat radius = static_cast<GLfloat>(this->body->getRadius()); // Ajusted position for the propeller, to be on the corner of the MainSupport.
    Vertex ajustedPosition = Vertex((p.x + radius*2.5f), p.y, (p.z + height/2.0f));
    for (int i = 0; i < 4; ++i)
        this->propellers[i].setPropeller(ajustedPosition, static_cast<GLfloat>(i)*90.0f);
}

class UpperSupport : ComponentInterface {
    public:
        UpperSupport();
        ~UpperSupport();
        virtual void draw();
    private:
        Cylinder* cable;
        Cylinder* base;
};

UpperSupport::UpperSupport() {
    this->base = new Cylinder(0.2f, 0.05f, 0.4f, 32, 1);
    this->cable = new Cylinder(0.05f, 0.05f, 0.9f, 32, 1);
    Vertex position = Vertex(0.0f, 0.0f, 0.5f-static_cast<GLfloat>(this->cable->getHeight()));
    this->base->setPosition(position);
}

UpperSupport::~UpperSupport() {
    delete this->cable;
    delete this->base;
}

void UpperSupport::draw() {
    glPushMatrix();
        glColor3f(0.2f, 0.05f, 0.0f);
        this->base->draw();
        glColor3f(0.5f, 0.3f, 0.0f);
        this->cable->draw();
    glPopMatrix();
}

class CeilingFan : ComponentInterface {
    friend class UpperSupport;
    friend class MainSupport;
    public:
        CeilingFan();
        ~CeilingFan();
        virtual void draw();
    private:
        UpperSupport* upperSupport;
        MainSupport* mainSupport;
};

CeilingFan::CeilingFan() {
    this->upperSupport = new UpperSupport();
    this->mainSupport = new MainSupport();
}

CeilingFan::~CeilingFan() {
    delete this->upperSupport;
    delete this->mainSupport;
}

void CeilingFan::draw() {
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, static_cast<GLfloat>(-0.2f-(this->mainSupport->getBody()->getHeight())));
        this->upperSupport->draw();
    glPopMatrix();
    glPushMatrix();
        this->mainSupport->draw();
    glPopMatrix();
}

void setIlumination() {
    // Light parameters
    float ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float diffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
    float specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float positionLight[] = {1.0f, 1.0f, -15.0f, 1.0f};

    //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLight);
    //glMaterialfv(GL_FRONT, GL_AMBIENT, ambientLight);

    // Active the ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    // Set the light parameters
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, positionLight);
}

void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    setIlumination();
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glutSwapBuffers();

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);
#endif

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   int width, height, nrChannels;
   unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
   if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      //glGenerateMipmap(GL_TEXTURE_2D);
   }
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    CeilingFan* ceilingFan = new CeilingFan();
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glRotatef(static_cast<GLfloat>(rotateXaxis), 1.0f, 0.0f, 0.0f);
        ceilingFan->draw();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    #ifdef GL_VERSION_1_1
    glBindTexture(GL_TEXTURE_2D, texName);
    #endif

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-2.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-2.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.0, -1.0, 0.0);

    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(2.41421, 1.0, -1.41421);
    glTexCoord2f(1.0, 0.0); glVertex3f(2.41421, -1.0, -1.41421);
    glEnd();
    glFlush();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutSwapBuffers();
    delete ceilingFan;
}


void reshape(int w, int h) {
    glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h)); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, static_cast<GLfloat>(w)/static_cast<GLfloat>(h), 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            rotateZaxis = (rotateZaxis + 5) % 360;
            glutPostRedisplay();
            break;
        case 's':
            rotateZaxis = (rotateZaxis - 5) % 360;
            glutPostRedisplay();
            break;
        case 'A':
            rotateZaxis = (rotateZaxis + 10) % 360;
            glutPostRedisplay();
            break;
        case 'S':
            rotateZaxis = (rotateZaxis - 10) % 360;
            glutPostRedisplay();
            break;
        case 'o':
        case 'O':
            if (!lightOn) {
                lightOn = true;
                glEnable(GL_LIGHTING);
            } else {
                lightOn = false;
                glDisable(GL_LIGHTING);
            }
            glutPostRedisplay();
            break;
        default:
            break;
    }
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    switch(button) {
        case GLUT_LEFT_BUTTON:
            rotateXaxis = (rotateXaxis + 5) % 360;
            glutPostRedisplay();
            break;
        case GLUT_RIGHT_BUTTON:
            rotateXaxis = (rotateXaxis - 5) % 360;
            glutPostRedisplay();
            break;
    }
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ceiling Fan");
    init();
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}

