/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "clockControl.h"
#include "clockDisplay.h"
#include "display.h"
#include "interrupts.h"
#include "leds.h"
#include "utils.h"
#include "xparameters.h"

#define MILESTONE_1 1
#define MILESTONE_2 2

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the following lines to run Milestone 1 or 2      ///////////
////////////////////////////////////////////////////////////////////////////////
// #define RUN_PROGRAM MILESTONE_1
// #define RUN_PROGRAM MILESTONE_2

// If nothing is uncommented above, run milestone 2
#ifndef RUN_PROGRAM
#define RUN_PROGRAM MILESTONE_2
#endif

#define RUN_DISPLAY_TEST_MSG "Running Milestone 1: clockDisplay_test()\n"
#define RUN_MILESTONE_2_MSG "Running Milestone 2: full clock lab\n"

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

// Keep track of how many times isr_function() is called.
uint32_t isr_functionCallCount = 0;

// This main uses isr_function() to invoked clockControl_tick().
int main() {
#if (RUN_PROGRAM == MILESTONE_1)
  printf(RUN_DISPLAY_TEST_MSG);
  clockDisplay_runTest();

#elif (RUN_PROGRAM == MILESTONE_2)
  // This main() uses the flag method to invoke clockControl_tick().

  printf(RUN_MILESTONE_2_MSG);
  // Initialize the GPIO LED driver and print out an error message if it fails
  // (argument = true). You need to init the LEDs so that LD4 can function as
  // a heartbeat.
  leds_init(true);
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the
  // argument = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();
  // Initialization of the clock display is not time-dependent, do it outside
  // of the state machine.
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
      if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
      fflush(stdout);
    }
  }
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n", personalInterruptCount);
#endif
  return 0;
}

// Keep this empty
// The 'interrupts_isrFlagGlobal' flag will be automatically set on an interrupt
// behind the scenes.  We don't need to set it here.
void isr_function() {}
