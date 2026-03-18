/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	Color *new_color = (Color *)malloc(sizeof(Color));
	if (new_color == NULL) {
		fprintf(stderr, "Memory allocation failed for new Color struct\n");
		return NULL;
	}
	uint8_t blue_value = image->image[row][col].B;
	if (blue_value & 1) {
		new_color->R = 255;
		new_color->G = 255;
		new_color->B = 255;
	} else {
		new_color->R = 0;
		new_color->G = 0;
		new_color->B = 0;
	}
	return new_color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	Image *img = (Image *)malloc(sizeof(Image));
	if (img == NULL) {
		fprintf(stderr, "Memory allocation failed for Image struct\n");
		return NULL;
	}
	img->rows = image->rows;
	img->cols = image->cols;
	img->image = (Color **)malloc(img->rows * sizeof(Color *));
	if (img->image == NULL) {
		fprintf(stderr, "Memory allocation failed for image rows.\n");
		free(img);
		return NULL;
	}
	for (int i = 0; i < img->rows; i++) {
		img->image[i] = (Color *)malloc(img->cols * sizeof(Color));
		if (img->image[i] == NULL) {
			fprintf(stderr, "Memory allocation failed for image row %d.\n", i);
			for (int j = 0; j < i; j++) {
				free(img->image[j]);
			}
			free(img->image);
			free(img);
			return NULL;
		}
		for (int j = 0; j < img->cols; j++) {
			Color *new_color = evaluateOnePixel(image, i, j);
			img->image[i][j] = *new_color;
			free(new_color);
		}
	}
	return img;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input.ppm>\n", argv[0]);
		return -1;
	}

	Image *image = readData(argv[1]);
	if (image == NULL) {
		return -1;
	}

	Image *stego_image = steganography(image);
	if (stego_image == NULL) {
		freeImage(image);
		return -1;
	}

	writeData(stego_image);

	freeImage(image);
	freeImage(stego_image);
	return 0;
}
