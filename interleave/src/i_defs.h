#ifndef __I_DEFS_H
#define __I_DEFS_H

typedef unsigned char byte_t;
typedef enum {INTERLEAVE_PACKED, INTERLEAVE_UNPACKED} i_mode_t;

/* Image filepath */
#define IMG_FP "resources/mnist-ubyte-no-header"
/* Number of images we want to fetch from data set. Max = 70000 */
#define NOF_IMAGES 128 

/* Dataset image dimensions */
#define IMG_X 28
#define IMG_Y 28
#define IMG_SIZE 784

/* Threshold for use when downsampling images */
#define THRESHOLD 100

/* Number of images to interleave at a time */
#define ITRLV_N 4
/* Number of pixels to take from each image in turn */
#define ITRLV_W 28

#define OUTPUT_PATH "resources/img_out/"

/*
	Ex. 
		INTERLEAVE_N 4, INTERLEAVE_WIDTH 4 and
		array0 = [0 0 0 0 0 0...],
		array1 = [1 1 1 1 1 1...],
		array2 = [2 2 2 2 2 2...],
		array3 = [3 3 3 3 3 3...],

		gives the following result:
	
		[0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3 0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3...]

*/

#endif