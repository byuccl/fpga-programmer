
#ifndef XIL_IO_H_
#define XIL_IO_H_

#include <stdint.h>

// These are the only two Xil functions supported by the emulator.
uint32_t Xil_In32(uint32_t Addr);
void Xil_Out32(uint32_t Addr, uint32_t Value);

#endif // XIL_IO_H_
