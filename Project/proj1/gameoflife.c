/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	Color *new_color = (Color *)malloc(sizeof(Color));
	if (new_color == NULL) {
		fprintf(stderr, "Memory allocation failed for new Color struct\n");
		return NULL;
	}

	int rows = image->rows;
	int cols = image->cols;
	new_color->R = 0;
	new_color->G = 0;
	new_color->B = 0;

	for (int bit = 0; bit < 8; bit++) {
		int live_neighbors_r = 0;
		int live_neighbors_g = 0;
		int live_neighbors_b = 0;

		for (int dr = -1; dr <= 1; dr++) {
			for (int dc = -1; dc <= 1; dc++) {
				if (dr == 0 && dc == 0) {
					continue;
				}

				int neighbor_row = (row + dr + rows) % rows;
				int neighbor_col = (col + dc + cols) % cols;
				Color neighbor = image->image[neighbor_row][neighbor_col];

				live_neighbors_r += (neighbor.R >> bit) & 1;
				live_neighbors_g += (neighbor.G >> bit) & 1;
				live_neighbors_b += (neighbor.B >> bit) & 1;
			}
		}

		Color current = image->image[row][col];
		int current_r = (current.R >> bit) & 1;
		int current_g = (current.G >> bit) & 1;
		int current_b = (current.B >> bit) & 1;

		int rule_index_r = current_r * 9 + live_neighbors_r;
		int rule_index_g = current_g * 9 + live_neighbors_g;
		int rule_index_b = current_b * 9 + live_neighbors_b;

		int next_r = (rule >> rule_index_r) & 1;
		int next_g = (rule >> rule_index_g) & 1;
		int next_b = (rule >> rule_index_b) & 1;

		new_color->R |= (next_r << bit);
		new_color->G |= (next_g << bit);
		new_color->B |= (next_b << bit);
	}

	return new_color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
	Image *new_image = (Image *)malloc(sizeof(Image));
	if (new_image == NULL) {
		fprintf(stderr, "Memory allocation failed for Image struct\n");
		return NULL;
	}
	new_image->rows = image->rows;
	new_image->cols = image->cols;
	new_image->image = (Color **)malloc(new_image->rows * sizeof(Color *));
	if (new_image->image == NULL) {
		fprintf(stderr, "Memory allocation failed for image rows.\n");
		free(new_image);
		return NULL;
	}
	for (int i = 0; i < new_image->rows; i++) {
		new_image->image[i] = (Color *)malloc(new_image->cols * sizeof(Color));
		if (new_image->image[i] == NULL) {
			fprintf(stderr, "Memory allocation failed for image columns.\n");
			for (int j = 0; j < i; j++) {
				free(new_image->image[j]);
			}
			free(new_image->image);
			free(new_image);
			return NULL;
		}
		for (int j = 0; j < new_image->cols; j++) {
			Color *new_color = evaluateOneCell(image, i, j, rule);
			if (new_color == NULL) {
				for (int k = 0; k <= i; k++) {
					free(new_image->image[k]);
				}
				free(new_image->image);
				free(new_image);
				return NULL;
			}
			new_image->image[i][j] = *new_color;
			free(new_color);
		}
	}
	return new_image;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input.ppm> <rule>\n", argv[0]);
		return -1;
	}

	Image *image = readData(argv[1]);
	if (image == NULL) {
		return -1;
	}

	char *endptr;
	uint32_t rule = strtoul(argv[2], &endptr, 16);
	if (*endptr != '\0') {
		fprintf(stderr, "Invalid rule: %s\n", argv[2]);
		freeImage(image);
		return -1;
	}

	Image *new_image = life(image, rule);
	if (new_image == NULL) {
		freeImage(image);
		return -1;
	}

	writeData(new_image);

	freeImage(image);
	freeImage(new_image);
	return 0;
}
