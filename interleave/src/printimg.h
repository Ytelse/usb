#ifndef __PRINTIMG_H_
#define __PRINTIMG_H_

#define BMP_HEADER_SIZE 54

typedef unsigned char byte_t;

void textPrintImg(byte_t* img, int x_size, int y_size, int threshold);
void textPrintInterleavedImg(byte_t* i_img, int n, int x_size, int y_size, int threshold);
void output(char* filename, byte_t* img, int x_size, int y_size);

#endif

