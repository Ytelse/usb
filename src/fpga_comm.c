#include "defs.h"
#include "i_defs.h"

#include "interleave.h"
#include "pthread_helper.h"
#include "debug.h"

#include "printimg.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


extern int _keepalive;
extern barrier_t barrier;

void * fpga_runloop(void* pdata_void_ptr) {

	// printf("Recompile\n");

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
		printf("\rReading images -- %2d%% done...", n / (NOF_IMAGES/100));
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

 	// int img_num = 0;

 	byte_t *i_img_buf0, *i_img_buf1;
 	init_i_img_buffer(&i_img_buf0, IMG_X, IMG_Y, ITRLV_N, INTERLEAVE_UNPACKED);
 	init_i_img_buffer(&i_img_buf1, IMG_X, IMG_Y, ITRLV_N, INTERLEAVE_UNPACKED);
 	while (_keepalive) {
 		// /* Outupt some images just for testing */
 		// if (img_num < ((20 < NOF_IMAGES) ? 20 : NOF_IMAGES)) {
 		// 	char filename[80];
 		// 	if (img_num % 2) {
 		// 		sprintf(filename, OUTPUT_PATH "image_%.4d_b0.bmp", img_num);
 		// 		interleave(&img[img_num*ITRLV_N], i_img_buf0, ITRLV_N, ITRLV_W, IMG_X, IMG_Y, INTERLEAVE_UNPACKED, THRESHOLD);
 		// 		output(filename, i_img_buf0, IMG_X*ITRLV_N, IMG_Y);
 		// 	} else {
 		// 		sprintf(filename, OUTPUT_PATH "image_%.4d_b1.bmp", img_num);
 		// 		interleave(&img[img_num*ITRLV_N], i_img_buf1, ITRLV_N, ITRLV_W, IMG_X, IMG_Y, INTERLEAVE_UNPACKED, THRESHOLD);
 		// 		output(filename, i_img_buf1, IMG_X*ITRLV_N, IMG_Y);
 		// 	}
 		// }
 		// img_num++;
 	};

 	debugprint("FPGA_comm: Finished busy-wait. Freeing images.", DEFAULT);

 	/* Free MNIST dataset */

 	for (int i = 0; i < NOF_IMAGES; i++) {
 		free(img[i]);
 	}
 	free(img);

 	destroy_i_img_buffer(i_img_buf0);
 	destroy_i_img_buffer(i_img_buf1);

	return NULL;

}