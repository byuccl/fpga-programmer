/*
 * intervalTimer.c
 *
 *  Created on: Apr 2, 2014
 *      Author: hutch
 */
#include <stdio.h>
#include <stdint.h>
#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"

#define INTERVAL_TIMER_STATUS_OK 1        // The function will return this if it was successful.
#define INTERVAL_TIMER_STATUS_FAIL 0      // The function will return this if it fails.

// Register Offsets. Leave these in the .h file so I can write macros for speed.
#define INTERVAL_TIMER_TCSR0_REG_OFFSET 0x00    // Offset to the status/control reg. for lower counter.
#define INTERVAL_TIMER_TLR0_REG_OFFSET  0x04    // Offset to the load register for the lower counter.
#define INTERVAL_TIMER_TCR0_REG_OFFSET  0x08    // Offset to the lower counter.
#define INTERVAL_TIMER_TCSR1_REG_OFFSET 0x10    // Offset to the status/control reg. for the upper counter.
#define INTERVAL_TIMER_TLR1_REG_OFFSET  0x14    // Offset to the load register for the upper counter.
#define INTERVAL_TIMER_TCR1_REG_OFFSET  0x18    // Offset to the upper counter.

// Register Bit Masks. Only the commented values are actually used.
// The single on-bit indicates the position of the corresponding bit in the register.
// Please see the Xilinx documentation for the axi_timer for details.
#define INTERVAL_TIMER_TSCR0_CASC_BIT_MASK  0x0800  // cascade bit.
#define INTERVAL_TIMER_TSCR0_ENALL_BIT_MASK 0x0400  // enable bit.
#define INTERVAL_TIMER_TSCR0_PWMA0_BIT_MASK 0x0200  // PWM-specific.
#define INTERVAL_TIMER_TSCR0_TOINT_BIT_MASK 0x0100  // interrupt-related.
#define INTERVAL_TIMER_TSCR0_ENT0_BIT_MASK  0x0080  // unused
#define INTERVAL_TIMER_TSCR0_ENIT0_BIT_MASK 0x0040  // unused
#define INTERVAL_TIMER_TSCR0_LOAD0_BIT_MASK 0x0020  // load bit
#define INTERVAL_TIMER_TSCR0_ARHT0_BIT_MASK 0x0010  // unused.
#define INTERVAL_TIMER_TSCR0_CAPT0_BIT_MASK 0x0008  // unused.
#define INTERVAL_TIMER_TSCR0_GENT0_BIT_MASK 0x0004  // unused.
#define INTERVAL_TIMER_TSCR0_UDT0_BIT_MASK  0x0002  // updown bit.
#define INTERVAL_TIMER_TSCR0_MDT0_BIT_MASK  0x0001  // unused.

// Same defines as above for second counter.
#define INTERVAL_TIMER_TSCR1_ENALL_BIT_MASK 0x400
#define INTERVAL_TIMER_TSCR1_PWMA0_BIT_MASK 0x200
#define INTERVAL_TIMER_TSCR1_T1INT_BIT_MASK 0x100
#define INTERVAL_TIMER_TSCR1_ENT1_BIT_MASK  0x080
#define INTERVAL_TIMER_TSCR1_ENIT1_BIT_MASK 0x040
#define INTERVAL_TIMER_TSCR1_LOAD1_BIT_MASK 0x020
#define INTERVAL_TIMER_TSCR1_ARHT1_BIT_MASK 0x010
#define INTERVAL_TIMER_TSCR1_CAPT1_BIT_MASK 0x008
#define INTERVAL_TIMER_TSCR1_GENT1_BIT_MASK 0x004
#define INTERVAL_TIMER_TSCR1_UDT1_BIT_MASK  0x002
#define INTERVAL_TIMER_TSCR1_MDT1_BIT_MASK  0x001

#define COUNTER_RESET_VALUE 0

// The TSCR0/1 bit-pattern that I want to use to start the timer.
#define INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND (INTERVAL_TIMER_TSCR0_CASC_BIT_MASK | INTERVAL_TIMER_TSCR0_ENT0_BIT_MASK)
// The TSCR0 bit-pattern to stop the timer.
#define INTERVAL_TIMER_64_BIT_COUNTER_STOP_COMMAND (INTERVAL_TIMER_TSCR0_CASC_BIT_MASK)

// Utility defines
#define INTERVAL_TIMER_MAX_TIMER_INDEX 2  // Maximum number of timers, zero-based.


