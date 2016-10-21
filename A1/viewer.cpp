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
int triangles = 0;

void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vNormal
;    // GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;
    uint nn;
    uint ni;
    uint nv;

    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    uint x;
    uint y;
    std::ifstream file;
    file.open("terrainInfo.txt");
    file >> x;
    file >> y;
    printf("%u %u\n", x, y);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    nv = (x + 1) * (y + 1);
    glm::vec3(*vertices) = new glm::vec3[nv];

    for (uint i = 0; i <= y; i++) {
        for (uint j = 0; j <= x; j++) {
            vertices[(x+1)*i+j].x = float(j);
            vertices[(x+1)*i+j].y = float(i);
            vertices[(x+1)*i+j].z = float(0.0);
            printf("%i\n",(x+1)*i+j);
            printf("%f %f %f\n", float(i),float(j),float(0.0));
        }
    }

    ni = (x) * (y) * 2 * 3;
    indices = new GLuint[ni];
    int run = 0;
    for (uint i = 0; i < y; i++) {
        for (uint j = 0; j < x; j++) {
            indices[(i*(x-1) + j)*6] = i*(x+1) + j;
            indices[(i*(x-1) + j)*6 + 1] = i*(x+1) + j + x+1 + 1;
            indices[(i*(x-1) + j)*6 + 2] = i*(x+1) + j + 1;

            printf("%u %u %u \n", i*(x+1) + j,i*(x+1) + j + x+1 + 1,i*(x+1) + j + 1);


            indices[(i*(x-1) + j)*6 + 3] = i*(x+1) + j;
            indices[(i*(x-1) + j)*6 + 4] = j + (i+1)*(x+1);
            indices[(i*(x-1) + j)*6 + 5] = j + (i+1)*(x+1) + 1;
            printf("%u %u %u \n\n", i*(x+1) + j,j + (i+1)*(x+1),j + (i+1)*(x+1) + 1);
            triangles += 2;
        }
    }
    printf("%u\n", nv);
    printf("%d\n", triangles);
    printf("%u\n", ni);

    // inc = 0;
    // for (uint i = ni/2; i < ni; i += 3) {
    //     if (inc%(x - 1) == 0 && inc != 0) {
    //         inc++;
    //     } 
    //     indices[i] = inc;
    //     indices[i + 1] = inc + x;
    //     indices[i + 2] = inc + x + 1;
    //     inc++;
    // }

    for (int i = 0; i < 126; i+=3) {
        
        printf("%f %f %f\n",vertices[indices[i]].x,vertices[indices[i]].y,vertices[indices[i]].z);
        printf("%f %f %f\n",vertices[indices[i+1]].x,vertices[indices[i+1]].y,vertices[indices[i+1]].z);
        printf("%f %f %f\n",vertices[indices[i+2]].x,vertices[indices[i+2]].y,vertices[indices[i+2]].z);
        printf("%d\n",i);
    }


    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, (nv /*+ nn*/)*sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(glm::vec3), vertices);
    // glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*ni, indices, GL_STATIC_DRAW);

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
    glDrawElements(GL_TRIANGLES, triangles*4, GL_UNSIGNED_INT, NULL);
    // glPointSize(3);

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
    printf("%f %f %f \n", eyex, eyey, eyez);

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
    eyez = -10.0;
    eyey = 0.0;

    theta = 1.5;
    phi = 1.5;
    r = 15.0;

    glutMainLoop();

}
