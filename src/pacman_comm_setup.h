#ifndef __PACMAN_COMM_SETUP_H_
#define __PACMAN_COMM_SETUP_H_

#include <libusb.h>
#include "defs.h"

/* Return codes */
enum {CONNECT_SUCCESS, CONNECT_FAILURE};

int connect(libusb_context* context,			/* In  */ /* libusb context */
			libusb_device_handle** dev_handle, 	/* Out */ /* Pointer to usb handle */
			pacman_device_t device,				/* In  */ /* Type of device to conenct to */
			int* interface);					/* Out */ /* Device interface claimed by host */

#endif /* __PACMAN_COMM_SETUP_H_ */