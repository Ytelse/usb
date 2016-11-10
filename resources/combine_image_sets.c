#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#define FILEPATH_LEARNING_SET "train-images-idx3-ubyte"
#define FILEPATH_TEST_SET "t10k-images-idx3-ubyte"
#define FILEPATH_OUTPUTFILE "mnist-ubyte-no-header"

typedef unsigned char byte_t;

int main(void) {
	struct timeval start, end;

	gettimeofday(&start, NULL);

	/* Open files */
	FILE *ls, *ts, *out;
	ls = fopen(FILEPATH_LEARNING_SET, "rb");
	assert(ls != NULL);
	ts = fopen(FILEPATH_TEST_SET, "rb");
	assert(ts != NULL);
	out = fopen(FILEPATH_OUTPUTFILE, "wb");
	assert(out != NULL);

	/* Skip header bytes */
	fseek(ls, 16, SEEK_SET);
	fseek(ts, 16, SEEK_SET);

	int count = 0;
	byte_t byte = 0;

	while (!feof(ls)) {
		byte = getc(ls);
		putc(byte, out);
		count++;
	}

	while (!feof(ts)) {
		byte = getc(ts);
		putc(byte, out);
		count++;
	}
	
	gettimeofday(&end, NULL);
 	long int ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    double s = ms/1e6;
 	printf("File I/O completed in %f s\n\n", s);


	printf("Bytecount = %d\n", count);
	printf("Imagecount = %d\n", count/784);

	fclose(ls);
	fclose(ts);
	fclose(out);
}