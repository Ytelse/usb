#include "defs.h"
#include "pthread_helper.h"
#include "debug.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define IMG_FP "resources/mnist-ubyte-no-header"

#define NOF_IMAGES 70000	// Test set size + training set size
#define IMG_Y 28
#define IMG_X 28
#define IMG_SIZE 784		// 28*28 -- Full size of the MNIST images
#define THRESHOLD 40		// Threshold for setting pixel value to 0 or 1


#define INTERLEAVE_N 4
#define INTERLEAVE_W 28

extern int _keepalive;
extern barrier_t barrier;

static byte_t* interleave(int n, int iw, byte_t** img, int* result_length);
byte_t* unpack(byte_t* p_img, int n);

/* Test functions */
void printInterleavedImg(byte_t* i_img, int n);


void * fpga_runloop(void* pdata_void_ptr) {

	/* Two buffers to hold interleaved images for sending to the FPGA */
	/* Double buffering allows for interleaving one batch while sending another using async sends */
	byte_t *i_img_buf0, *i_img_buf1;

	/* Cast void ptr back to original type */
	pdata_t* pdata = (pdata_t*) pdata_void_ptr;
	UNUSED(pdata);
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
		printf("\rReading images -- %2d%% done...", n / 700);
 	}
 	printf("\rReading images -- 100%% done.\n");

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

 	/* TODO: Some crazy pointer magic memory mumbo jumbo going on here. Fix it */

 	int result_length;
 	i_img_buf0 = interleave(INTERLEAVE_N, INTERLEAVE_W, &img[0], &result_length);
 	i_img_buf1 = interleave(INTERLEAVE_N, INTERLEAVE_W, &img[0], &result_length);

 	byte_t* unpacked0 = unpack(i_img_buf0, INTERLEAVE_N);
 	byte_t* unpacked1 = unpack(i_img_buf1, INTERLEAVE_N);
 	printInterleavedImg(unpacked0, INTERLEAVE_N);
 	printInterleavedImg(unpacked1, INTERLEAVE_N);
 	free(unpacked0);
 	free(unpacked1);


 	free(i_img_buf0);
 	free(i_img_buf1);

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
 *		int* result_length	-- Pointer to a place to store the length of the result
 *	
 *	Returns: Pointer to array containing the packed result of the interleaving. Must be 'free'd by the caller.
 */	

/* TODO: Requires some further testing in order to check that the packing works as intended */

static byte_t* interleave(int n, int iw, byte_t** img, int* result_length) {
	/* Temporary result, stored in bytes */
	byte_t* temp_result = malloc(n * IMG_SIZE * sizeof(byte_t));
	/* Final result, bits packed in bytes */
	byte_t* result = malloc(((n * IMG_SIZE) / 8) * sizeof(byte_t));

	/* Interleave images into temp_result */

	for (int i = 0, i_n = 0; i < n*IMG_SIZE; i+=n*iw, i_n+=iw) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < iw; k++) {
				temp_result[i + j*iw + k] = img[j][i_n + k];
			}
		}
	}

	/* Pack temp_result into result */

	byte_t byte;
	unsigned int pixel;
	for (int i = 0; i < (n*IMG_SIZE)/8; i++) {
		byte = 0;
		for (int j = 0; j < 8; j++) {
			pixel = temp_result[i*8 + j];
			if (pixel >= THRESHOLD) {
				pixel = 1;
			} else {
				pixel = 0;
			}
			// pixel = (pixel >= THRESHOLD) ? 1 : 0;
			byte |= (pixel << (7 - i));
		}
		result[i] = byte;
	}

	free(temp_result);

	*result_length = n*IMG_SIZE/8;
	return result;
}

byte_t* unpack(byte_t* p_img, int n) {
	byte_t* u_img;
	u_img = malloc(sizeof(byte_t) * n * IMG_SIZE);

	for (int i = 0; i < (n * IMG_SIZE)/8; i+=8) {
		u_img[i+0] = p_img[i] & (1 << 7);
		u_img[i+1] = p_img[i] & (1 << 6);
		u_img[i+2] = p_img[i] & (1 << 5);
		u_img[i+3] = p_img[i] & (1 << 4);
		u_img[i+4] = p_img[i] & (1 << 3);
		u_img[i+5] = p_img[i] & (1 << 2);
		u_img[i+6] = p_img[i] & (1 << 1);
		u_img[i+7] = p_img[i] & (1 << 0);
	}
	return u_img;
}

void printInterleavedImg(byte_t* i_img, int n) {
	int px;
	for (int y = 0; y < IMG_Y; y++) {
		for (int x = 0; x < IMG_X * n; x++) {
			px = (i_img[y*28*n+x] >= THRESHOLD) ? 1 : 0;
			printf("%d ", px);
		}
		printf("\n");
	}
	printf("\n");
}