// Returns the base address of the timer based upon xparameters.h and the timerNumber parameter.
uint32_t intervalTimer_getTimerBaseAddress(uint32_t timerNumber) {
  uint32_t timerBaseAddress = 0;  // Base address value will be here.
  switch (timerNumber) {          // Just switch based upon the timer number.
  case INTERVAL_TIMER_TIMER_0:    // timer 0.
    timerBaseAddress = XPAR_AXI_TIMER_0_BASEADDR; // address comes from the xparameters.h file.
    break;
  case INTERVAL_TIMER_TIMER_1:  // timer 1.
    timerBaseAddress = XPAR_AXI_TIMER_1_BASEADDR;  // address comes from the xparameters.h file.
    break;
  case INTERVAL_TIMER_TIMER_2:  // timer 2.
    timerBaseAddress = XPAR_AXI_TIMER_2_BASEADDR; // address comes from the xparameters.h file.
    break;
  default:
    // Always do some error checking.
    printf("intervalTimer_getTimerBaseAddress: timerNumber (%u) is not between 0 and %d.\n\r",
        timerNumber,
        INTERVAL_TIMER_MAX_TIMER_INDEX);  // Tell the user what the max index is.
  }
  return timerBaseAddress;
}

// Takes a timer number, register offset, and value and writes value to register.
uint32_t intervalTimer_writeRegister(uint32_t timerNumber, uint32_t registerOffset, uint32_t value) {
  uint32_t timerBaseAddress = intervalTimer_getTimerBaseAddress(timerNumber);   // Compute the address
  Xil_Out32((timerBaseAddress+registerOffset), value);  // Write the value to the address.
  return 0;
}

// Takes a timer number, register offset and returns the value stored in the register.
uint32_t intervalTimer_readRegister(uint32_t timerNumber, uint32_t registerOffset) {
  uint32_t timerBaseAddress = intervalTimer_getTimerBaseAddress(timerNumber);   // Compute the address.
  return Xil_In32((timerBaseAddress+registerOffset));   // Read the value from the address.
}

// Enables all counters. For cascade mode, enable both counters.
// uint32_t intervalTimer_start(uint32_t timerNumber){
void intervalTimer_start(uint32_t timerNumber) {
  // Use the start-command defined in the .h file.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR0_REG_OFFSET, INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND);
}

// timerNumber parameter indicates which timer on which to operate.
// Stop counter 0 (this assumes that we are in CASCADE mode (64-bit), so stops all counters.
// In cascade mode, you only need to enable ENT0 bit in the TSCRO to start the 64-bit counter.
// uint32_t intervalTimer_stop(uint32_t timerNumber) {
void intervalTimer_stop(uint32_t timerNumber) {
    // Use the bit-pattern defined in the .h file.
	intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR0_REG_OFFSET, INTERVAL_TIMER_64_BIT_COUNTER_STOP_COMMAND);
}

// Makes sure the timer is off and then reset both 32-bit counters by loading a 0 into them.
// timerNumer indicates the which timer to use.
//uint32_t intervalTimer_reset(uint32_t timerNumber) {
void intervalTimer_reset(uint32_t timerNumber) {
  // Load 0 in to the counter 0 load register.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, COUNTER_RESET_VALUE);
  // Load 0 in to the counter 1 load register.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR1_REG_OFFSET, COUNTER_RESET_VALUE);
  // Load the contents of load register 0 into counter 0.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR0_REG_OFFSET, INTERVAL_TIMER_TSCR0_LOAD0_BIT_MASK);
  // Load the contents of load register 1 into counter 1.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR1_REG_OFFSET, INTERVAL_TIMER_TSCR1_LOAD1_BIT_MASK);
  // Reinitialize the timer.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR0_REG_OFFSET, COUNTER_RESET_VALUE);
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR1_REG_OFFSET, COUNTER_RESET_VALUE);
  // Cascade mode, up count.
  intervalTimer_writeRegister(timerNumber,
                INTERVAL_TIMER_TCSR0_REG_OFFSET,
                INTERVAL_TIMER_TSCR0_CASC_BIT_MASK | INTERVAL_TIMER_TSCR0_UDT0_BIT_MASK);
}

// This function checks to see if the timer hardware is present by reading and
// writing a couple of registers.
#define LOAD_REGISTER0_TEST_VALUE 0xF0F0F0F0
#define LOAD_REGISTER1_TEST_VALUE 0x0F0F0F0F
intervalTimer_status_t intervalTimer_checkTimerHardwarePresent(uint32_t timerNumber) {
  // Write then read the load registers to see if hardware is present at the
  // timer address. These registers will work because they are readable/writeable.
  // Writing two registers with different values will catch the situation where you are
  // writing the same register twice, in case the hardware is not what it should be.
  // Write load register 0.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, LOAD_REGISTER0_TEST_VALUE);
  // Write load register 1.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR1_REG_OFFSET, LOAD_REGISTER1_TEST_VALUE);
  // Read back both registers and compare to written values to see if they match.
  if ((intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET) != LOAD_REGISTER0_TEST_VALUE) ||
      (intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TLR1_REG_OFFSET) != LOAD_REGISTER1_TEST_VALUE)) {
    return INTERVAL_TIMER_STATUS_FAIL;  // Return a failing status if the values don't match.
  }
  // Zero out both load registers so the test values are overwritten.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, COUNTER_RESET_VALUE);
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR1_REG_OFFSET, COUNTER_RESET_VALUE);
  return INTERVAL_TIMER_STATUS_OK; // Hardware is present if you get here.
}

