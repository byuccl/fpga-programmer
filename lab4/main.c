/*
 * clockMainIsrFunctionBasedInterrupts.c
 *
 *  Created on: Aug 19, 2015
 *      Author: hutch
 */

#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include "supportFiles/leds.h"
#include "supportFiles/utils.h"
#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ISR_INVOCATION_MESSAGE "Running the clock lab using isr_function()\n"
#define FLAG_INVOCATION_MESSAGE "Running the clock lab using the flag method\n"

// The formula for computing the load value is based upon the formula
// from 4.1.1
// (calculating timer intervals) in the Cortex-A9 MPCore Technical Reference
// Manual 4-2. Assuming that the prescaler = 0, the formula for computing the
// load value based upon the desired period is: load-value = (period *
// timer-clock) - 1
#define TIMER_PERIOD 50.0E-3
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define INTERRUPTS_PER_SECOND (1.0 / TIMER_PERIOD)
#define TOTAL_SECONDS 20
#define MAX_INTERRUPT_COUNT (INTERRUPTS_PER_SECOND * TOTAL_SECONDS)

// Comment out this line to use the flag method.
//#define CLOCK_ISR_FUNCTION_INTERRUPT_METHOD

// Uncomment this line to run clockDisplay_test().
//#define RUN_CLOCK_DISPLAY_TEST
#define RUN_DISPLAY_TEST_MSG "Running clockDisplay_test()\n"

// Keep track of how many times isr_function() is called.
uint32_t isr_functionCallCount = 0;

#ifdef CLOCK_ISR_FUNCTION_INTERRUPT_METHOD
// This main uses isr_function() to invoked clockControl_tick().
int main() {
#ifdef RUN_CLOCK_DISPLAY_TEST
  printf(RUN_DISPLAY_TEST_MSG);
  clockDisplay_runTest();
  return 0;
#endif // RUN_CLOCK_DISPLAY_TEST
  printf(ISR_INVOCATION_MESSAGE);
  // Initialize the GPIO LED driver and print out an error message if it fails
  // (argument = true). You need to init the LEDs so that LD4 can function as a
  // heartbeat.
  leds_init(true);
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  printf("timer load value:%d\n\r", (int32_t)TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();
  // Initialization of the clock display is not time-dependent, do it outside of
  // the state machine.
  clockDisplay_init();
  clockControl_init();
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  // Disabled this for the moment to reduce CPU load.
  // The while-loop just waits until the total number of timer ticks have
  // occurred before proceeding.
  while (1) {
    utils_sleep();
    if (isr_functionCallCount == MAX_INTERRUPT_COUNT)
      break;
  }
  // All done, now disable interrupts and print out the interrupt counts.
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n\r", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n\r", isr_functionCallCount);
  return 0;
}

// The clockControl_tick() function is now called directly by the timer
// interrupt service routine.
void isr_function() {
  clockControl_tick();     // tick gets called directly by the timer interrupt
                           // service routine.
  isr_functionCallCount++; // Keep track of how many times you are called to
                           // ensure that you don't miss interrupts.
}

#else
// This main() uses the flag method to invoke clockControl_tick().
int main() {
#ifdef RUN_CLOCK_DISPLAY_TEST
  printf(RUN_DISPLAY_TEST_MSG);
  clockDisplay_runTest();
  return 0;
#endif // RUN_CLOCK_DISPLAY_TEST
  printf(FLAG_INVOCATION_MESSAGE);
  // Initialize the GPIO LED driver and print out an error message if it fails
  // (argument = true). You need to init the LEDs so that LD4 can function as a
  // heartbeat.
  leds_init(true);
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
  interrupts_enableTimerGlobalInts();
  // Initialization of the clock display is not time-dependent, do it outside of
  // the state machine.
  clockDisplay_init();
  clockControl_init();
  // Keep track of your personal interrupt count. Want to make sure that you
  // don't miss any interrupts.
  int32_t personalInterruptCount = 0;
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  while (1) {
    if (interrupts_isrFlagGlobal) {
      // Count ticks.
      personalInterruptCount++;
      clockControl_tick();
      interrupts_isrFlagGlobal = 0;
      if (personalInterruptCount == MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
    }
  }
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n\r", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n\r", personalInterruptCount);
  return 0;
}

// Interrupt routine
void isr_function() {
  // Empty for now.
}

#endif