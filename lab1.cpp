#include <FreeImage.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define	WIDTH 512
#define HEIGHT 512

unsigned char image[HEIGHT][WIDTH][3];
bool flip = true;


void makeImage(){
	int i;
	int j;

	for (j = 0; j < HEIGHT; j++){
		for (i = 0; i < WIDTH; i++){
			image[j][i][0] = j % 256;
			image[j][i][1] = 0;
			image[j][i][2] = 0;
		}
	}
}

void displayFunc(){
	glLoadIdentity();
	glViewport(0, 0, 511, 511);
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (flip){
		glRasterPos2i(0, 511);
		glPixelZoom(1.0f, -1.0f);
	}
	else{
		glRasterPos2i(0, 0);
		glPixelZoom(1.0f, 1.0);
	}

	glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image);
	glFlush();
}

void keyboardFunc(unsigned char k, int, int){
	switch (k){
	case 'f':
		flip = !flip;
		break;
	}
	glutPostRedisplay();
}

void saveImage(char *filename){
	int i;
	int j;
	FIBITMAP *bitmap;
	BYTE *bits;

	bitmap = FreeImage_Allocate(WIDTH, HEIGHT, 24);
	for (j = 0; j < HEIGHT; j++){
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i < WIDTH; i++){
			bits[FI_RGBA_RED] = image[j][i][0];
			bits[FI_RGBA_GREEN] = image[j][i][1];
			bits[FI_RGBA_BLUE] = image[j][i][2];
			bits += 3;
		}
	}
	FreeImage_Save(FIF_PNG, bitmap, filename, PNG_DEFAULT);
}

void loadImage(char *filename){
	int i, j;
	FIBITMAP *bitmap;
	BYTE *bits;

	bitmap = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
	for (j = 0; j < HEIGHT; j++){
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i < WIDTH; i++){
			image[j][i][0] = bits[FI_RGBA_RED];
			image[j][i][1] = bits[FI_RGBA_GREEN];
			image[j][i][2] = bits[FI_RGBA_BLUE];
			bits += 3;
		}
	}
	FreeImage_Unload(bitmap);
}

int main(int argc, char **argv){

	makeImage();
	saveImage("name.png");
	//loadImage("name.png");

	glutInit(&argc, argv);
	glutInitWindowSize((GLsizei)WIDTH, (GLsizei)HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Image Display");


	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);


	glutMainLoop();
}
