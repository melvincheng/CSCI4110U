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
std::normal_distribution<float> distribution(0.0,1.0);
std::default_random_engine generator;
double theta, phi;
double r;
bool wireframe = false;
GLuint program; 
float H;
uint x;
uint y;
uint nv;
glm::mat4 projection;
GLuint objVAO;
int triangles = 0;
glm::vec3 direction = glm::vec3(1.0f, 1.0f, -0.50f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 eye = glm::vec3(0.0f, 0.0f, 100.0f);

void diamond(glm::vec3 (*vertices), uint p1, uint p2, uint p3, uint p4) {
    // printf("%d %d %d %d\n", p1, p2, p3, p4);
    float mean = (vertices[p1].z + vertices[p2].z + vertices[p3].z + vertices[p4].z) / 4;

    float val = distribution(generator);

    uint middle = (p4 + p1) / 2;

    vertices[middle].z = mean + 2*val*H - H;
}
void square(glm::vec3 (*vertices), uint p1, uint p2, uint p3, uint p4) {
    // printf("%f %f %f %f\n", vertices[p1].z, vertices[p2].z, vertices[p3].z, vertices[p4].z);
    float mean;

    int outPoint;

    float val;

    uint middle = (p4 + p1) / 2;

    bool rightEdge = false;
    bool leftEdge = false;

    if ((((p2 + p4) / 2) + 1) % (x + 1) == 0){
        rightEdge = true;
    }
    if (((p1 + p3) / 2) % (x + 1) == 0){
        leftEdge = true;
    }

    outPoint = ((p2 + p1) / 2) - (x + 1);
    if (outPoint < 0) {
        mean = (vertices[p1].z + vertices[p2].z + vertices[middle].z) / 3;
        
    } else {
        mean = (vertices[p1].z + vertices[p2].z + vertices[middle].z + vertices[outPoint].z) / 4;
    }
    val = distribution(generator);
    vertices[(p2 + p1) / 2].z = mean + 2*val*H - H;


    outPoint = ((p1 + p3) / 2) - (middle - ((p1 + p3) / 2));
    if (leftEdge){
        mean = (vertices[p1].z + vertices[p3].z + vertices[middle].z) / 3;
    } else {
        mean = (vertices[p1].z + vertices[p3].z + vertices[middle].z + vertices[outPoint].z) / 4;
    }
    val = distribution(generator);
    vertices[(p3 + p1) / 2].z = mean + 2*val*H - H;

    outPoint = ((p2 + p4) / 2) + (((p2 + p4) / 2) - middle);
    if (rightEdge){
        mean = (vertices[p2].z + vertices[p4].z + vertices[middle].z) / 3;
    } else {
        mean = (vertices[p1].z + vertices[p4].z + vertices[middle].z + vertices[outPoint].z) / 4;
    }
    val = distribution(generator);
    vertices[(p4 + p2) / 2].z = mean + 2*val*H - H;

    outPoint = ((p3 + p4) / 2) + (x + 1);
    if (outPoint > nv) {
        mean = (vertices[p3].z + vertices[p4].z + vertices[middle].z) / 3;
    } else {
        mean = (vertices[p3].z + vertices[p4].z + vertices[middle].z + vertices[outPoint].z) / 4;
    }
    val = distribution(generator);
    vertices[(p4 + p3) / 2].z = mean + 2*val*H - H;
}
void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal;
    // GLfloat *vertices;
    // GLfloat *normals;
    GLuint *indices;
    uint ni;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);
 
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
    file >> H;
    uint *seeds = new uint[numx * numy];
    uint seednum = 0;
    // printf("%u %u\n", x, y);
    // printf("%d %d\n", numx,numy);

    if(numx == 2) {
        incx = x;
    }else{
        incx = floor(float(x+1)/float(numx - 1));
    }

    if(numy == 2) {
        incy = y;
    }else{
        incy = floor(float(y+1)/float(numy - 1));
        // printf("%\n");
        // std::cout<<round(float(y/numy))<<std::endl;
    }

    // printf("%i %i\n", incx, incy);

    nv = (x + 1) * (y + 1);
    glm::vec3(*vertices) = new glm::vec3[nv];
    
    float height;
    bool heightinc = true;
    
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
                    // while(height > 10) {
                    //     height = height/10;
                    // }
                    // printf("%f\n",2/3);
                    seeds[seednum++] = (x+1)*i+j;
                    // printf("%d %d %f\n", i/100, j/100, height);
                }else{
                    height = 0.0;
                }
            }
            // randomNum = rand() % 10;
            vertices[(x+1)*i+j] = glm::vec3(j,i,height);
        }
    }


    while(incx > 1 && incy > 1){
        for (int j = 0; j < y - 1; j += incy) {
            for (int i = 0; i < x - 1; i += incx) {
                diamond(vertices, j*(y+1) + i, j*(y+1) + i+incx, (j*(y+1))+(y+1)*incy + i, (j*(y+1))+(y+1)*incy + i+incx);
            }
        }
        for (int j = 0; j < y - 1; j += incy) {
            for (int i = 0; i < x - 1; i += incx) {
                square(vertices, j*(y+1) + i, j*(y+1) + i+incx, (j*(y+1))+(y+1)*incy + i, (j*(y+1))+(y+1)*incy + i+incx);
            }
        }
        H /= 2;
        incx /= 2;
        incy /= 2;
    }

    // for (int j = 0; j < numy-1; j++) {
    //     for (int i = 0; i < numx-1; i++) {
    //         diamond(vertices, seeds[j*numy + i], seeds[j*numy + i+1], seeds[(j*numy)+numy + i], seeds[(j*numy)+numy + i+1]);
    //     }
    // }

    // for (int j = 0; j < numy-1; j++) {
    //     for (int i = 0; i < numx-1; i++) {
    //         square(vertices, seeds[j*numy + i], seeds[j*numy + i+1], seeds[(j*numy)+numy + i], seeds[(j*numy)+numy + i+1]);
    //     }
    // }

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

    if (wireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    view = glm::lookAt(eye, eye + direction, up);

    viewLoc = glGetUniformLocation(program, "modelView");
    glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
    projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

    colourLoc = glGetUniformLocation(program, "colour");
    glUniform4f(colourLoc, 0.0, 1.0, 0.0, 1.0);
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
        eye += 1.0f * glm::cross(direction, up);
        break;
    case 'q':
        eye -= 1.0f * glm::cross(direction, up);
        break;
    case 'r':
        eye += 1.0f * up;
        break;
    case 'f':
        eye -= 1.0f * up;
        break;
    case 'x':
        eye += 1.0f * direction;
        break;
    case 'c':
        eye -= 1.0f * direction;
        break;
    case 'p':
        if(wireframe) {
            wireframe = false;
        }else{
            wireframe = true;
        }
        break;
    }


    // printf("%s %f %f %f\n", "Eye:",eye.x,eye.y,eye.z);
    // printf("%s %f %f %f\n", "Direction",direction.x,direction.y,direction.z);
    // printf("%s %f %f %f\n", "up",up.x,up.y,up.z);
    // eyex = r*sin(theta)*cos(phi);
    // eyey = r*sin(theta)*sin(phi);
    // eyez = r*cos(theta);

    glutPostRedisplay();

}

int main(int argc, char **argv) {
    int fs;
    int vs;

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
    glClearColor(0.53, 0.81, 0.98, 1.0);

    
    vs = buildShader(GL_VERTEX_SHADER, "a1.vs");
    
    fs = buildShader(GL_FRAGMENT_SHADER, "a1.fs");
    program = buildProgram(vs, fs, 0);
    dumpProgram(program, "A1 shader program");
    init();

    glutMainLoop();

}
