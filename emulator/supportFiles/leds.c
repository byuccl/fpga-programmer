/*
 * leds.c
 *
 *  Created on: Mar 24, 2014
 *      Author: hutch
 */

#include "leds.h"
#include "mio.h"

#include "stdio.h"
#include "xparameters.h"
#include "xil_io.h"

static int initLedFlag = 0;  // This will be '1' if LEDs have been initialized.

#define LD3_LD2_LD1_LD0_ON 0xF
#define LD3_LD2_LD1_LD0_OFF 0x0
#define LED_LD4_ON 1
#define LED_LD4_OFF 0

#define GPIO_DIRECTION_IS_OUTPUT 0x0
#define GPIO_DIRECTION_IS_INPUT 0xF
#define GPIO_CHANNEL_NUMBER 1

#define STATUS_OK 0
#define STATUS_FAIL 1

#define LEDS_GPIO_DEVICE_BASE_ADDRESS XPAR_LEDS_BASEADDR
#define GPIO_DATA_REGISTER 0
#define GPIO_TRI_REGISTER 4

void leds_writeGpioRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(LEDS_GPIO_DEVICE_BASE_ADDRESS + offset, value);
}

uint32_t leds_readGpioRegister(uint32_t offset) {
  return Xil_In32(LEDS_GPIO_DEVICE_BASE_ADDRESS + offset);
}

// This will init the GPIO hardware so you can write to the 4 LEDs  (LD3 - LD0) on the ZYBO board.
// if printFailedStatusFlag = 1, it will print out an error message if an internal error occurs.
int leds_init(bool printFailedStatusFlag) {
  // Only invoke this function once.
  if (initLedFlag)
    return STATUS_OK;
  // Just set the direction for the GPIO outputs.
  leds_writeGpioRegister(GPIO_TRI_REGISTER, GPIO_DIRECTION_IS_OUTPUT);
  return STATUS_OK;
}

// This write the lower 4 bits of ledValue to the LEDs.
// LED3 gets bit3 and so forth.
// '1' = illuminated.
// '0' = off.
void leds_write(int ledValue) {
  leds_writeGpioRegister(GPIO_DATA_REGISTER, ledValue);
}

// Access LD4 via MIO.
void leds_writeLd4(int ledValue) {
  // Add mio stuff later to emulate hardware.
//  global_getEmulator()->writeLd4(ledValue);
}

// This blinks all of the LEDs for several seconds to provide a visual test of the code.
// This will use a simple for-loop as a delay to keep the code independent of other code.
// Always returns 0 because this is strictly a visual test.
#define LEDS_TEST_LENGTH_IN_SECONDS 5
#define LEDS_FOR_LOOP_COUNT_FOR_ONE_BLINK 1000000
int leds_runTest() {
  leds_init(true);
  int i, j;
  for (i=0; i<LEDS_TEST_LENGTH_IN_SECONDS; i++) {
	  for (j=0; j<LEDS_FOR_LOOP_COUNT_FOR_ONE_BLINK; j++) {
	    leds_write(LD3_LD2_LD1_LD0_ON);  // All LEDs are turned on.
	    leds_writeLd4(LED_LD4_ON);
	  }
	  for (j=0; j<LEDS_FOR_LOOP_COUNT_FOR_ONE_BLINK; j++) {
	    leds_write(LED_LD4_OFF);  // All LEDs are turned on.
	    leds_writeLd4(LD3_LD2_LD1_LD0_OFF);
	  }
  }
  return 0;
}








