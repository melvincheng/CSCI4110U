#include <stdio.h>
#include <stdlib.h>
#include <FreeImage.h>
#include "perlin.h"

#define	WIDTH 512
#define HEIGHT 512

unsigned char image[HEIGHT][WIDTH][3];
bool flip = true;


void makeImage(float f){
	float vec[2];

	int i, j;
	float dx, dy;
	float n;

	dx = f/WIDTH;
	dy = f/HEIGHT;
	vec[0] = vec[1] = 0.0;
	for (i = 0; i < WIDTH; i++){
		vec[0] = i*dx;
		for (j = 0; j < HEIGHT; j++){
			vec[1] = j*dy;
			n = noise2(vec);
			n = 0.5*n+0.5;
			image[i][j][0] = 256*n;
			image[i][j][1] = 256*n;
			image[i][j][2] = 256*n;
		}
	}
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
	float f;

	f = 10.0;
	if(argc > 1)
		f = atof(argv[1]);
	printf("f: %f\n", f);
	makeImage(f);
	saveImage("perlin.png");
}
