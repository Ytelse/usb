#ifndef __FPGA_COMM_H_
#define __FPGA_COMM_H_

/* Pass a pointer to a struct containing all necessary data for the communication */
void* fpga_runloop(void* usb_device_struct_void_ptr);


#endif /* __FPGA_COMM_H_ */

