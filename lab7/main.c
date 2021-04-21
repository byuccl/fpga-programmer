/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "buttons.h"
#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include "switches.h"
#include "utils.h"
#include "wamControl.h"
#include "wamDisplay.h"
#include "xparameters.h"

#define MILESTONE_1 1
#define MILESTONE_2 2

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the following lines to run Milestone 1 or 2    /////////////
////////////////////////////////////////////////////////////////////////////////
// #define RUN_PROGRAM MILESTONE_1
// #define RUN_PROGRAM MILESTONE_2

// If nothing is uncommented above, run milestone 2
#ifndef RUN_PROGRAM
#define RUN_PROGRAM MILESTONE_2
#endif

#define MILESTONE_1_MSG "Running Milestone 1, wamDisplay_runMilestone1_test()\n"
#define MILESTONE_2_MSG "Running Milestone 2, Whack-a-mole game\n"

// The formula for computing the load value is based upon the formula from 4.1.1
// (calculating timer intervals) in the Cortex-A9 MPCore Technical Reference
// Manual 4-2. Assuming that the prescaler = 0, the formula for computing the
// load value based upon the desired period is: load-value = (period *
// timer-clock) - 1
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define MAX_ACTIVE_MOLES 1 // Start out with this many moles.
#define MAX_MISSES 50      // Game is over when there are this many misses.
#define FOREVER 1          // Syntactic sugar for while (1) statements.
#define MS_PER_TICK                                                            \
  (TIMER_PERIOD * 1000) // Just multiply the timer period by 1000 to get ms.

#define SWITCH_VALUE_9 9 // Binary 9 on the switches indicates 9 moles.
#define SWITCH_VALUE_6 6 // Binary 6 on the switches indicates 6 moles.
#define SWITCH_VALUE_4 4 // Binary 9 on the switches indicates 4 moles.
#define SWITCH_MASK 0xf  // Ignore potentially extraneous bits.

// Mole count is selected by setting the slide switches. The binary value for
// the switches determines the mole count (1001 - nine moles, 0110 - 6 moles,
// 0100 - 4 moles). All other switch values should default to 9 moles).
void wamMain_selectMoleCountFromSwitches(uint16_t switchValue) {
  switch (switchValue & SWITCH_MASK) {
  case SWITCH_VALUE_9: // this switch pattern = 9 moles.
    wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
    break;
  case SWITCH_VALUE_6: // this switch pattern = 6 moles.
    wamDisplay_selectMoleCount(wamDisplay_moleCount_6);
    break;
  case SWITCH_VALUE_4: // this switch pattern = 4 moles.
    wamDisplay_selectMoleCount(wamDisplay_moleCount_4);
    break;
  default: // Any other pattern of switches = 9 moles.
    wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
    break;
  }
}

// States for the main wam state machine.
typedef enum {
  wamMain_init_st,
  wamMain_waitForTouch_st,
  wamMain_waitForNoTouch_st,
  wamMain_runGame_st,
  wamMain_gameOver_st
} wamMain_cs_t;

static wamMain_cs_t wamMain_currentState; // current state for main SM.
static uint32_t randomSeed; // Used to make the game seem more random.

void wamMain_tick() {
  switch (wamMain_currentState) {
  case wamMain_init_st:
    // Increment a random seed until touch to make each game different.
    randomSeed++;
    // Wait for user to touch screen.
    if (display_isTouched()) {
      // Set the random seed.
      wamControl_setRandomSeed(randomSeed);
      wamMain_currentState = wamMain_waitForNoTouch_st;
    }
    break;
  // Wait for user to stop touching the screen.
  case wamMain_waitForNoTouch_st:
    if (!display_isTouched()) {
      // User no longer touching screen, set up the game and
      // draw the mole board.
      wamMain_selectMoleCountFromSwitches(switches_read());
      wamDisplay_init();
      wamControl_init();
      wamControl_setMaxActiveMoles(MAX_ACTIVE_MOLES);
      wamControl_setMaxMissCount(MAX_MISSES);

      // Mole count selected via slide switches.
      // Disable interrupts, draw mole board, enable interrupts.
      interrupts_disableArmInts();
      wamDisplay_drawMoleBoard();
      interrupts_enableArmInts();
      wamControl_enable();
      wamMain_currentState = wamMain_runGame_st;
    }
    break;
  case wamMain_runGame_st:
    // Do nothing, just wait for the game to end or user to terminate game.
    if (wamControl_isGameOver()) {
      wamMain_currentState = wamMain_gameOver_st;
    } else if (buttons_read()) {
      wamControl_disable();
    }
    break;
  case wamMain_gameOver_st:
    wamControl_disable();
    interrupts_disableArmInts();
    wamDisplay_drawGameOverScreen(); // Draw the game-over screen.
    wamDisplay_resetAllScoresAndLevel();
    interrupts_enableArmInts();
    wamMain_currentState = wamMain_init_st;
    break;
  default:
    // do nothing
    break;
  }
}

int main() {
#if RUN_PROGRAM == MILESTONE_1
  printf(MILESTONE_1_MSG);
  wamDisplay_runMilestone1_test();
#elif RUN_PROGRAM == MILESTONE_2
  printf(MILESTONE_2_MSG);
  switches_init(); // Init the slide switches.
  buttons_init();  // Init the push buttons.
  // You need to init the LEDs so that LD4 can function as a heartbeat.
  leds_init(true);
  wamMain_selectMoleCountFromSwitches(switches_read());
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true); // Init the interrupt code.
  interrupts_setPrivateTimerLoadValue(
      TIMER_LOAD_VALUE);              // Set the timer period.
  interrupts_enableTimerGlobalInts(); // Enable interrupts at the timer.
  interrupts_startArmPrivateTimer();  // Start the private ARM timer running.
  display_init(); // Init the display (make sure to do it only once).
  wamControl_setMaxActiveMoles(
      MAX_ACTIVE_MOLES); // Start out with this many simultaneous active moles.
  wamControl_setMaxMissCount(
      MAX_MISSES); // Allow this many misses before ending game.
  wamControl_setMsPerTick(
      MS_PER_TICK);              // Let the controller know how ms per tick..
  wamDisplay_drawSplashScreen(); // Draw the game splash screen.
  wamControl_init();
  wamDisplay_init();
  wamMain_currentState = wamMain_init_st;
  randomSeed = 0;
  utils_msDelay(500);
  interrupts_enableArmInts();
  while (1) {
    utils_sleep();
  }
#endif
}

void isr_function() {
#if RUN_PROGRAM == MILESTONE_2
  wamMain_tick();
  wamControl_tick();
#endif
}
