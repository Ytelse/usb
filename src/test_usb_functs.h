#ifndef __TEST_USB_FUNCTS_H_
#define __TEST_USB_FUNCTS_H_

#include <libusb.h>

void sendTick(libusb_context* context, libusb_device_handle* efm_handle);
void testRecv(libusb_context* context, libusb_device_handle* efm_handle);
void testSendRecv(libusb_context* context, libusb_device_handle* efm_handle, int num_messages);
void sendRecvWait(libusb_context* context, libusb_device_handle* efm_handle);
void receiveNMsgs(libusb_context* context, libusb_device_handle* efm_handle, int num_recvs);
void sendNTicks(libusb_context* context, libusb_device_handle* efm_handle, int num_ticks);

#endif /* __TEST_USB_FUNCTS_H_ */