// Must call this before using a timer.
// Returns a INTERVAL_TIMER_STATUS_OK if successful, INTERVAL_TIMER_STATUS_FAIL if fails.
// Don't proceed to use the intervalTimer functions if the init fails.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  intervalTimer_status_t status = intervalTimer_checkTimerHardwarePresent(timerNumber);
  if (status != INTERVAL_TIMER_STATUS_OK) {
    printf("intervalTimer_init(): ERROR: hardware not present!\n\r"); // Print error message if status failed.
    return status;  // Just return as the rest of the init is meaningless if the hardware is not present.
  }
  // Configure the timer as follows.
  // Restore both control registers to zero.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR0_REG_OFFSET, COUNTER_RESET_VALUE);
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR1_REG_OFFSET, COUNTER_RESET_VALUE);
  // Cascade mode, up count.
  intervalTimer_writeRegister(timerNumber,
							  INTERVAL_TIMER_TCSR0_REG_OFFSET,
							  INTERVAL_TIMER_TSCR0_CASC_BIT_MASK | INTERVAL_TIMER_TSCR0_UDT0_BIT_MASK);
  return INTERVAL_TIMER_STATUS_OK;
}

// Just calls intervalTimer_init on all timers.
//uint32_t intervalTimer_initAll() {
intervalTimer_status_t intervalTimer_initAll() {
  int i;
  intervalTimer_status_t status = INTERVAL_TIMER_STATUS_OK;
  for (i=0; i<=INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    status &= intervalTimer_init(i);
  }
  return status;
}

// Just calls intervalTimer_reset on all timers.
void intervalTimer_resetAll() {
  int i;
  for (i=0; i<=INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    intervalTimer_reset(i);
  }
}

#define TEST_LOAD_VALUE 0x55AA
// Personal test code.
// This will configure the counter, start it, see if it runs, stop it, see if it stays stopped.
// Pretty good way to test for the existence of the counter hardware.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {
  uint32_t errorValue = INTERVAL_TIMER_STATUS_OK;
  // See if the hardware appears to be present by writing a value to the load register and reading it back.
  uint32_t loadValue = TEST_LOAD_VALUE;
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, loadValue);
  // If the hardware is not present, you won't read the value that your wrote.
  if (loadValue != intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET)) {
    printf("intervalTimer_test: hardware for timer: %u does not appear to be present.\n\r", timerNumber);
  }
  intervalTimer_init(timerNumber);  // Init the timer.
  intervalTimer_reset(timerNumber); // Reset the timer to 0.
  uint32_t counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
  // See if the timer is reset. Timer should be zero at this point.
  if (counter0Value) {
    printf("intervalTimer_test: counter 0 should be zero after initialization. Value is: %u.\n", counter0Value);
    errorValue = INTERVAL_TIMER_STATUS_FAIL;
  } else {
	// OK, see if the timer will run.
    intervalTimer_start(timerNumber);
    volatile int i;  // Wait for a bit.
    for (i=0; i<1000; i++);
    // Read the lower counter.
    counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
    if (!counter0Value) {
	  printf("intervalTimer_test: counter 0 does not seem to be running. Value is: %u.\n\r", counter0Value);
	  errorValue = INTERVAL_TIMER_STATUS_FAIL;
    } else {
      // OK. See if the timer will stop.
      intervalTimer_stop(timerNumber);
      counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
      for (i=0; i<1000; i++);
      uint32_t secondCounter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
      if (counter0Value != secondCounter0Value) {
  	    printf("intervalTimer_test: counter 0 does not seem to be stopped.\n\r");
	    errorValue = INTERVAL_TIMER_STATUS_FAIL;
      }
    }
  }
  return errorValue;
}

// Just calls intervalTimer_test on all timers.
intervalTimer_status_t intervalTimer_testAll() {
  intervalTimer_status_t error = INTERVAL_TIMER_STATUS_OK;
  int i;
  for (i=0; i<=INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    if (intervalTimer_test(i))
      error = INTERVAL_TIMER_STATUS_FAIL;
  }
  return error;
}

