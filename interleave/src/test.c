#include "interleave.h"
#include "deinterleave.h"
#include "printimg.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "i_defs.h"

void output_not_packed_images(byte_t** not_packed_results);
void output_not_packed_deinterleaved_image(byte_t* result);

int main(void) {
	struct timeval start, end;

	/* ========== DO FILE I/O ========== */

	byte_t** img;
	img = malloc(sizeof(byte_t*) * NOF_IMAGES);
	for (int i = 0; i < NOF_IMAGES; i++) {
		img[i] = malloc(sizeof(byte_t) * IMG_SIZE);
	}
	printf("Reading images...\n");
	gettimeofday(&start, NULL);
	FILE* f;
	f = fopen(IMG_FP, "rb");
	assert(f);
	for (int n = 0; n < NOF_IMAGES; n++) {
		for (int px = 0; px < IMG_SIZE; px++) {
			img[n][px] = getc(f);
		}
	}
	gettimeofday(&end, NULL);
 	long int ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    double s = ms/1e6;
 	printf("File I/O completed in %f s\n", s);
	fclose(f);

	/* ========== DO INTERLEAVING ========== */

	byte_t** results = malloc(sizeof(byte_t*) * NOF_IMAGES/INTERLEAVE_N);
	byte_t** not_packed_results = malloc(sizeof(byte_t*) * NOF_IMAGES/INTERLEAVE_N);

	printf("Interleaving...\n");
	gettimeofday(&start, NULL);

	for (int n = 0; n < NOF_IMAGES/INTERLEAVE_N; n++) {
		init_i_img_buffer(&results[n], IMG_X, IMG_Y, INTERLEAVE_N, INTERLEAVE_PACKED);
		init_i_img_buffer(&not_packed_results[n], IMG_X, IMG_Y, INTERLEAVE_N, INTERLEAVE_UNPACKED);

		interleave(&img[n*INTERLEAVE_N], results[n], INTERLEAVE_N, INTERLEAVE_WIDTH, IMG_X, IMG_Y, INTERLEAVE_PACKED, THRESHOLD);
		interleave(&img[n*INTERLEAVE_N], not_packed_results[n], INTERLEAVE_N, INTERLEAVE_WIDTH, IMG_X, IMG_Y, INTERLEAVE_UNPACKED, THRESHOLD);
	}

	gettimeofday(&end, NULL);
	ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    s = ms/1e6;
 	
 	printf("Interleaving completed in %f s\n", s);

 	/* ========== PRINT INTERLEAVED IMAGES ========== */

 	output_not_packed_images(not_packed_results);

 	output_not_packed_deinterleaved_image(not_packed_results[0]);
 	/* ========== FREE MEMORY ========== */

 	for (int n = 0; n < NOF_IMAGES/INTERLEAVE_N; n++) {
 		destroy_i_img_buffer(results[n]);
 		destroy_i_img_buffer(not_packed_results[n]);
 	}

 	free(results);
 	free(not_packed_results);
}

void output_not_packed_images(byte_t** results) {
	for (int i = 0; i < NOF_IMAGES/INTERLEAVE_N; i++) {
 		char filename[80];
 		sprintf(filename, OUTPUT_PATH "non_packed_interleaved_image_%.4d.bmp", i);
 		output(filename, results[i], IMG_X*INTERLEAVE_N, IMG_Y);
 	}
}

void output_not_packed_deinterleaved_image(byte_t* result) {
	byte_t** di_imgs = malloc(INTERLEAVE_N * sizeof(byte_t*));

	for (int i = 0; i < INTERLEAVE_N; i++) {
		init_di_img_buffer(&di_imgs[i], IMG_X, IMG_Y);
	}

	for (int i = 0; i < INTERLEAVE_N; i++) {
		for (int j = 0; j < 255; j++) {
			di_imgs[i][j] = 255;
		}
	}

	char filename1[80];
	sprintf(filename1, OUTPUT_PATH "resulttest.bmp");
	output(filename1, result, IMG_X*INTERLEAVE_N, IMG_Y);

	deinterleave(result, di_imgs, INTERLEAVE_N, INTERLEAVE_WIDTH, IMG_X, IMG_Y, INTERLEAVE_UNPACKED);

	for (int i = 0; i < INTERLEAVE_N; i++) {
		char filename[80];
		sprintf(filename, OUTPUT_PATH "test_%.4d.bmp", i);
		output(filename, di_imgs[i], IMG_X, IMG_Y);
	}
}


