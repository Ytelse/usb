#ifndef __INTERLEAVE_H_
#define __INTERLEAVE_H_

#include "i_defs.h"

int init_i_img_buffer(byte_t** i_img_buf, int x_size, int y_size, int n, i_mode_t mode);

void destroy_i_img_buffer(byte_t* i_img_buf);

int interleave(
	byte_t** img, 			/* In */	/* Images to be interleaved */
	byte_t* i_img,			/* Out */	/* Pointer to the new interleaved image */
	int n,					/* In */	/* Number of images to interleave */
	int iw,					/* In */	/* Number of bits to interleave */
	int x_size, int y_size,	/* In */	/* Dimensions of images to be interleaved */
	i_mode_t mode,			/* In */	/* Interleave mode, packed or unpacked result */
	int threshold			/* In */	/* Threshold for use when downsampling from byte to bit */
);

#endif