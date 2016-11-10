#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#define IMG_FP "mnist-ubyte-no-header"

#define IMG_X 28
#define IMG_Y 28
#define IMG_SIZE 784

#define NOF_IMAGES 70000

#define INTERLEAVE_N 4
#define INTERLEAVE_WIDTH 28

#define THRESHOLD 40

typedef unsigned char byte_t;

byte_t* interleave(int n, int iw, byte_t** img, int* result_length);
byte_t* interleave_no_pack(int n, int iw, byte_t** img, int* result_length);
byte_t* unpack(byte_t* p_img, int n);

void printImg(byte_t* img);
void printInterleavedImg(byte_t* i_img, int n);

int main(void) {

	struct timeval start, end;

	byte_t** img;
	img = malloc(sizeof(byte_t*) * NOF_IMAGES);
	for (int i = 0; i < NOF_IMAGES; i++) {
		img[i] = malloc(sizeof(byte_t) * IMG_SIZE);
	}

/* ============================================================================================ */

	printf("Reading images...\n");
	gettimeofday(&start, NULL);

	FILE* f;
	f = fopen(IMG_FP, "rb");
	assert(f);
	for (int n = 0; n < NOF_IMAGES; n++) {
		for (int px = 0; px < IMG_SIZE; px++) {
			img[n][px] = getc(f);
		}
	}

	gettimeofday(&end, NULL);
 	long int ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    double s = ms/1e6;
 	printf("File I/O completed in %f s\n", s);
	printf("Images loaded successfully!\n\n");

	fclose(f);

/* ============================================================================================ */

	printf("Interleaving\n");

	byte_t* result;
	int result_length;

	gettimeofday(&start, NULL);
	
	result = interleave(INTERLEAVE_N, INTERLEAVE_WIDTH, img, &result_length);

	free(result);
	for (int n = 1; n < NOF_IMAGES/INTERLEAVE_N; n++) {
		result = interleave(INTERLEAVE_N, INTERLEAVE_WIDTH, &img[n*INTERLEAVE_N], &result_length);
		free(result);
	}
	
	gettimeofday(&end, NULL);
	ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    s = ms/1e6;
 	
 	printf("Interleaving completed in %f s\n", s);
 	printf("Interleaving completed successfully!\n\n");

/* ============================================================================================ */

 	printf("Interleaving w/o packing\n");
 	gettimeofday(&start, NULL);
 	byte_t** all_results_no_pack = malloc(sizeof(byte_t*) * NOF_IMAGES/INTERLEAVE_N);
	all_results_no_pack[0] = interleave_no_pack(INTERLEAVE_N, INTERLEAVE_WIDTH, img, &result_length);
	for (int n = 1; n < NOF_IMAGES/INTERLEAVE_N; n++) {
		all_results_no_pack[n] = interleave_no_pack(INTERLEAVE_N, INTERLEAVE_WIDTH, &img[n*INTERLEAVE_N], &result_length);
	}
	gettimeofday(&end, NULL);
	ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    s = ms/1e6;
 	printf("Interleaving completed in %f s\n", s);
 	printf("Interleaving w/o packing completed successfully!\n");

 /* ============================================================================================ */

 	printImg(img[69996]);
 	printImg(img[69997]);
 	printImg(img[69998]);
 	printImg(img[69999]);
	
	printInterleavedImg(all_results_no_pack[17499], INTERLEAVE_N);

}

byte_t* interleave(int n, int iw, byte_t** img, int* result_length) {
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

	byte_t byte, pixel;
	for (int i = 0; i < (n*IMG_SIZE)/8; i++) {
		byte = 0;
		for (int j = 0; j < 8; j++) {
			pixel = temp_result[i*8 + j];
			pixel = (pixel >= THRESHOLD) ? 1 : 0;
			byte |= (pixel << (7 - i));
		}
		result[i] = byte;
	}

	free(temp_result);

	*result_length = n*IMG_SIZE/8;
	return result;
}

byte_t* interleave_no_pack(int n, int iw, byte_t** img, int* result_length) {
	byte_t* temp_result = malloc(n * IMG_SIZE * sizeof(byte_t));

	for (int i = 0, i_n = 0; i < n*IMG_SIZE; i+=n*iw, i_n+=iw) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < iw; k++) {
				temp_result[i + j*iw + k] = img[j][i_n + k];
			}
		}
	}

	*result_length = n*IMG_SIZE;
	return temp_result;
}

void printImg(byte_t* img) {
	int px;
	for (int x = 0; x < IMG_X; x++) {
		for (int y = 0; y < IMG_Y; y++) {
			px = (img[x*IMG_X+y] >= THRESHOLD) ? 1 : 0;
			fprintf(stdout, "%d ", px);
		}
		fprintf(stdout, "\n");
	}
	printf("\n");
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


