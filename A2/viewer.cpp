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

bool wireframe = false;

GLuint programSphere;
GLuint programCube; 

glm::mat4 projection;

GLuint objVAO[2];

GLuint tBuffer[2];

int triangles;

void init() {
    int vs;
    int fs;

    GLuint vbuffer[2];
    GLuint ibuffer[2];
    
    GLint vPositionSphere;
    GLint vNormalSphere;
    GLint vTexSphere;
    GLfloat *verticesSphere;
    GLfloat *normalsSphere;
    GLuint *indicesSphere;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    int nv;
    int nn;
    int ni;
    int i;

    int verts;
    int nt;
    GLfloat *texSphere;
    GLfloat theta;
    GLfloat phi;
    GLfloat x;
    GLfloat y;
    GLfloat z;


    vs = buildShader(GL_VERTEX_SHADER, "A2sphere.vs");
    fs = buildShader(GL_FRAGMENT_SHADER, "A2sphere.fs");
    programSphere = buildProgram(vs, fs, 0);
    glUseProgram(programSphere);

    glGenVertexArrays(1, &objVAO[0]);
    glBindVertexArray(objVAO[0]);

    std::string err = tinyobj::LoadObj(shapes, materials, "sphere.obj", 0);

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return;
    }

    nv = shapes[0].mesh.positions.size();
    verticesSphere = new GLfloat[nv];
    for (i = 0; i<nv; i++) {
        verticesSphere[i] = shapes[0].mesh.positions[i];
    }

    nn = shapes[0].mesh.normals.size();
    normalsSphere = new GLfloat[nn];
    for (i = 0; i<nn; i++) {
        normalsSphere[i] = shapes[0].mesh.normals[i];
    }

    ni = shapes[0].mesh.indices.size();
    triangles = ni / 3;
    indicesSphere = new GLuint[ni];
    for (i = 0; i<ni; i++) {
        indicesSphere[i] = shapes[0].mesh.indices[i];
    }

    Cube *textureSphere;
    textureSphere = loadCube("/home/melvincheng/Repositories/CSCI4110U/A2/VancouverConventionCentreScaled");

    
    glGenTextures(1, &tBuffer[0]);
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tBuffer[0]);

    for (i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, textureSphere->width, textureSphere->height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureSphere->data[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    verts = nv / 3;
    nt = 2 * verts;
    texSphere = new GLfloat[nt];
    for (i = 0; i < verts; i++) {
        x = verticesSphere[3 * i];
        y = verticesSphere[3 * i + 1];
        z = verticesSphere[3 * i + 2];

        theta = atan2(x, z);
        phi = atan2(y, sqrt(x*x + z*z));

        texSphere[2 * i] = fabs(theta) / (M_PI);
        texSphere[2 * i + 1] = phi / M_PI;
    }

    glGenBuffers(1, &vbuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, (nv + nn + nt)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), verticesSphere);
    glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normalsSphere);
    glBufferSubData(GL_ARRAY_BUFFER, (nv+nn)*sizeof(GLfloat), nt*sizeof(GLfloat), texSphere);
    
    glGenBuffers(1, &ibuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indicesSphere, GL_STATIC_DRAW);

    glUseProgram(programSphere);
    vPositionSphere = glGetAttribLocation(programSphere, "vPosition");
    glVertexAttribPointer(vPositionSphere, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionSphere);
    vNormalSphere = glGetAttribLocation(programSphere, "vNormal");
    glVertexAttribPointer(vNormalSphere, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(*verticesSphere)));
    glEnableVertexAttribArray(vNormalSphere);
    vTexSphere = glGetAttribLocation(programSphere, "vTex");
    glVertexAttribPointer(vTexSphere, 2, GL_FLOAT, GL_FALSE, 0, (void*)((nv + nn)*sizeof(*normalsSphere)));
    glEnableVertexAttribArray(vTexSphere);

    vs = buildShader(GL_VERTEX_SHADER, "A2cube.vs");
    fs = buildShader(GL_FRAGMENT_SHADER, "A2cube.fs");
    programCube = buildProgram(vs, fs, 0);
    glUseProgram(programCube);

    GLint vPositionCube;
    GLint vNormalCube;
    GLint vTexCube; 

    glGenVertexArrays(1, &objVAO[1]);
    glBindVertexArray(objVAO[1]);

    GLfloat verticesCube[8][4] = {
        { -5.0, -5.0, -5.0, 1.0 },      //0
        { -5.0, -5.0, 5.0, 1.0 },       //1
        { -5.0, 5.0, -5.0, 1.0 },       //2
        { -5.0, 5.0, 5.0, 1.0 },        //3

        { 5.0, -5.0, -5.0, 1.0 },       //4
        { 5.0, -5.0, 5.0, 1.0 },        //5
        { 5.0, 5.0, -5.0, 1.0 },        //6
        { 5.0, 5.0, 5.0, 1.0 }          //7
};

    // GLfloat normalsCube[8][3] = {
    //         { -1.0, -1.0, -1.0 },           //0
    //         { -1.0, -1.0, 1.0 },            //1
    //         { -1.0, 1.0, -1.0 },            //2
    //         { -1.0, 1.0, 1.0 },             //3
    //         { 1.0, -1.0, -1.0 },            //4
    //         { 1.0, -1.0, 1.0 },             //5
    //         { 1.0, 1.0, -1.0 },             //6
    //         { 1.0, 1.0, 1.0 }               //7
    // };

    GLuint indexesCube[36] = { 
        0, 1, 3, 0, 2, 3,
        0, 4, 5, 0, 1, 5,
        2, 6, 7, 2, 3, 7,
        0, 4, 6, 0, 2, 6,
        1, 5, 7, 1, 3, 7,
        4, 5, 7, 4, 6, 7 
    };

    Cube *textureCube;
    textureCube = loadCube("/home/melvincheng/Repositories/CSCI4110U/A2/VancouverConventionCentre");


    glGenTextures(1, &tBuffer[1]);
    // glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tBuffer[1]);

    for (i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, textureCube->width, textureCube->height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureCube->data[i]);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    // GLfloat texCube[8][3] = {
    //         { -1.0, -1.0, -1.0 },           //0
    //         { -1.0, -1.0, 1.0 },            //1
    //         { -1.0, 1.0, -1.0 },            //2
    //         { -1.0, 1.0, 1.0 },             //3
    //         { 1.0, -1.0, -1.0 },            //4
    //         { 1.0, -1.0, 1.0 },             //5
    //         { 1.0, 1.0, -1.0 },             //6
    //         { 1.0, 1.0, 1.0 }               //7
    //     };

    glGenBuffers(1, &vbuffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube) , NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesCube), verticesCube);
    // glBufferSubData(GL_ARRAY_BUFFER, sizeof(verticesCube), sizeof(normalsCube), normalsCube);
    // glBufferSubData(GL_ARRAY_BUFFER, sizeof(verticesCube) + sizeof(normalsCube), sizeof(texCube), texCube);

    glGenBuffers(1, &ibuffer[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexesCube), indexesCube, GL_STATIC_DRAW);

    glUseProgram(programCube);
    vPositionCube = glGetAttribLocation(programCube, "vPosition");
    glVertexAttribPointer(vPositionCube, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionCube);

    // vNormalCube = glGetAttribLocation(programCube, "vNormal");
    // glVertexAttribPointer(vNormalCube, 3, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(*verticesCube));
    // glEnableVertexAttribArray(vNormalCube);

    // vTexCube = glGetAttribLocation(programCube, "vTex");
    // glVertexAttribPointer(vTexCube, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(*verticesCube) + sizeof(*texCube)));
    // glEnableVertexAttribArray(vTexCube);
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
    int texLoc;

    if (wireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    view = glm::lookAt(glm::vec3(eyex, eyey, eyez), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));


    glUseProgram(programSphere);

    texLoc = glGetUniformLocation(programSphere, "tex");
    glUniform1i(texLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tBuffer[0]);

    viewLoc = glGetUniformLocation(programSphere, "modelView");
    glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
    projLoc = glGetUniformLocation(programSphere, "projection");
    glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

    colourLoc = glGetUniformLocation(programSphere, "colour");
    glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
    eyeLoc = glGetUniformLocation(programSphere, "Eye");
    glUniform3f(eyeLoc, eyex, eyey, eyez);
    lightLoc = glGetUniformLocation(programSphere, "light");
    glUniform3f(lightLoc, 1.0, 1.0, 1.0);
    materialLoc = glGetUniformLocation(programSphere, "material");
    glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);
    
    glBindVertexArray(objVAO[0]);
    glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);



    glUseProgram(programCube);

    texLoc = glGetUniformLocation(programCube, "tex");
    glUniform1i(texLoc, 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tBuffer[1]);

    viewLoc = glGetUniformLocation(programCube, "modelView");
    glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
    projLoc = glGetUniformLocation(programCube, "projection");
    glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

    colourLoc = glGetUniformLocation(programCube, "colour");
    glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
    eyeLoc = glGetUniformLocation(programCube, "Eye");
    glUniform3f(eyeLoc, eyex, eyey, eyez);
    lightLoc = glGetUniformLocation(programCube, "light");
    glUniform3f(lightLoc, 1.0, 1.0, 1.0);
    materialLoc = glGetUniformLocation(programCube, "material");
    glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);
    
    glBindVertexArray(objVAO[1]);
    glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, NULL);

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
    case 'p':
        if(wireframe) {
            wireframe = false;
        }else{
            wireframe = true;
        }
        break;
    }

    eyex = r*sin(theta)*cos(phi);
    eyey = r*sin(theta)*sin(phi);
    eyez = r*cos(theta);

    glutPostRedisplay();

}

int main(int argc, char **argv) {
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

    glutDisplayFunc(displayFunc);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboardFunc);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    init();

    eyex = 0.0;
    eyez = 0.0;
    eyey = 10.0;

    theta = 1.5;
    phi = 1.5;
    r = 5.0f;

    glutMainLoop();

}
