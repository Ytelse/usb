#ifndef __DEINTERLEAVE_H_
#define __DEINTERLEAVE_H_

#include "i_defs.h"

int init_di_img_buffer(byte_t** di_img_buf, int x_size, int y_size);
void destroy_di_img_buffer(byte_t* di_img_buf);
int deinterleave(byte_t* i_img, byte_t** di_img, int n, int iw, int x_size, int y_size, i_mode_t mode);

#endif
