/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "vsTester.h"

#include <stdbool.h>
#include <stdint.h>

#include "buttonHandler.h"
#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "verifySequence.h"

#define MESSAGE_X 0
//#define MESSAGE_Y (display_width()/4)
#define MESSAGE_Y (display_height() / 2)
#define MESSAGE_TEXT_SIZE 1
//#define MESSAGE_STARTING_OVER
#define BUTTON_0 0 // Index for button 0
#define BUTTON_1 1 // Index for button 1
#define BUTTON_2 2 // Index for button 2
#define BUTTON_3 3 // Index for button 3

// Prints the instructions that the user should follow when
// testing the verifySequence state machine.
// Takes an argument that specifies the length of the sequence so that
// the instructions are tailored for the length of the sequence.
// This assumes a simple incrementing pattern so that it is simple to
// instruct the user.
void vsTester_printInstructions(uint8_t length, bool draw) {
  static uint8_t prevLengthValue = 0;
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
    prevLengthValue = length; // Keep track of length from last draw.
  } else {
    display_setTextColor(DISPLAY_BLACK);
    length = prevLengthValue; // Overwrite length for erase.
  }
  display_setTextSize(MESSAGE_TEXT_SIZE);  // Make it readable.
  display_setCursor(MESSAGE_X, MESSAGE_Y); // Rough center.
  // Print messages are self-explanatory, no comments needed.
  // These messages request that the user touch the buttons in a specific
  // sequence.
  display_print("Tap: ");
  //  display_print("\n");
  switch (length) {
  case 1:
    display_print("red");
    break;
  case 2:
    display_print("red, yellow ");
    break;
  case 3:
    display_print("red, yellow, blue ");
    break;
  case 4:
    display_print("red, yellow, blue, green ");
    break;
  default:
    break;
  }
}

// Just clears the screen and draws the four buttons used in Simon.
void vsTester_drawButtons() {
  // Draw the four buttons.
  simonDisplay_drawButton(BUTTON_0, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(BUTTON_1, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(BUTTON_2, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(BUTTON_3, SIMON_DISPLAY_DRAW);
}

// This will set the sequence to a simple sequential pattern.
#define MAX_TEST_SEQUENCE_LENGTH 4 // the maximum length of the pattern
uint8_t vsTester_testSequence[MAX_TEST_SEQUENCE_LENGTH] = {
    0, 1, 2, 3}; // A simple pattern.

// Increment the sequence length making sure to skip over 0.
// Used to change the sequence length during the test.
static int16_t incrementSequenceLength(int16_t sequenceLength) {
  int16_t value = (sequenceLength + 1) % (MAX_TEST_SEQUENCE_LENGTH + 1);
  if (value == 0)
    value++;
  return value;
}

// Used to select from a variety of informational messages.
typedef enum {
  user_time_out_e,       // means that the user waited too long to tap a color.
  user_wrong_sequence_e, // means that the user tapped the wrong color.
  user_correct_sequence_e, // means that the user tapped the correct sequence.
  user_quit_e,             // means that the user wants to quite.
  user_erase_e             // Just means that prev. message will be erased.
} vsTester_infoMessage_t;

// Prints out informational messages based upon a message type (see above).
void vsTester_printInfoMessage(vsTester_infoMessage_t messageType, bool draw) {
  // Setup text color, position and clear the screen.
  static vsTester_infoMessage_t prevType;
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
    prevType = messageType; // Keep track for future erase.
  } else {
    display_setTextColor(DISPLAY_BLACK);
    messageType = prevType; // Overwrite messageType for erase.
  }
  display_setCursor(MESSAGE_X, MESSAGE_Y);
  vsTester_infoMessage_t previousType;
  switch (messageType) {
  case user_time_out_e: // Tell the user that they typed too slowly.
    display_print("Error: too slow.");
    break;
  case user_wrong_sequence_e: // Tell the user that they tapped the wrong
                              // color.
    display_print("Error: wrong color.");
    break;
  case user_correct_sequence_e: // Tell the user that they were correct.
    display_print("Correct.");
    break;
  case user_quit_e: // Acknowledge that you are quitting the test.
    display_print("Quitting.");
    break;
  default:
    break;
  }
}

#define DRAW_MESSAGE true
#define ERASE_MESSAGE false
#define DELAY_COUNTER_EXPIRE 20
uint16_t delayCounter;

typedef enum {
  vsTester_init_st,
  vsTester_running_st,
  vsTester_delay_st,
  vsTester_quit_st,
  vsTester_done_st
} vsTester_state_t;
vsTester_state_t vsTester_currentState;
uint16_t sequenceLength;

void vsTester_init() {
  display_init();
  buttonHandler_init();
  flashSequence_init();
  verifySequence_init();
  vsTester_currentState = vsTester_init_st;
  sequenceLength = 1;
  delayCounter = 0;
  globals_setSequence(vsTester_testSequence, MAX_TEST_SEQUENCE_LENGTH);
  globals_setSequenceIterationLength(sequenceLength);
  display_fillScreen(DISPLAY_BLACK);
  vsTester_printInstructions(sequenceLength, DRAW_MESSAGE);
}

void vsTester_tick() {
  switch (vsTester_currentState) {
  case vsTester_init_st:
    if (delayCounter < DELAY_COUNTER_EXPIRE) {
      delayCounter++;
    } else {
      delayCounter = 0;
      verifySequence_enable();
      vsTester_printInstructions(sequenceLength, ERASE_MESSAGE);
      vsTester_drawButtons();
      vsTester_currentState = vsTester_running_st;
    }
    break;
  case vsTester_running_st:
    if (verifySequence_isComplete()) {
      sequenceLength++;
      if (verifySequence_isTimeOutError()) { // Was the user too slow?
        // Yes, tell the user that they were too slow.
        vsTester_printInfoMessage(user_time_out_e, DRAW_MESSAGE);
        // Did the user tap the wrong color?
      } else if (verifySequence_isUserInputError()) {
        // Yes, tell them so.
        vsTester_printInfoMessage(user_wrong_sequence_e, DRAW_MESSAGE);
      } else {
        // User was correct if you get here.
        vsTester_printInfoMessage(user_correct_sequence_e, DRAW_MESSAGE);
      }
      if (sequenceLength <= MAX_TEST_SEQUENCE_LENGTH) {
        globals_setSequenceIterationLength(sequenceLength);
        verifySequence_disable();
        delayCounter = 0;
        vsTester_currentState = vsTester_delay_st;
      } else {
        delayCounter = 0;
        vsTester_currentState = vsTester_quit_st;
      }
    }
    break;
  case vsTester_delay_st:
    if (delayCounter < DELAY_COUNTER_EXPIRE) {
      delayCounter++;
    } else {
      delayCounter = 0;
      vsTester_printInfoMessage(user_erase_e, ERASE_MESSAGE);
      vsTester_printInstructions(sequenceLength, DRAW_MESSAGE);
      vsTester_currentState = vsTester_init_st;
    }
    break;
  case vsTester_quit_st:
    // Just park here with a short delay to print a quitting message.
    if (delayCounter < DELAY_COUNTER_EXPIRE) {
      delayCounter++;
    } else {
      vsTester_printInfoMessage(user_erase_e, ERASE_MESSAGE);
      vsTester_printInfoMessage(user_quit_e, DRAW_MESSAGE);
      vsTester_currentState = vsTester_done_st;
    }
    break;
  case vsTester_done_st:
    // Just park it here.
    break;
  }
}
