#include "defs.h"
#include "pthread_helper.h"
#include "debug.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define IMAGES_FP "resources/bit_mnist_images"
#define NOF_IMAGES 70000

extern int _keepalive;
extern barrier_t barrier;

typedef unsigned char byte_t;

typedef struct Image {
	/* Array of bytes containing 8 pixels each */
	byte_t packed_px[98];
} image_t;

/* Test functions */
static void read_bitfile_image(image_t image, int* img);
static void printImg(int* img);

void * fpga_runloop(void* pdata_void_ptr) {
	/* Cast void ptr back to original type */
	pdata_t* pdata = (pdata_t*) pdata_void_ptr;
	/* Array of images */
	image_t* images;
	images = (image_t*) malloc(sizeof(image_t) * NOF_IMAGES);
	
	#ifdef DEBUG
	/* Time file I/O */
	struct timeval start, end;
	#endif
	
	debugprint("FPGA_comm: Reading MNIST data set into memory...", DEFAULT);

	FILE* f;
	f = fopen(IMAGES_FP, "rb");

	if (!f) {
		colorprint("ERROR: FPGA_comm: Failed to open image set!", RED);
		return NULL;
	}

	#ifdef DEBUG
	gettimeofday(&start, NULL);
	#endif

	for (int n = 0; n < NOF_IMAGES; n++) {
		for (int p = 0; p < 98; p++) {
			images[n].packed_px[p] = getc(f);
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

 	while (_keepalive) {};

 	debugprint("FPGA_comm: Finished busy-wait. Freeing images.", DEFAULT);

 	free(images);

	return NULL;

}

/* TEST */

static void read_bitfile_image(image_t image, int* img) {
	byte_t temp = 0;
	for (int i = 0; i < 98; i++) {
		temp = image.packed_px[i];
		for (int offset = 0; offset < 8; offset++) {
			img[i * 8 + offset] = (temp & (1 << (7 - offset))) >> (7 - offset);
		}
	}
}

static void printImg(int* img) {
	for (int x = 0; x < 28; x++) {
		for (int y = 0; y < 28; y++) {
			fprintf(stdout, "%d ", img[x * 28 + y]);
		}
		fprintf(stdout, "\n");
	}
}