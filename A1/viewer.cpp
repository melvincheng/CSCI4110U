// #define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shaders.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <math.h>

#include <stdlib.h>
#include <time.h>
    

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles = 0;
glm::vec3 direction = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 eye = glm::vec3(02.5f, 02.5f, 1.0f);

void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal;
    // GLfloat *vertices;
    // GLfloat *normals;
    GLuint *indices;
    uint nn;
    uint ni;
    uint nv;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    uint x;
    uint y; 
    uint numx;
    uint numy;
    uint incx;
    uint incy;
    std::ifstream file;
    file.open("terrainInfo.txt");
    file >> x;
    file >> y;
    file >> numx;
    file >> numy;
    printf("%u %u\n", x, y);
    printf("%d %d\n", numx,numy);
    if(numx == 2) {
        incx = x;
    }else{
        incx = round(float(x+1)/float(numx));
    }

    if(numy == 2) {
        incy = y;
    }else{
        incy = round(float(y+1)/float(numy));
        // printf("%\n");
        std::cout<<round(float(y/numy))<<std::endl;
    }
    printf("%i %i\n", incx, incy);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    nv = (x + 1) * (y + 1);
    glm::vec3(*vertices) = new glm::vec3[nv];
    int randomNum;
    float height;
    bool heightinc = true;
    srand (time(NULL));
    for (int i = 0; i <= y; i++) {
        height = 0.0;
        if(i%incy == 0){
            heightinc = true;
        }else{
            heightinc = false;
        }
        for (int j = 0; j <= x; j++) {
            if(heightinc){
                if(j%incx == 0){
                    file >> height;
                    // printf("%f\n",2/3);
                    printf("%d %d %f\n", i, j, height);
                }else{
                    height = 0.0;
                }
            }
            // randomNum = rand() % 10;
            vertices[(x+1)*i+j] = glm::vec3(j,i,height);
        }
    }

    // for (int i = 0; i < nv; i++) {
    //     printf("%f %f %f\n", vertices[i].x,vertices[i].y,vertices[i].z);
    // }

    ni = (x) * (y) * 2 * 3;
    indices = new GLuint[ni]; 
    // printf("%s\n", "begin");

    int k = 0;
    int l = 0;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) { 
            indices[l++] = x*i + j + k;
            indices[l++] = x*i + j + x+1 + 1 + k;
            indices[l++] = x*i + j + 1 + k;
        
            indices[l++] = x*i + j + k;
            indices[l++] = x*i + j + x+1 + k;
            indices[l++] = x*i + j + x+1 + 1 + k;

            // printf("%u %u %u\n", 
            //     x*i + j + k,
            //     x*i + j + x + 1 + k,
            //     x*i + j + 1 + k);
            triangles+=2;
        }
        k++;
    }


    glm::vec3 (*facenormals) = new glm::vec3[triangles];

    GLuint temp1, temp2, temp3;
    glm::vec3 tempvec1, tempvec2, finalvec;

    for (int i = 0; i < triangles; i++){
        temp1 = indices[i * 3 + 0];
        temp2 = indices[i * 3 + 1];
        temp3 = indices[i * 3 + 2];
        tempvec1 = glm::vec3(
            vertices[temp1].x - vertices[temp3].x,
            vertices[temp1].y - vertices[temp3].y,
            vertices[temp1].z - vertices[temp3].z);
        tempvec2 = glm::vec3(
            vertices[temp2].x - vertices[temp3].x,
            vertices[temp2].y - vertices[temp3].y,
            vertices[temp2].z - vertices[temp3].z);
        finalvec = glm::cross(tempvec2, tempvec1);
        facenormals[i].x = finalvec.x;
        facenormals[i].y = finalvec.y;
        facenormals[i].z = finalvec.z;
    }

    GLfloat(*verNormals)[4] = new GLfloat[nv][4];
    int tempCount;

    for (int i = 0; i < nv; i++){
    for (int j = 0; j < 4; j++){
            verNormals[i][j] = 0.0;
        }
    }

    for (int i = 0; i < triangles; i++){
        for (int j = 0; j < 3; j++){
            verNormals[indices[i * 3 + j]][3]++;
            for (int k = 0; k < 3; k++){
                verNormals[indices[i * 3 + j]][k] = verNormals[indices[i * 3 + j]][k] + facenormals[i][k];
            }
        }
    }

    glm::vec3(*normals) = new glm::vec3[nv];
    for (int i = 0; i < nv; i++){
        for (int j = 0; j < 3; j++){
            normals[i][j] = verNormals[i][j] / verNormals[i][3];
        }
    }

    // printf("%u\n", nv);
    // printf("%d\n", triangles);
    // printf("%u\n", ni);

    // for (int i = 0; i < triangles*3; i++) {
    //     printf("%d \n",indices[i]);
    //     printf("%f %f %f\n", vertices[indices[i]].x,vertices[indices[i]].y,vertices[indices[i]].z);
    // }


    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, (nv)*sizeof(glm::vec3)*2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(glm::vec3), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(glm::vec3), nv*sizeof(glm::vec3), normals);

    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*ni, indices, GL_STATIC_DRAW);

    glUseProgram(program);
    vPosition = glGetAttribLocation(program, "vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);
    vNormal = glGetAttribLocation(program, "vNormal");
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(*vertices)));
    glEnableVertexAttribArray(vNormal);
}


void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).

    if (h == 0)
        h = 1;

    float ratio = 1.0 * w / h;

    glViewport(0, 0, w, h);

    projection = glm::perspective(45.0f, ratio, 1.0f, 10000.0f);

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

    view = glm::lookAt(eye, eye + direction, up);

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
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, NULL);
    glPointSize(3);
    glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y) {

    switch (key) {
    case 'a':
        direction = glm::mat3(glm::rotate(1.0f,up)) * direction;
        break;
    case 'd':
        direction = glm::mat3(glm::rotate(-1.0f,up)) * direction;
        break;
    case 'w':
        direction = glm::mat3(glm::rotate(1.0f,glm::cross(up,direction))) * direction;
        break;
    case 's':
        direction = glm::mat3(glm::rotate(-1.0f,glm::cross(up,direction))) * direction;
        break;
    case 'e':
        eye += 0.1f * glm::cross(direction, up);
        break;
    case 'q':
        eye -= 0.1f * glm::cross(direction, up);
        break;
    case 'r':
        eye += 0.1f * up;
        break;
    case 'f':
        eye -= 0.1f * up;
        break;
    case 'x':
        eye += 0.1f * direction;
        break;
    case 'c':
        eye -= 0.1f * direction;
        break;
    }

    printf("%s %f %f %f\n", "Eye:",eye.x,eye.y,eye.z);
    printf("%s %f %f %f\n", "Direction",direction.x,direction.y,direction.z);
    printf("%s %f %f %f\n", "up",up.x,up.y,up.z);
    // eyex = r*sin(theta)*cos(phi);
    // eyey = r*sin(theta)*sin(phi);
    // eyez = r*cos(theta);

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

    glutMainLoop();

}
