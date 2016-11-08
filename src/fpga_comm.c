#include "defs.h"
#include "pthread_helper.h"
#include "debug.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define IMG_FP "resources/train-images-idx3-ubyte"

#define NOF_IMAGES 70000	// Test set size + training set size
#define IMG_SIZE 784	// 28*28 -- Full size of the MNIST images

extern int _keepalive;
extern barrier_t barrier;

typedef unsigned char byte_t;

/* Test functions */
static void printImg(byte_t* img);

void * fpga_runloop(void* pdata_void_ptr) {
	/* Cast void ptr back to original type */
	pdata_t* pdata = (pdata_t*) pdata_void_ptr;
	/* Array of images */
	byte_t** img;
	img = (byte_t**) malloc(sizeof(byte_t*) * NOF_IMAGES);
	for (int i = 0; i < NOF_IMAGES; i++) {
		img[i] = (byte_t*) malloc(sizeof(byte_t)*IMG_SIZE);
	}
	
	#ifdef DEBUG
	/* Time file I/O */
	struct timeval start, end;
	#endif
	
	debugprint("FPGA_comm: Reading MNIST data set into memory...", DEFAULT);

	FILE* f;
	f = fopen(IMG_FP, "rb");

	if (!f) {
		colorprint("ERROR: FPGA_comm: Failed to open image set!", RED);
		return NULL;
	}

	#ifdef DEBUG
	gettimeofday(&start, NULL);
	#endif

	for (int n = 0; n < NOF_IMAGES; n++) {
		for (int px = 0; px < IMG_SIZE; px++) {
			img[n][px] = getc(f);
		}
 	}

	#ifdef DEBUG
 	gettimeofday(&end, NULL);
 	long int ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    double s = ms/1e6;
 	printf("FPGA_comm: File I/O completed in %f s\n", s);
	#endif

 	fclose(f);

 	debugprint("FPGA_comm: Finished reading MNIST dataset, synchronizing with MCU thread.", GREEN);
 	/* Sync up with MCU thread */
 	barrier_wait(&barrier);

 	debugprint("FPGA_comm: Got past barrier!", GREEN);

 	printImg(img[0]);

 	while (_keepalive) {};

 	debugprint("FPGA_comm: Finished busy-wait. Freeing images.", DEFAULT);

 	/* Free MNIST dataset */

 	for (int i = 0; i < NOF_IMAGES; i++) {
 		free(img[i]);
 	}
 	free(img);

	return NULL;

}

/**
 *	Description:
 *		Interleaves images in order to help the FPGA to process images in parallel.
 *	Params:
 *		int n,				-- Number of images to be interleaved
 *		int iw,				-- Interleave width, number of bits in sequence from each image
 *		image_t* images,	-- Pointer to the first of the n images to be interleaved
 *		image_t* saveptr	-- I images == NULL, saveptr will be used as starting point.
 *		byte_t* result		-- Pointer to the resulting array. Will be malloc'd in this function, and needs to be freed by the caller.
 *	
 *	Returns: Length of the result array
 */	

static int interleave(int n, int iw, byte_t** img, byte_t** saveptr, byte_t* result) {
	/* Temporary result, stored in bytes */
	byte_t* temp_result = (byte_t*) malloc(n * IMG_SIZE * sizeof(byte_t));
	/* Final result, bits packed in bytes */
	result = (byte_t*) malloc(((n * IMG_SIZE) / 8) * sizeof(byte_t));

	byte_t** img_ptr;
	/* If images == NULL use saveptr */
	img_ptr = (img) ? img : saveptr;

	/* Interleave images into temp_result */

	/* TODO: Fix this loop, unsure if it produces correct result */

	for (int i = 0; i < n*IMG_SIZE; i+=iw) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < iw; k++) {
				temp_result[i + j * iw + k] = img_ptr[j][i+k]
			}
		}
	}

	/* Pack temp_result into result */

	byte_t byte = 0;
	for (int i = 0; i < bytesize; i++) {
		byte = 0;
		for (int i = 0; i < 8; i++) {

		}
	}

	free(temp_result);
	saveptr = img_ptr + n;

}

/* TEST */

static void printImg(byte_t* img) {
	for (int x = 0; x < 28; x++) {
		for (int y = 0; y < 28; y++) {
			fprintf(stdout, "%d ", (int) img[x * 28 + y]);
		}
		fprintf(stdout, "\n");
	}
}