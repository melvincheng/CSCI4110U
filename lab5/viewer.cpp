    /************************************************************
*                   CSCI 4110 Lab 2
*
*  Basic OpenGL program that shows how to set up a
*  VAO and some basic shaders.  This program draws
*  a cube or sphere depending upon whether CUBE or
*  SPHERE is defined.
*
*/
//#include <Windows.h>
#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdio.h>
#include "Shaders.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include "texture.h"
    
//#define CUBE
#define SPHERE

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles;

#ifdef CUBE
/*
 * This version of the init procedure produces the
 * data for drawing a cube.  The vertex and index
 * data are stored in constant arrays which are copied
 * into the buffers.  This code should be familiar
 * from class.
 */
void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    GLfloat vertices[8][4] = {
            { -1.0, -1.0, -1.0, 1.0 },      //0
            { -1.0, -1.0, 1.0, 1.0 },       //1
            { -1.0, 1.0, -1.0, 1.0 },       //2
            { -1.0, 1.0, 1.0, 1.0 },        //3
            { 1.0, -1.0, -1.0, 1.0 },       //4
            { 1.0, -1.0, 1.0, 1.0 },        //5
            { 1.0, 1.0, -1.0, 1.0 },        //6
            { 1.0, 1.0, 1.0, 1.0 }          //7
    };

    GLfloat normals[8][3] = {
            { -1.0, -1.0, -1.0 },           //0
            { -1.0, -1.0, 1.0 },            //1
            { -1.0, 1.0, -1.0 },            //2
            { -1.0, 1.0, 1.0 },             //3
            { 1.0, -1.0, -1.0 },            //4
            { 1.0, -1.0, 1.0 },             //5
            { 1.0, 1.0, -1.0 },             //6
            { 1.0, 1.0, 1.0 }               //7
    };

    GLuint indexes[36] = { 0, 1, 3, 0, 2, 3,
        0, 4, 5, 0, 1, 5,
        2, 6, 7, 2, 3, 7,
        0, 4, 6, 0, 2, 6,
        1, 5, 7, 1, 3, 7,
        4, 5, 7, 4, 6, 7 };

    triangles = 12;

    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glUseProgram(program);
    vPosition = glGetAttribLocation(program, "vPosition");
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);
    vNormal = glGetAttribLocation(program, "vNormal");
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(*vertices));
    glEnableVertexAttribArray(vNormal);

}
#endif

#ifdef SPHERE
void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLuint tBuffer;
    GLint vPosition;
    GLint vNormal;
    GLint vTex;
    GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    int nv;
    int nn;
    int ni;
    int i;

    int verts;
    int nt;
    GLfloat *tex;
    GLfloat theta;
    GLfloat phi;
    GLfloat x;
    GLfloat y;
    GLfloat z;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    /*  Load the obj file */

    std::string err = tinyobj::LoadObj(shapes, materials, "sphere.obj", 0);

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return;
    }

    /*  Retrieve the vertex coordinate data */

    nv = shapes[0].mesh.positions.size();
    vertices = new GLfloat[nv];
    for (i = 0; i<nv; i++) {
        vertices[i] = shapes[0].mesh.positions[i];
    }

    /*  Retrieve the vertex normals */

    nn = shapes[0].mesh.normals.size();
    normals = new GLfloat[nn];
    for (i = 0; i<nn; i++) {
        normals[i] = shapes[0].mesh.normals[i];
    }

    /*  Retrieve the triangle indices */

    ni = shapes[0].mesh.indices.size();
    triangles = ni / 3;
    indices = new GLuint[ni];
    for (i = 0; i<ni; i++) {
        indices[i] = shapes[0].mesh.indices[i];
    }

    // Texture *texture;
    // texture = loadTexture("checkerboard.jpg");
    // glGenTextures(1, &tBuffer);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, tBuffer);
    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texture-> width, texture->height);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    Cube *texture;
    texture = loadCube("/home/melvincheng/Repositories/CSCI4110U/lab5/VancouverConventionCentre");
    glGenTextures(1, &tBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tBuffer);
    for (i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    verts = nv / 3;
    nt = 2 * verts;
    tex = new GLfloat[nt];
    for (i = 0; i < verts; i++) {
        x = vertices[3 * i];
        y = vertices[3 * i + 1];
        z = vertices[3 * i + 2];

        theta = atan2(x, z);
        phi = atan2(y, sqrt(x*x + z*z));

        tex[2 * i] = fabs(theta) / (M_PI);
        tex[2 * i + 1] = phi / M_PI;
    }

    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, (nv + nn + nt)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);
    glBufferSubData(GL_ARRAY_BUFFER, (nv+nn)*sizeof(GLfloat), nt*sizeof(GLfloat), tex);
    // /*
    // *  load the vertex coordinate data
    // */
    // glGenBuffers(1, &vbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    // glBufferData(GL_ARRAY_BUFFER, (nv + nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
    // glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

    // /*
    // *  load the vertex indexes
    // */
    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

    // /*
    // *  link the vertex coordinates to the vPosition
    // *  variable in the vertex program.  Do the same
    // *  for the normal vectors.
    // */
    glUseProgram(program);
    vPosition = glGetAttribLocation(program, "vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);
    vNormal = glGetAttribLocation(program, "vNormal");
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(*vertices)));
    glEnableVertexAttribArray(vNormal);
    vTex = glGetAttribLocation(program, "vTex");
    glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, (void*)((nv + nn)*sizeof(*tex)));
    glEnableVertexAttribArray(vTex);

}
#endif


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
    glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);

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

    if (argc < 3) {
        printf("usage: viwer vertex_suffix fragment_suffix\n");
        exit(0);
    }
    vertex = argv[1];
    fragment = argv[2];

    glutDisplayFunc(displayFunc);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboardFunc);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    sprintf(vertexName, "lab5%s.vs", vertex);
    vs = buildShader(GL_VERTEX_SHADER, vertexName);
    sprintf(fragmentName, "lab5%s.fs", fragment);
    fs = buildShader(GL_FRAGMENT_SHADER, fragmentName);
    program = buildProgram(vs, fs, 0);
    dumpProgram(program, "Lab 2 shader program");
    init();

    eyex = 0.0;
    eyez = 0.0;
    eyey = 10.0;

    theta = 1.5;
    phi = 1.5;
    r = 10.0;

    glutMainLoop();

}
