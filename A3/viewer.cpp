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
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdio.h>
#include "Shaders.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <random>
#include <time.h>

#define SPHERE

struct boid{

};

struct Monkey{
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	glm::mat4 model;
	int triangles;
	int nv;
	int nn;
	int ni;
	float velocity;
	float acceleration;
	float x;
	float z;
	float start;
	glm::vec3 goal;
	glm::vec3 position;
	glm::vec4 colour;
	GLuint objVAO;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};	

float eyex, eyey, eyez;
float startR = -15.0;
float startB = 15.0; 

double theta, phi;
double r;
uint x;
uint y;
int end = 0;

glm::vec3 eye = glm::vec3(0.0f, 30.0f, 0.0f);
glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles;

Monkey monkeys[10];

#ifdef SPHERE
void init() {
	int i;
	float posR = -15.0f;
	float posB = -15.0f;
	for(int a = 0; a < 10; a++){

		monkeys[a].acceleration = 0.0;
		monkeys[a].velocity = 0.0;

		if(a > 4){
			posR = rand() % 30 - 15;
			monkeys[a].colour = glm::vec4(1.0,0.0,0.0,1.0);
			monkeys[a].model = glm::translate(glm::mat4(), glm::vec3(15.0f,0.0f,posR));
			glm::vec3 position = glm::vec3(15.0f,0.0f,posR);
			monkeys[a].goal = glm::vec3(-15.0,0.0,0.0);
			monkeys[a].start = posR;
			posR += 7.5;
		} else {
			posB = rand() % 30 - 15;
			monkeys[a].colour = glm::vec4(0.0,0.0,1.0,1.0);
			monkeys[a].model = glm::translate(glm::mat4(), glm::vec3(-15.0f,0.0f,posB));
			glm::vec3 position = glm::vec3(-15.0f,0.0f,posB);
			monkeys[a].goal = glm::vec3(15.0,0.0,0.0);
			monkeys[a].start = posB;
			posB += 7.5;
		}

		glGenVertexArrays(1, &monkeys[a].objVAO);
		glBindVertexArray(monkeys[a].objVAO);

		std::string err = tinyobj::LoadObj(monkeys[a].shapes, monkeys[a].materials, "monkey.obj", 0);

		if (!err.empty()) {
			std::cerr << err << std::endl;
			return;
		}

		monkeys[a].nv = monkeys[a].shapes[0].mesh.positions.size();
		monkeys[a].vertices = new GLfloat[monkeys[a].nv];
		for (i = 0; i<monkeys[a].nv; i++) {
			monkeys[a].vertices[i] = monkeys[a].shapes[0].mesh.positions[i];
		}


		monkeys[a].nn = monkeys[a].shapes[0].mesh.normals.size();
		monkeys[a].normals = new GLfloat[monkeys[a].nn];
		for (i = 0; i<monkeys[a].nn; i++) {
			monkeys[a].normals[i] = monkeys[a].shapes[0].mesh.normals[i];
		}

		monkeys[a].ni = monkeys[a].shapes[0].mesh.indices.size();
		monkeys[a].triangles = monkeys[a].ni / 3;
		monkeys[a].indices = new GLuint[monkeys[a].ni];
		for (i = 0; i<monkeys[a].ni; i++) {
			monkeys[a].indices[i] = monkeys[a].shapes[0].mesh.indices[i];
		}

		glGenBuffers(1, &monkeys[a].vbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, monkeys[a].vbuffer);
		glBufferData(GL_ARRAY_BUFFER, (monkeys[a].nv + monkeys[a].nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, monkeys[a].nv*sizeof(GLfloat), monkeys[a].vertices);
		glBufferSubData(GL_ARRAY_BUFFER, monkeys[a].nv*sizeof(GLfloat), monkeys[a].nn*sizeof(GLfloat), monkeys[a].normals);

		glGenBuffers(1, &monkeys[a].ibuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, monkeys[a].ibuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, monkeys[a].ni*sizeof(GLuint), monkeys[a].indices, GL_STATIC_DRAW);

		glUseProgram(program);
		monkeys[a].vPosition = glGetAttribLocation(program, "vPosition");
		glVertexAttribPointer(monkeys[a].vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(monkeys[a].vPosition);
		monkeys[a].vNormal = glGetAttribLocation(program, "vNormal");
		glVertexAttribPointer(monkeys[a].vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(monkeys[a].nv*sizeof(*monkeys[a].vertices)));
		glEnableVertexAttribArray(monkeys[a].vNormal);
	}

	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int nv;
	int nn;
	int ni;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Load the obj file */

	std::string err = tinyobj::LoadObj(shapes, materials, "ground.obj", 0);

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

	/*
	*  load the vertex coordinate data
	*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

	/*
	*  load the vertex indexes
	*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	*  link the vertex coordinates to the vPosition
	*  variable in the vertex program.  Do the same
	*  for the normal vectors.
	*/
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(*vertices)));
	glEnableVertexAttribArray(vNormal);


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

	view = glm::lookAt(eye, eye + direction, up);

	viewLoc = glGetUniformLocation(program, "modelView");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
	projLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

	colourLoc = glGetUniformLocation(program, "colour");
	glUniform4f(colourLoc, 0.5, 0.5, 0.5, 1.0);
	eyeLoc = glGetUniformLocation(program, "Eye");
	glUniform3f(eyeLoc, eyex, eyey, eyez);
	lightLoc = glGetUniformLocation(program, "light");
	glUniform3f(lightLoc, 1.0, 1.0, 1.0);
	materialLoc = glGetUniformLocation(program, "material");
	glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);
	
	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);


	for(int i = 0; i < 10; i++){

		viewLoc = glGetUniformLocation(program, "modelView");
		glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view * monkeys[i].model));
		projLoc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

		colourLoc = glGetUniformLocation(program, "colour");
		glUniform4f(colourLoc, monkeys[i].colour.x, monkeys[i].colour.y, monkeys[i].colour.z, 1.0);
		eyeLoc = glGetUniformLocation(program, "Eye");
		glUniform3f(eyeLoc, eyex, eyey, eyez);
		lightLoc = glGetUniformLocation(program, "light");
		glUniform3f(lightLoc, 1.0, 1.0, 1.0);
		materialLoc = glGetUniformLocation(program, "material");
		glUniform4f(materialLoc, 0.3, 0.7, 0.7, 150.0);
		
		glBindVertexArray(monkeys[i].objVAO);
		glDrawElements(GL_TRIANGLES, 3*monkeys[i].triangles, GL_UNSIGNED_INT, NULL);
	}

	glutSwapBuffers();
}

