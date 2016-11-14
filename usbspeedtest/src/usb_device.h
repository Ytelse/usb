#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

#include "libusb.h"

/* USB DEVICE DESCRIPTOR */	
#define VENDOR_ID 	0x10c4
#define PRODUCT_ID 	0x0007

#define EP_IN 		0x81
#define EP_OUT 		0x01

int connect(libusb_context* context, libusb_device_handle** dev_handle, int interface);

#endif