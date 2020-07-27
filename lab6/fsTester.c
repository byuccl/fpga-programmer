/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "fsTester.h"

#include <stdint.h>
#include <stdio.h>

#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "utils.h"

// This will set the sequence to a simple sequential pattern.
// It starts by flashing the first color, and then increments the index and
// flashes the first two colors and so forth. Along the way it prints info
// messages to the LCD screen.
#define TEST_SEQUENCE_LENGTH 8 // Just use a short test sequence.
uint8_t flashSequence_testSequence[TEST_SEQUENCE_LENGTH] = {
    SIMON_DISPLAY_REGION_0, SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_REGION_0}; // Simple sequence.
#define INCREMENTING_SEQUENCE_MESSAGE1 "Incrementing Sequence" // Info message.
#define RUN_TEST_COMPLETE_MESSAGE "Runtest() Complete"         // Info message.
#define MESSAGE_TEXT_SIZE 2    // Make the text easy to see.
#define TWO_SECONDS_IN_MS 2000 // Two second delay.
#define TICK_PERIOD 75         // 200 millisecond delay.
#define TEXT_ORIGIN_X 0
#define TEXT_ORIGIN_Y (DISPLAY_HEIGHT / 2)

// Make a simple state machine to exercise flashSequence.
// fsTester states.
typedef enum {
  fsTester_init_st,     // Start here.
  fsTester_flashing_st, // Flashing the sequence in this state.
  fsTester_msgDelay_st, // Wait here so user can read message.
  fsTester_done_st      // Go here when all done.
} fsTester_cs_t;

static fsTester_cs_t fsTester_currentState; // fsTester current state.
uint16_t sequenceLength;                    // Current sequence length.
uint16_t fsTester_msgDelayCount;            // Counter for a delay.

#define FS_EXER_MSG_DELAY 20 // Provides a delay to read message.

// fsTester must init the flashSequence state machine and set its initial state.
void fsTester_init() {
  // Setup the screen and font size.
  display_init();
  display_setTextSize(MESSAGE_TEXT_SIZE); // Use a standard text size.
  display_fillScreen(DISPLAY_BLACK);      // Clear the display.
  flashSequence_init();                   // obligatory init.
  // Set the provided test sequence.
  globals_setSequence(flashSequence_testSequence, TEST_SEQUENCE_LENGTH);
  // Start with the first element of the sequence.
  sequenceLength = 1;
  globals_setSequenceIterationLength(sequenceLength);
  // State machine starts in this state.
  fsTester_currentState = fsTester_init_st;
}

// print incrementing message if flag is true, erase if false.
static void fsTester_printIncrementMessage(bool draw) {
  if (draw)
    display_setTextColor(DISPLAY_WHITE);
  else
    display_setTextColor(DISPLAY_BLACK);
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y); // Roughly centered.
  display_println(INCREMENTING_SEQUENCE_MESSAGE1); // Print the message.
}

#define DRAW_MESSAGE true
#define ERASE_MESSAGE false

// tick the flash sequence test state machine
void fsTester_tick() {
  // transition.
  switch (fsTester_currentState) {
  case fsTester_init_st:
    //   Enable the state machine.
    fsTester_currentState = fsTester_flashing_st;
    fsTester_msgDelayCount = 0;
    flashSequence_enable(); // Mealy action.
    break;
  case fsTester_flashing_st:
    // Wait for the sequence to complete.
    if (flashSequence_isComplete()) {
      // Sequence has completed, disable the flashSequence SM.
      flashSequence_disable(); // Mealy action.
      sequenceLength++;        // Increment the sequenceLength.
      // Finished the sequence yet?
      if (sequenceLength > TEST_SEQUENCE_LENGTH) {
        fsTester_currentState = fsTester_done_st; // Yes.
        // Set the cursor position.
        display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);
        // Print the ending message.
        display_setTextColor(DISPLAY_WHITE);
        display_println(RUN_TEST_COMPLETE_MESSAGE);
      } else {
        // Set the length of the pattern.
        globals_setSequenceIterationLength(sequenceLength);
        fsTester_currentState = fsTester_msgDelay_st; // No, keep going.
        fsTester_printIncrementMessage(DRAW_MESSAGE);
      }
    }
    break;
  case fsTester_msgDelay_st:
    // Provide a short delay so user can read message before it is erased.
    if (fsTester_msgDelayCount >= FS_EXER_MSG_DELAY) {
      fsTester_printIncrementMessage(ERASE_MESSAGE);
      fsTester_currentState = fsTester_init_st;
    } else {
      fsTester_msgDelayCount++;
    }
    break;
  case fsTester_done_st:
    // Just sit in this state.
    break;
  }
}
