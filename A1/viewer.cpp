#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shaders.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
    

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles;
int ni;

void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal;
    // GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;
    int nv;
    int nn;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    uint x;
    uint y;
    std::ifstream file;
    file.open("terrainInfo.txt");
    file >> x;
    file >> y;
    printf("%u %u\n", x, y);

    nv = x * y;
    glm::vec3(*vertices) = new glm::vec3[nv];

    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; i++) {
            vertices[y*i+j].x = i;
            vertices[y*i+j].y = j;
            vertices[y*i+j].z = 0;

        }
    }

    ni = x * y;
    indices = new GLuint[ni];
    for (int i = 0; i < 0; i++) {
        indices[i] = i;
    }



    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, (nv /*+ nn*/)*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(glm::vec3), vertices);
    // glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ni, indices, GL_STATIC_DRAW);

    glUseProgram(program);
    vPosition = glGetAttribLocation(program, "vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);
    // vNormal = glGetAttribLocation(program, "vNormal");
    // glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(*vertices)));
    // glEnableVertexAttribArray(vNormal);
}


void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).

    if (h == 0)
        h = 1;

    float ratio = 1.0 * w / h;

    glViewport(0, 0, w, h);

    projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);

}

void displayFunc(void) {
    glm::mat4 view;
    int viewLoc;
    int projLoc;
    int colourLoc;
    int eyeLoc;
    int lightLoc;
    int materialLoc;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    view = glm::lookAt(glm::vec3(eyex, eyey, eyez), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    viewLoc = glGetUniformLocation(program, "modelView");
    glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
    projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

    colourLoc = glGetUniformLocation(program, "colour");
    glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
    eyeLoc = glGetUniformLocation(program, "Eye");
    glUniform3f(eyeLoc, eyex, eyey, eyez);
    lightLoc = glGetUniformLocation(program, "light");
    glUniform3f(lightLoc, 1.0, 1.0, 1.0);
    materialLoc = glGetUniformLocation(program, "material");
    glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);
    
    glBindVertexArray(objVAO);
    glDrawElements(GL_POINTS, ni, GL_UNSIGNED_INT, NULL);

    glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y) {

    switch (key) {
    case 'a':
        phi -= 0.1;
        break;
    case 'd':
        phi += 0.1;
        break;
    case 'w':
        theta += 0.1;
        break;
    case 's':
        theta -= 0.1;
        break;
    }

    eyex = r*sin(theta)*cos(phi);
    eyey = r*sin(theta)*sin(phi);
    eyez = r*cos(theta);

    glutPostRedisplay();

}

int main(int argc, char **argv) {
    int fs;
    int vs;
    int user;

    char vertexName[256];
    char fragmentName[256];
    char *fragment;
    char *vertex;

    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);


    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("Viewer");

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        printf("Error starting GLEW: %s\n", glewGetErrorString(error));
        exit(0);
    }

    // if (argc < 3) {
    //     printf("usage: viwer vertex_suffix fragment_suffix\n");
    //     exit(0);
    // }
    // vertex = argv[1];
    // fragment = argv[2];

    glutDisplayFunc(displayFunc);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboardFunc);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    
    vs = buildShader(GL_VERTEX_SHADER, "lab4a.vs");
    
    fs = buildShader(GL_FRAGMENT_SHADER, "lab4a.fs");
    program = buildProgram(vs, fs, 0);
    dumpProgram(program, "A1 shader program");
    init();

    eyex = 0.0;
    eyez = 0.0;
    eyey = 10.0;

    theta = 1.5;
    phi = 1.5;
    r = 10.0;

    glutMainLoop();

}
