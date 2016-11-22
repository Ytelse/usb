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
#include <assert.h>

#define INPUT_FILE "bit_mnist_images"
#define NOF_IMAGES 3*100
#define IMG_SIZE 98
#define IMG_PR_TRANSFER 3
#define TIMEOUT 200

#define EP_IN 0x81 /* TODO: Get proper endpoint addresses */
#define EP_OUT 0x01

unsigned char images[NOF_IMAGES*IMG_SIZE];

extern int _keepalive;
extern barrier_t barrier;

void * fpga_runloop(void* pdata_void_ptr) {
  int rc;
  /* Cast void ptr back to original type */
  pdata_t* pdata = (pdata_t*) pdata_void_ptr;
  UNUSED(pdata);
  /* Array of images */

  debugprint("FPGA_comm: Reading MNIST data set into memory...", DEFAULT);

  /* Open the input file*/
  FILE* f;
  f = fopen(IMG_FP, "rb");
  assert(f);

  /* Read the data set */
  for (int i = 0; i < NOF_IMAGES; i++) {
    for (int j = 0; j < IMG_SIZE; j++) {
      images[i*IMG_SIZE + j] = getc(f);
    }
  }

  /* Sync up with MCU */
  barrier_wait(&barrier);

  /* Keep running until stop signal is issued*/
  while(_keepalive) {
    for (int i = 0; i < NOF_IMAGES; i+= IMG_PR_TRANSFER) {
      int xferred;
      libusb_bulk_transfer(pdata->dev_handle, EP_OUT, &images[i*IMG_SIZE], IMG_SIZE*IMG_PR_TRANSFER, &xferred, TIMEOUT);
    }
  }

  fclose(f);

  free(images);
}
