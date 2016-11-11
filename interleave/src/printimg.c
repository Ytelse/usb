#include "printimg.h"

#include <stdio.h>
#include <stdlib.h>



void textPrintImg(byte_t* img, int x_size, int y_size, int threshold) {
	int px;
	for (int y = 0; y < y_size; y++) {
		for (int x = 0; y < x_size; x++) {
			px = (img[y*y_size+x] >= threshold) ? 1 : 0;
			fprintf(stdout, "%d ", px);
		}
		fprintf(stdout, "\n");
	}
	printf("\n");
}

void textPrintInterleavedImg(byte_t* i_img, int n, int x_size, int y_size, int threshold) {
	int px;
	for (int y = 0; y < y_size; y++) {
		for (int x = 0; x < x_size * n; x++) {
			px = (i_img[y*y_size*n+x] >= threshold) ? 1 : 0;
			if (px) {
				printf("X ");
			} else {
				printf(". ");
			}
			// printf("%d ", px);
		}
		printf("\n");
	}
	printf("\n");
}

static void colorize(byte_t* p, byte_t px) {
	/* No need to clamp px value, as it is only 8 bits */
	p[0] = 140;
	p[1] = p[2] = px;
}

void savebmp(char* name, byte_t* buffer, int x_size, int y_size) {
  FILE *f = fopen(name, "wb");
  if (!f) {
    printf("Error writing image to disk.\n");
    return;
  }
  unsigned int size = x_size * y_size * 3 + BMP_HEADER_SIZE;
  unsigned char header[BMP_HEADER_SIZE] = {'B', 'M',
                      size&255,
                      (size >> 8)&255,
                      (size >> 16)&255,
                      size >> 24,
                      0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, x_size&255, x_size >> 8, 0,
                      0, y_size&255, y_size >> 8, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  fwrite(header, 1, BMP_HEADER_SIZE, f);
  fwrite(buffer, 1, x_size * y_size * 3, f);
  fclose(f);
}

void output(char* filename, byte_t* img, int x_size, int y_size) {
	unsigned char *buffer = calloc(x_size * y_size * 3, 1);

	for (int j = 0; j < y_size; j++) {
		for (int i = 0; i < x_size; i++) {
			int p = ((y_size - j - 1) * x_size + i) * 3;
			colorize(buffer + p, img[j*x_size + i]);
		}
	}
	savebmp(filename, buffer, x_size, y_size);
	free(buffer);
}