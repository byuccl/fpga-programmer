/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdint.h>
#include <stdio.h>
#include <xparameters.h>

#include "bhTester.h"
#include "buttonHandler.h"
#include "config.h"
#include "display.h"
#include "flashSequence.h"
#include "fsTester.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include "simonControl.h"
#include "simonDisplay.h"
#include "utils.h"
#include "verifySequence.h"
#include "vsTester.h"

#define MILESTONE_1 1
#define MILESTONE_2 2
#define MILESTONE_3 3
#define MILESTONE_4 4

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the following lines to run Milestone 1, 2, 3, or 4    //////
////////////////////////////////////////////////////////////////////////////////
// #define RUN_PROGRAM MILESTONE_1
// #define RUN_PROGRAM MILESTONE_2
// #define RUN_PROGRAM MILESTONE_3
// #define RUN_PROGRAM MILESTONE_4

// If nothing is uncommented above, run milestone 4
#ifndef RUN_PROGRAM
#define RUN_PROGRAM MILESTONE_4
#endif

// Compute the timer clock freq.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
// Compute timer load value.
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)
#define FOREVER 1 // syntactic sugar for while-forever loop.

// Code below is associated with the tick timer that is not
// currently used in this main().

// The max duration across all ticks.
static double maxDuration_g;
// Name of the function with longest duration is here.
static const char *maxDurationFunctionName_g;

#define MAX_DURATION_TIMER_INDEX 2

// Use this function to time each tick.
typedef void(functionPointer_t)();
#define SIMON_CONTROL_TICK "simonControl_tick()"
#define VERIFY_SEQUENCE_TICK "verifySequence_tick()"
#define FLASH_SEQUENCE_TICK "flashSequency_tick()"
#define BUTTON_HANDLER_TICK "buttonHandler_tick()"
#define WIN_SCREEN_TICK "winScreen_tick()"

// This assumes that the interval timer was initialized.
static void tickTimer(functionPointer_t *fp, const char *functionName) {
  intervalTimer_reset(MAX_DURATION_TIMER_INDEX); // Reset the timer.
  intervalTimer_start(MAX_DURATION_TIMER_INDEX); // Start the timer.
  fp(); // Invoke the tick function passed in by argument.
  intervalTimer_stop(MAX_DURATION_TIMER_INDEX); // Stop the timer.
  double duration;                              // Keep track of tick duration.
  duration = intervalTimer_getTotalDurationInSeconds(
      MAX_DURATION_TIMER_INDEX);  // Get the duration.
  if (maxDuration_g < duration) { // Keep the max duration.
    maxDuration_g = duration;
    maxDurationFunctionName_g =
        functionName; // Keep track of which function had the max. duration.
  }
}

/**************** main() code starts here. ************************/

/****************************** RUN_BUTTON_HANDLER_TEST ****************/
#if RUN_PROGRAM == MILESTONE_1
static void test_init() {
  leds_init(true);
  bhTester_init();
  printf("Running the buttonHandler test.\n");
}

void isr_function() {
  buttonHandler_tick();
  bhTester_tick();
}
#endif

/****************************** RUN_SIMON_FLASH_SEQUENCE_TEST ****************/
#if RUN_PROGRAM == MILESTONE_2
static void test_init() {
  leds_init(true);
  fsTester_init();
  printf("Running the flashSequence test.\n");
}

void isr_function() {
  fsTester_tick();
  flashSequence_tick();
}

/*********************** RUN VERIFY SEQUENCE TEST ***************************/
#elif RUN_PROGRAM == MILESTONE_3
static void test_init() {
  vsTester_init();
  printf("Running the verifySequence test.\n");
}

void isr_function() {
  buttonHandler_tick();
  flashSequence_tick();
  verifySequence_tick();
  vsTester_tick();
}

/****************************** RUN_SIMON_GAME ****************/
#elif RUN_PROGRAM == MILESTONE_4
static void test_init() {
  display_init();
  leds_init(true);
  simonControl_init();
  buttonHandler_init();
  flashSequence_init();
  verifySequence_init();
  display_fillScreen(DISPLAY_BLACK);
  simonControl_enable();
  printf("Running the simon game.\n");
}

void isr_function() {
  simonControl_tick();
  buttonHandler_tick();
  flashSequence_tick();
  verifySequence_tick();
}
#endif

// All programs share the same main.
// Differences are limited to test_init() and isr_function().
int main() {
  test_init(); // Program specific.
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs if the argument
  // = true.
  interrupts_initAll(1);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
  interrupts_enableTimerGlobalInts();
  // Keep track of your personal interrupt count. Want to make sure that you
  // don't miss any interrupts.
  int32_t personalInterruptCount = 0;
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  // main() must NOT terminate or prints to the terminal will not work
  // on the board (emulator is OK).
  while (FOREVER) {
    utils_sleep();
  }
}
