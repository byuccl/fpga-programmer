/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "bhTester.h"
#include "buttonHandler.h"
#include "display.h"
#include "simonDisplay.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {
  bhTester_init_st,
  bhTester_running_st,
  bhTester_done_st
} bhTester_state_t;
bhTester_state_t bhTester_currentState;
uint16_t touchCount;

#define MAX_TOUCH_COUNT 20
#define DISPLAY_X_CENTER (DISPLAY_WIDTH / 2)
#define DISPLAY_Y_CENTER (DISPLAY_HEIGHT / 2)
#define TEXT_SIZE 3

// Init the button handler test state machine
void bhTester_init() {
  display_init();
  buttonHandler_init();
  bhTester_currentState = bhTester_init_st;
  touchCount = 0;
  display_fillScreen(DISPLAY_BLACK); // Clear the screen.
  simonDisplay_drawAllButtons();
}

// Tick the button handler test state machine
void bhTester_tick() {
  static uint8_t lastRegionNumber = 0;

  // SM actions and transitions
  switch (bhTester_currentState) {
  case bhTester_init_st:
    buttonHandler_enable();
    bhTester_currentState = bhTester_running_st;
    break;
  case bhTester_running_st:
    if (buttonHandler_releaseDetected()) {
      uint8_t regionNumber = buttonHandler_getRegionNumber();
      // Erase last drawn region numbers.
      display_drawChar(DISPLAY_X_CENTER, DISPLAY_Y_CENTER,
                       '0' + lastRegionNumber, DISPLAY_BLACK, DISPLAY_BLACK,
                       TEXT_SIZE);
      // Draw the latest region number.
      display_drawChar(DISPLAY_X_CENTER, DISPLAY_Y_CENTER, '0' + regionNumber,
                       DISPLAY_WHITE, DISPLAY_BLACK, TEXT_SIZE);
      printf("button released: %d\n", buttonHandler_getRegionNumber());
      touchCount++;
      buttonHandler_disable();
      if (touchCount < MAX_TOUCH_COUNT) {
        bhTester_currentState = bhTester_init_st;
      } else {
        bhTester_currentState = bhTester_done_st;
        printf("Terminating test after %d touches.\n", MAX_TOUCH_COUNT);
      }
    }
    break;
  case bhTester_done_st:
    // Just hang out here.
    break;
  }
}