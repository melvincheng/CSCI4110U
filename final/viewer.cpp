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
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdio.h>
#include "Shaders.h"
#include <fstream>
#include <iostream>
#include <vector>

#define CUBE

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles;

void init() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;

	int nv = 32;
	int nn = 24;
	int ni = 36;


	int iter;
	std::string var;
	std::string constants;
	std::string axiom;
	std::string pattern;

	std::vector<std::string> varVec;
	std::vector<std::string> constVec;
	std::vector<std::string> pattVec;




	float angle;

	std::ifstream file;
	std::string line;
	std::string foo = "foo";
	file.open("input.txt");
	// while(std::getline(file,line)){
	// 	std::cout << line << std::endl; 
	// }
	std::getline(file,line);
	iter = std::stoi(line);
	std::cout << "n = " << iter << std::endl;
	std::getline(file, var);
	std::cout << "Variables: " << var << std::endl;
	std::getline(file, constants);
	std::cout << "Constants: " << constants << std::endl;
	std::getline(file, axiom);
	std::cout << "Axiom: " << axiom << std::endl;
	std::getline(file, pattern);
	std::cout << "Patterns: " << pattern << std::endl;
	std::getline(file,line);
	angle = std::stof(line);
	std::cout << "Angle: " << angle << std::endl;


	std::string token;
	int pos = 0;


	while ((pos = var.find(" ")) != std::string::npos){
		token = var.substr(0, pos);
		varVec.push_back(token);
		var.erase(0, pos + 1);
	}
	varVec.push_back(var);

	for(int i = 0; i != varVec.size(); i++){
		std::cout << varVec[i] << " ";
	}
	std::cout<<std::endl;

	while ((pos = constants.find(" ")) != std::string::npos){
		token = constants.substr(0, pos);
		constVec.push_back(token);
		constants.erase(0, pos + 1);
	}
	constVec.push_back(constants);

	for(int i = 0; i != constVec.size(); i++){
		std::cout << constVec[i] << " ";
	}
	std::cout<<std::endl;

	while ((pos = pattern.find(" ")) != std::string::npos){
		token = pattern.substr(0, pos);
		pattVec.push_back(token);
		pattern.erase(0, pos + 1);
	}
	pattVec.push_back(pattern);

	for(int i = 0; i != pattVec.size(); i++){
		std::cout << pattVec[i] << " ";
	}
	std::cout<<std::endl;


	for(int i = 0; i < iter; i++){
		for(int j = 0; j < pattVec.size(); j++){
			if()
		}
	}

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

	vs = buildShader(GL_VERTEX_SHADER, "final.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "final.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "Shader program");
	init();

	eyex = 0.0;
	eyez = 0.0;
	eyey = 10.0;

	theta = 1.5;
	phi = 1.5;
	r = 10.0;

	glutMainLoop();

}
