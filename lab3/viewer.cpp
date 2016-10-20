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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>



#define O_BINARY 0
#define INTERLEAVED

float eyex, eyey, eyez;

double theta, phi;
double r;

GLuint program; 

glm::mat4 projection;

GLuint objVAO;
int triangles;
struct stat buf;

double getSeconds() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + (t.tv_usec / static_cast<double>(1000000.0));
}

void init() {
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
	int i;
	int fid;
	int result;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	result = stat("buddha.bin", &buf);
	
	if(result == 0){

		fid = open("buddha.bin", O_RDONLY| O_BINARY);
		result = read(fid, &nv, (sizeof (nv)));
		result = read(fid, &nn, (sizeof (nn)));
		result = read(fid, &ni, (sizeof (ni)));
		triangles = ni / 3;
		printf("v: %d, n: %d i: %d\n", nv, nn, ni);
		vertices = new GLfloat[nv];
		result = read(fid, vertices, nv*(sizeof (GLfloat)));
		normals = new GLfloat[nn];
		result = read(fid, normals, nn*(sizeof (GLfloat)));
		indices = new GLuint[ni];
		result = read(fid, indices, ni*sizeof (GLuint));
		close(fid);

	}else{
		/*  Load the obj file */

		std::string err = tinyobj::LoadObj(shapes, materials, "buddha.obj", 0);

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

		fid = open("buddha.bin", O_WRONLY| O_BINARY| O_CREAT, S_IREAD| S_IWRITE);
		result = write(fid, &nv, (sizeof (nv)));	
		result = write(fid, &nn, (sizeof (nn)));
		result = write(fid, &ni, (sizeof (ni)));

		result = write(fid, vertices, nv*(sizeof (GLfloat)));
		result = write(fid, normals, nn*(sizeof (GLfloat)));
		result = write(fid, indices, ni*(sizeof (GLuint)));

		close(fid);
	}
	#ifdef INTERLEAVED
	GLfloat* combined = new GLfloat[nv + nn];
	int k=0;
	for(i=0; i<nv/3; i++){
		combined[k++] = vertices[3*i];
		combined[k++] = vertices[3*i+1];
		combined[k++] = vertices[3*i+2];
		combined[k++] = normals[3*i];
		combined[k++] = normals[3*i+1];
		combined[k++] = normals[3*i+2];
	}
	glGenBuffers(1,&vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)*sizeof(GLfloat),combined,GL_STATIC_DRAW);

	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 6*(sizeof(GLfloat)),0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 6*(sizeof(GLfloat)), (void*)(combined+3*(sizeof(GLfloat))));
	glEnableVertexAttribArray(vNormal);
	#else
	/*
	*  load the vertex coordinate data
	*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

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
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(vertices)));
	glEnableVertexAttribArray(vNormal);
	#endif

	/*
	*  load the vertex indexes
	*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	viewLoc = glGetUniformLocation(program, "modelView");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
	projLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projLoc, 1, 0, glm::value_ptr(projection));

	glBindVertexArray(objVAO);
	double t1 = getSeconds();
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);

	glutSwapBuffers();
	printf("%f\n", getSeconds() - t1);
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
	case 'q':
		r += 1;
		break;
	case 'e':
		r -= 1;
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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Viewer");
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

	vs = buildShader(GL_VERTEX_SHADER, "lab3.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab3.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "Lab 3 shader program");
	int currentTime = getSeconds();
	init();
	eyex = 0.0;
	eyez = 0.0;
	eyey = 10.0;

	theta = 1.5;
	phi = 1.5;
	r = 10.0;

	glutMainLoop();
}
