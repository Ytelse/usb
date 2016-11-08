#include <pthread.h>
#include "pthread_helper.h"

extern int _keepalive;
extern int _fpga_ready;

void * mcu_runloop(void* usb_device_struct_void_ptr) {
	/* TODO: Dereference the void ptr */
}