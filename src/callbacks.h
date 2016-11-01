#ifndef __CALLBACKS_H_
#define __CALLBACKS_H_

#include <libusb.h>
#include <stdbool.h>

extern bool pendingWrite, pendingReceive;

/* Generic callbacks for MCU transfers */

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer);
void LIBUSB_CALL mcu_dataSentCallback(struct libusb_transfer* transfer);

/* Generic callbacks for FPGA transfers (unimplemented) */

void LIBUSB_CALL fpga_dataReceivedCallback(struct libusb_transfer* transfer);
void LIBUSB_CALL fpga_dataSentCallback(struct libusb_transfer* transfer);

/* Test calback(s) */
void LIBUSB_CALL mcu_tickWriteFinishedCallback(struct libusb_transfer* transfer);

#endif