void idleFunc() {
	glm::vec3 somevec;
	for(int i = 0; i < 10; i++){
			if(i > 4){
				monkeys[i].model = glm::translate(monkeys[i].model, glm::vec3(-0.1, 0.0, 0.0f));
				monkeys[i].position += glm::vec3(-0.1f, 0.0f, 0.0f);
			} else {
				monkeys[i].model = glm::translate(monkeys[i].model, glm::vec3(0.1, 0.0, 0.0f));
				monkeys[i].position += glm::vec3(0.1f, 0.0f, 0.0f);
			}
	}
	glutPostRedisplay();

}

void keyboardFunc(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':
		direction = glm::mat3(glm::rotate(0.1f,up)) * direction;
		break;
	case 'd':
		direction = glm::mat3(glm::rotate(-0.1f,up)) * direction;
		break;
	case 'w':
		direction = glm::mat3(glm::rotate(0.1f,glm::cross(up,direction))) * direction;
		break;
	case 's':
		direction = glm::mat3(glm::rotate(-0.1f,glm::cross(up,direction))) * direction;
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

	glutPostRedisplay();

}

int main(int argc, char **argv) {
	srand(time(NULL));
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
	srand(time(NULL));
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);
	glutIdleFunc(idleFunc);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	
	vs = buildShader(GL_VERTEX_SHADER, "A3.vs");
	
	fs = buildShader(GL_FRAGMENT_SHADER, "A3.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "A3 shader program");
	init();

	eyex = 0.0;
	eyez = 0.0;
	eyey = -100.0;

	theta = 1.5;
	phi = 1.5;
	r = 100.0;

	glutMainLoop();

}