// Returns the frequency for each timer (from xparameters.h).
uint32_t intervalTimer_getTimerFrequency(uint32_t timerNumber) {
  switch(timerNumber) {
  case 0:
	  return XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;    // timer-0 frequency. all same but use the distinct values from xparameters.h
  case 1:
	  return XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;    // timer-1 frequency.
  case 2:
	  return XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ;    // timer-2 frequency.
  default:
	printf("intervalTimer_getTimerFrequency: timerNumber (%d) is not between 0 and %d.\n\r",
			timerNumber, INTERVAL_TIMER_MAX_TIMER_INDEX);
	return 0;
  }
}

// This routine assumes that the counter may be running. This could be simplified if you will only
// invoke this function when the counter is not running.
// timerNumber indicates the timer of interest.
uint64_t intervalTimer_get64BitCounterValue(uint32_t timerNumber) {
  // Read the upper counter.
  uint32_t upperCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR1_REG_OFFSET);
  // Read the lower counter.
  uint32_t lowerCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
  // Read the upper counter again.
  uint32_t secondUpperCounterValue = intervalTimer_readRegister(timerNumber,INTERVAL_TIMER_TCR1_REG_OFFSET);
  // If the upper counter has changed, reread the lower counter value because it probably rolled over.
  if (upperCounterValue != secondUpperCounterValue) {
    lowerCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
    upperCounterValue = secondUpperCounterValue;
  }
  uint64_t counterValue = upperCounterValue;
  counterValue <<= 32;
  counterValue |= lowerCounterValue;
  return counterValue;
}

// Just returns the lower 32 bit value of the counter. Useful for generating random number generator seeds.
uint32_t intervalTimer_getLower32BitCounterValue(uint32_t timerNumber) {
	return intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
}

// Use a separate return value to indicate an error, if you like.
//uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double *seconds) {
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  double timerFrequency = intervalTimer_getTimerFrequency(timerNumber);   // Get the frequency.
  double counterValue = intervalTimer_get64BitCounterValue(timerNumber);  // Get the 64-bit value.
  double seconds = counterValue / timerFrequency; // Compute the duration in seconds.
  return seconds; // Just return the value.
}

//#define INTERVAL_TIMER_MILESTONE_2
#ifdef INTERVAL_TIMER_MILESTONE_2
#include "supportFiles/intervalTimer.h"
#include "supportFiles/buttons.h"
#include "supportFiles/utils.h"
#include <stdio.h>

#define TEST_ITERATION_COUNT 4
#define ONE_SECOND_DELAY 1000
int main() {
  double duration0, duration1, duration2;  // Will hold the duration values for the various timers.
  buttons_init();           // init the buttons package.
  intervalTimer_initAll();  // init all of the interval timers.
  intervalTimer_resetAll(); // reset all of the interval timers.
  // Poll the push-buttons waiting for BTN0 to be pushed.
  printf("Interval Timer Accuracy Test\n\r");     // User status message.
  printf("waiting until BTN0 is pressed.\n\r");   // Tell user what you are waiting for.
  while (!(buttons_read() & BUTTONS_BTN0_MASK));  // Loop here until BTN0 pressed.
  // Start all of the interval timers.
  intervalTimer_start(INTERVAL_TIMER_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_TIMER_2);
  printf("started timers.\n\r");
  printf("waiting until BTN1 is pressed.\n\r");  // Poll BTN1.
  while (!(buttons_read() & BUTTONS_BTN1_MASK)); // Loop here until BTN1 pressed.
  // Stop all of the timers.
  intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
  printf("stopped timers.\n\r");
  // Get the duration values for all of the timers.
  duration0 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0);
  duration1 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1);
  duration2 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2);
  // Print the duration values for all of the timers.
  printf("Time Duration 0: %6.2e seconds.\n\r", duration0);
  printf("Time Duration 1: %6.2e seconds.\n\r", duration1);
  printf("Time Duration 2: %6.2e seconds.\n\r", duration2);
  // Now, test to see that all timers can be restarted multiple times.
  printf("Iterating over fixed delay tests\n\r");
  printf("Delays should approximately be: 1, 2, 3, 4 seconds.\n\r");
  for (int i=0; i<TEST_ITERATION_COUNT; i++) {
    // Reset all the timers.
    intervalTimer_resetAll();
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i+1)*ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    // Print the duration of all of the timers. The delays should be approximately 1, 2, 3, and 4 seconds.
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_0, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_1, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_2, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }
  // Now, test for increment timing (start-stop-start-stop...)
  // Reset all the timers.
  intervalTimer_resetAll();
  for (int i=0; i<TEST_ITERATION_COUNT; i++) {
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i+1)*ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    // Print the duration of all of the timers. The delays should be approximately 1, 3, 6, and 10 seconds.
    printf("Delays should approximately be: 1, 3, 6, 10 seconds.\n\r");
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_0, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_1, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_2, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }

  printf("intervalTimer Test Complete.\n\r");
}

void isr_function(){}
#endif


