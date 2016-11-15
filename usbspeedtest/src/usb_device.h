#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

#include "libusb.h"

#ifdef MCU
/* USB DEVICE DESCRIPTOR MCU */	
#define VENDOR_ID 	0x10c4
#define PRODUCT_ID 	0x0007
#define EP_IN 		0x81
#define EP_OUT 		0x01
#else
/* USB DEVICE DESCRIPTOR FPGA */	
#define VENDOR_ID 	0x8888
#define PRODUCT_ID 	0x0001
#define EP_IN		0x81 /* TODO: Get proper endpoint addresses */
#define EP_OUT		0x01
#endif

int connect(libusb_context* context, libusb_device_handle** dev_handle, int interface);

#endif