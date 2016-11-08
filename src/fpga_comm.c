#include <pthread.h>
#include "pthread_helper.h"

extern int _keepalive;
extern pthread_barrier_t ;

void * fpga_runloop(void* usb_device_struct_void_ptr) {
	/* TODO: Dereference the void ptr */
}