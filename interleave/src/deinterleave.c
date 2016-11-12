#include "deinterleave.h"

#include <stdlib.h>
#include <stdio.h>

#include "printimg.h"

/* Local function prototypes */
static int _unpack(byte_t* p_img, byte_t* u_img, int len);
static int _deinterleave_packed(byte_t* i_img, byte_t** di_img, int n, int iw, int x_size, int y_size);
static int _deinterleave(byte_t* i_img, byte_t** di_img, int n, int iw, int x_size, int y_size);

int init_di_img_buffer(byte_t** di_img_buf, int x_size, int y_size) {
	*di_img_buf = calloc(x_size*y_size, sizeof(byte_t*));
	return x_size*y_size;
}

void destroy_di_img_buffer(byte_t* di_img_buf) {
	free(di_img_buf);
}

int deinterleave(byte_t* i_img, byte_t** di_img, int n, int iw, int x_size, int y_size, i_mode_t mode) {
	if  (mode == INTERLEAVE_PACKED) {
		return _deinterleave_packed(i_img, di_img, n, iw, x_size, y_size);
	} else {
		return _deinterleave(i_img, di_img, n, iw, x_size, y_size);
	}
}

static int _deinterleave_packed(byte_t* ip_img, byte_t** di_img, int n, int iw, int x_size, int y_size) {
	byte_t* iup_img = calloc(x_size*y_size*n, sizeof(byte_t));
	int size = 0;
	
	_unpack(ip_img, iup_img, x_size*y_size*n/8);
	size = _deinterleave(iup_img, di_img, n, iw, x_size, y_size);
	
	free(iup_img);
	return size;
}

/* TODO: Fix deinterleaving. Not essential. */

static int _deinterleave(byte_t* i_img, byte_t** di_img, int n, int iw, int x_size, int y_size) {
	for (int img = 0; img < n; img++) {
		for (int j = 0; j < y_size; j++) {
			for (int i = 0; i < x_size; i+=iw) {
				for (int k = 0; k < iw; k++) {
					di_img[img][j*x_size+i+k] = i*6 + k*6;
				}
			}
		}
	}

	return n;
}

static int _unpack(byte_t* p_img, byte_t* up_img, int len) {

	for (int i = 0; i < len; i++) {
		for (int offset = 0; offset < 8; offset++) {
			up_img[i*8+offset] = ((p_img[i] & (1 << (7 - offset))) >> (7 - offset)) ? 255 : 0;
		}

	}
	return len*8;
}


