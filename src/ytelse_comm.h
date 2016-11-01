#ifndef __EFM32_COMM_H_
#define __EFM32_COMM_H_

#include <libusb.h>

/* Error codes */
enum {YTELSE_DEVICE_USB_OPEN_SUCCESS, YTELSE_DEVICE_NOT_FOUND, YTELSE_DEVICE_USB_OPEN_FAILURE};

int get_ytelse_mcu_handle(libusb_context* context, libusb_device_handle** dev_handle);
/* Prototype for connecting to the FPGA */
int get_ytelse_fpga_handle(libusb_context* context, libusb_device_handle** dev_handle);

#endif /* __EFM32_COMM_H_ */