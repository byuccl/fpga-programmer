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

#include "xil_io.h"
#include "xparameters.h"

#include "my_libs/buttons.h"
#include "my_libs/intervalTimer.h"
#include "utils.h"

#define MILESTONE_1 1
#define MILESTONE_2 2

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the following lines to run Milestone 1 or 2      ///////////
////////////////////////////////////////////////////////////////////////////////
// #define RUN_PROGRAM MILESTONE_1
// #define RUN_PROGRAM MILESTONE_2

#ifndef RUN_PROGRAM
#define RUN_PROGRAM MILESTONE_2
#endif

#define MILESTONE_1_MSG "Running milestone 1.\n"
#define MILESTONE_2_MSG "Running milestone 2.\n"

#define ROLLOVER_DELAY_IN_MS 45000

#define TCR0_OFFSET 0x08 // register offset for TCR0
#define TCR1_OFFSET 0x18 // register offset for TCR1

// Reads the timer1 registers based upon the offset.
uint32_t readTimer1Register(uint32_t registerOffset) {
  uint32_t address = XPAR_AXI_TIMER_0_BASEADDR +
                     registerOffset; // Add the offset to the base address.
  return Xil_In32(address);          // Read the register at that address.
}

#define SHORT_DELAY 10

// Milestone 1 test
void milestone1() {
  printf("=============== Starting milestone 1 ===============\n");
  intervalTimer_init(INTERVAL_TIMER_TIMER_0);  // Init timer 0.
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0); // Reset timer 0.
  // Show that the timer is reset.
  // Check lower register.
  printf("timer_0 TCR0 should be 0 at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  // Check upper register.
  printf("timer_0 TCR1 should be 0 at this point:%u\n",
         readTimer1Register(TCR1_OFFSET));
  intervalTimer_start(INTERVAL_TIMER_TIMER_0); // Start timer 0.
  // Show that the timer is running.
  printf("The following register values should be changing while reading "
         "them.\n");
  // Just checking multiple times to see if the timer is running.
  utils_msDelay(SHORT_DELAY);
  printf("timer_0 TCR0 should be changing at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  utils_msDelay(SHORT_DELAY);
  printf("timer_0 TCR0 should be changing at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  utils_msDelay(SHORT_DELAY);
  printf("timer_0 TCR0 should be changing at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  utils_msDelay(SHORT_DELAY);
  printf("timer_0 TCR0 should be changing at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  utils_msDelay(SHORT_DELAY);
  printf("timer_0 TCR0 should be changing at this point:%u\n",
         readTimer1Register(TCR0_OFFSET));
  // Wait about 2 minutes so that you roll over to TCR1.
  // If you don't see a '1' in TCR1 after this long wait you probably haven't
  // programmed the timer correctly.
  //  waitALongTime();
  printf("wait for awhile...\n");
  utils_msDelay(ROLLOVER_DELAY_IN_MS);
  // Check lower register.
  printf("timer_0 TCR0 value after wait:%u\n", readTimer1Register(TCR0_OFFSET));
  // Check upper register.
  printf("timer_0 TCR1 should have changed at this point:%u\n",
         readTimer1Register(TCR1_OFFSET));
}

#define TEST_ITERATION_COUNT 4
#define ONE_SECOND_DELAY 1000

// Mileston 2 test function
void milestone2() {
  printf("=============== Starting milestone 2 ===============\n");
  double duration0, duration1,
      duration2;  // Will hold the duration values for the various timers.
  buttons_init(); // init the buttons package.
  intervalTimer_initAll();  // init all of the interval timers.
  intervalTimer_resetAll(); // reset all of the interval timers.
  // Poll the push-buttons waiting for BTN0 to be pushed.
  printf("Interval Timer Accuracy Test\n");   // User status message.
  printf("waiting until BTN0 is pressed.\n"); // Tell user what you are
                                              // waiting for.
  do {
    utils_sleep();
  } while (!(buttons_read() & BUTTONS_BTN0_MASK));
  // Start all of the interval timers.
  intervalTimer_start(INTERVAL_TIMER_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_TIMER_2);
  printf("started timers.\n");
  printf("waiting until BTN1 is pressed.\n"); // Poll BTN1.
  do {
    utils_sleep();
  } while (
      !(buttons_read() & BUTTONS_BTN1_MASK)); // Loop here until BTN1 pressed.
  // Stop all of the timers.
  intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
  printf("stopped timers.\n");
  // Get the duration values for all of the timers.
  duration0 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0);
  duration1 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1);
  duration2 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2);
  // Print the duration values for all of the timers.
  printf("Time Duration 0: %6.2e seconds.\n", duration0);
  printf("Time Duration 1: %6.2e seconds.\n", duration1);
  printf("Time Duration 2: %6.2e seconds.\n", duration2);
  // Now, test to see that all timers can be restarted multiple times.
  printf("Iterating over fixed delay tests\n");
  printf("Delays should approximately be: 1, 2, 3, 4 seconds.\n");
  for (int8_t i = 0; i < TEST_ITERATION_COUNT; i++) {
    // Reset all the timers.
    intervalTimer_resetAll();
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i + 1) * ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    // Print the duration of all of the timers. The delays should be
    // approximately 1, 2, 3, and 4 seconds.
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_0,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_1,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_2,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }
  // Now, test for increment timing (start-stop-start-stop...)
  // Reset all the timers.
  intervalTimer_resetAll();
  for (int8_t i = 0; i < TEST_ITERATION_COUNT; i++) {
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i + 1) * ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    // Print the duration of all of the timers. The delays should be
    // approximately 1, 3, 6, and 10 seconds.
    printf("Delays should approximately be: 1, 3, 6, 10 seconds.\n");
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_0,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_1,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n", INTERVAL_TIMER_TIMER_2,
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }

  printf("intervalTimer Test Complete.\n");
}

// main executes both milestones.
int main() {

#if (RUN_PROGRAM == MILESTONE_1)
  printf(MILESTONE_1_MSG);
  milestone1(); // Execute milestone 1
#elif (RUN_PROGRAM == MILESTONE_2)
  printf(MILESTONE_2_MSG);
  milestone2(); // Execute milestone 2
#endif
  return 0;
}

// Interrupt Routine
void isr_function() {}