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

#include "display.h"
#include "interrupts.h"
#include "leds.h"
#include "testBoards.h"
#include "ticTacToeControl.h"
#include "ticTacToeDisplay.h"
#include "utils.h"
#include "xparameters.h"

#define MILESTONE1 1
#define MILESTONE2 2
#define MILESTONE3 3

// Uncomment this line and update it to run one of the milestones
#define MILESTONE MILESTONE3

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
#define MILESTONE1_MESSAGE "Running testBoards()\n"
#define MILESTONE2_MESSAGE "Running ticTacToeDisplay_runTest()\n"
#define MILESTONE3_MESSAGE "Running tic-tac-toe game\n"

// Keep track of how many times isr_function() is called.
uint32_t isr_functionCallCount = 0;

#if MILESTONE == MILESTONE1
int main() {
  printf("%s", MILESTONE1_MESSAGE);
  testBoards();
}

#elif MILESTONE == MILESTONE2
int main() {
  printf("%s", MILESTONE2_MESSAGE);
  ticTacToeDisplay_runTest();
}

#elif MILESTONE == MILESTONE3
int main() {
  // Flag method

  // Initialize the GPIO LED driver and print out an error message if it fails
  // (argument = true). You need to init the LEDs so that LD4 can function
  // as a heartbeat.
  leds_init(true);

  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();

  // Initialization ticTacToe SM
  ticTacToeControl_init();

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
      ticTacToeControl_tick();
      interrupts_isrFlagGlobal = 0;
      if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
    }
  }
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n\r", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n\r", personalInterruptCount);
  return 0;
}

#endif

// Interrupt routine
void isr_function() {
  // Empty for now.
}