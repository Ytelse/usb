#include "interleave.h"

#include <stdlib.h>

/* Local function prototypes */
static int pack(byte_t* u_img, byte_t* p_img, int len, int threshold);

int init_i_img_buffer(byte_t** i_img_buf, int x_size, int y_size, int n, i_mode_t mode) {
	if (mode == INTERLEAVE_PACKED) {
		*i_img_buf = calloc(x_size*y_size*n/8, sizeof(byte_t));
		// *i_img_buf = malloc(x_size*y_size*n/8*sizeof(byte_t));
		return x_size*y_size*n/8;
	} else {
		*i_img_buf = calloc(x_size*y_size*n, sizeof(byte_t));
		// *i_img_buf = malloc(x_size*y_size*n*sizeof(byte_t));
		return x_size*y_size*n;
	}

}

void destroy_i_img_buffer(byte_t* i_img_buf) {
	free(i_img_buf);
}

int interleave(byte_t** img, byte_t* i_img, int n, int iw, int x_size, int y_size, i_mode_t mode, int threshold) {
	byte_t* temp_result = calloc(n * x_size * y_size, sizeof(byte_t));

	for (int i = 0, i_n = 0; i < n*x_size*y_size; i+=n*iw, i_n+=iw) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < iw; k++) {
				temp_result[i+j*iw+k] = img[j][i_n+k];			
			}
		}
	}

	int len;

	if (mode == INTERLEAVE_PACKED) {
		/* The pack function assigns values to the interleaved image array */
		len = pack(temp_result, i_img, n*x_size*y_size, threshold);
	} else {
		/* If no packing we need to assign the temp values to the interleaved image array */
		for (int i = 0; i < n*x_size*y_size; i++) {
			i_img[i] = temp_result[i];
		} 
		len = n*x_size*y_size;
	}
	free(temp_result);
	return len;
}

/* Assuming len is divisible by 8, which it should be as 784 is divisible by 8 */

static int pack(byte_t* u_img, byte_t* p_img, int len, int threshold) {
	byte_t byte;
	unsigned int pixel;
	for (int i = 0; i < len/8; i++) {
		byte = 0;
		for (int j = 0; j < 8; j++) {
			pixel = (u_img[i*8+j] >= threshold) ? 1 : 0;
			byte |= (pixel << (7-j));
		}
		p_img[i] = byte;
	}
	return len/8;
}







