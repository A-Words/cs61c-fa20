/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		return NULL;
	}
	char format[3];
	int max_color_value;
	Image *img = (Image *)malloc(sizeof(Image));
	if (img == NULL) {
		fprintf(stderr, "Memory allocation failed for Image struct\n");
		fclose(file);
		return NULL;
	}
	if (fscanf(file, "%2s", format) != 1 || strcmp(format, "P3") != 0) {
		fprintf(stderr, "Invalid file format. Expected P3.\n");
		free(img);
		fclose(file);
		return NULL;
	}
	if (fscanf(file, "%u %u", &img->cols, &img->rows) != 2) {
		fprintf(stderr, "Failed to read image dimensions.\n");
		free(img);
		fclose(file);
		return NULL;
	}
	if (fscanf(file, "%d", &max_color_value) != 1 || max_color_value != 255) {
		fprintf(stderr, "Invalid max color value. Expected 255.\n");
		free(img);
		fclose(file);
		return NULL;
	}
	img->image = (Color **)malloc(img->rows * sizeof(Color *));
	if (img->image == NULL) {
		fprintf(stderr, "Memory allocation failed for image rows.\n");
		free(img);
		fclose(file);
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
			fclose(file);
			return NULL;
		}
		for (int j = 0; j < img->cols; j++) {
			int r, g, b;
			if (fscanf(file, "%d %d %d", &r, &g, &b) != 3) {
				fprintf(stderr, "Failed to read color values for pixel (%d, %d).\n", i, j);
				for (int k = 0; k <= i; k++) {
					free(img->image[k]);
				}
				free(img->image);
				free(img);
				fclose(file);
				return NULL;
			}
			img->image[i][j].R = (uint8_t)r;
			img->image[i][j].G = (uint8_t)g;
			img->image[i][j].B = (uint8_t)b;
		}
	}
	fclose(file);
	return img;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	printf("P3\n%u %u\n255\n", image->cols, image->rows);
	for (uint32_t i = 0; i < image->rows; i++) {
		for (uint32_t j = 0; j < image->cols; j++) {
			printf("%3u %3u %3u", image->image[i][j].R, image->image[i][j].G, image->image[i][j].B);
			if (j < image->cols - 1) {
				printf("   ");
			}
		}
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	for (uint32_t i = 0; i < image->rows; i++) {
		free(image->image[i]);
	}
	free(image->image);
	free(image);
}