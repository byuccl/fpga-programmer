#define MESSAGE_X 0
//#define MESSAGE_Y (display_width()/4)
#define MESSAGE_Y (display_height()/2)
#define MESSAGE_TEXT_SIZE 2
//#define MESSAGE_STARTING_OVER
#define BUTTON_0 0  // Index for button 0
#define BUTTON_1 1  // Index for button 1
#define BUTTON_2 2  // Index for button 2
#define BUTTON_3 3  // Index for button 3
// Prints the instructions that the user should follow when
// testing the verifySequence state machine.
// Takes an argument that specifies the length of the sequence so that
// the instructions are tailored for the length of the sequence.
// This assumes a simple incrementing pattern so that it is simple to
// instruct the user.
void verifySequence_printInstructions(uint8_t length, bool startingOver) {
    display_fillScreen(DISPLAY_BLACK);              // Clear the screen.
    display_setTextSize(MESSAGE_TEXT_SIZE);     // Make it readable.
    display_setCursor(MESSAGE_X, MESSAGE_Y);    // Rough center.
    if (startingOver) {                                             // Print a message if you start over.
        display_fillScreen(DISPLAY_BLACK);          // Clear the screen if starting over.
        display_setTextColor(DISPLAY_WHITE);        // Print whit text.
        display_println("Starting Over. ");         // Starting over message.
    }
    // Print messages are self-explanatory, no comments needed.
    // These messages request that the user touch the buttons in a specific sequence.
    display_println("Tap: ");
    display_println();
    switch (length) {
    case 1:
        display_println("red");
        break;
    case 2:
        display_println("red, yellow ");
        break;
    case 3:
        display_println("red, yellow, blue ");
        break;
    case 4:
        display_println("red, yellow, blue, green ");
        break;
    default:
        break;
    }
    display_println("in that order.");
    display_println();
    display_println("hold BTN0 to quit.");
}

// Just clears the screen and draws the four buttons used in Simon.
void verifySequence_drawButtons() {
    display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
    simonDisplay_drawAllButtons();      // Draw all the buttons.
}

// This will set the sequence to a simple sequential pattern.
#define MAX_TEST_SEQUENCE_LENGTH 4  // the maximum length of the pattern
uint8_t verifySequence_testSequence[MAX_TEST_SEQUENCE_LENGTH] = {0, 1, 2, 3};  // A simple pattern.
#define MESSAGE_WAIT_MS 4000  // Display messages for this long.

// Increment the sequence length making sure to skip over 0.
// Used to change the sequence length during the test.
int16_t incrementSequenceLength(int16_t sequenceLength) {
    int16_t value = (sequenceLength + 1) % (MAX_TEST_SEQUENCE_LENGTH+1);
    if (value == 0) value++;
    return value;
}

// Used to select from a variety of informational messages.
enum verifySequence_infoMessage_t {
    user_time_out_e,            // means that the user waited too long to tap a color.
    user_wrong_sequence_e,      // means that the user tapped the wrong color.
    user_correct_sequence_e,    // means that the user tapped the correct sequence.
    user_quit_e                 // means that the user wants to quite.
};

// Prints out informational messages based upon a message type (see above).
void verifySequence_printInfoMessage(verifySequence_infoMessage_t messageType) {
    // Setup text color, position and clear the screen.
    display_setTextColor(DISPLAY_WHITE);
    display_setCursor(MESSAGE_X, MESSAGE_Y);
    display_fillScreen(DISPLAY_BLACK);
  switch(messageType) {
  case user_time_out_e:  // Tell the user that they typed too slowly.
        display_println("Error:");
        display_println();
        display_println("  User tapped sequence");
        display_println("  too slowly.");
    break;
  case user_wrong_sequence_e:  // Tell the user that they tapped the wrong color.
        display_println("Error: ");
        display_println();
        display_println("  User tapped the");
        display_println("  wrong sequence.");
    break;
  case user_correct_sequence_e:  // Tell the user that they were correct.
        display_println("User tapped");
        display_println("the correct sequence.");
    break;
  case user_quit_e:             // Acknowledge that you are quitting the test.
    display_println("quitting runTest().");
    break;
  default:
    break;
  }
}

#define TICK_PERIOD_IN_MS 100
// Tests the verifySequence state machine.
// It prints instructions to the touch-screen. The user responds by tapping the
// correct colors to match the sequence.
// Users can test the error conditions by waiting too long to tap a color or
// by tapping an incorrect color.
void verifySequence_runTest() {
    display_init();  // Always must do this.
    buttons_init();  // Need to use the push-button package so user can quit.
    int16_t sequenceLength = 1;  // Start out with a sequence length of 1.
    verifySequence_printInstructions(sequenceLength, false);  // Tell the user what to do.
    utils_msDelay(MESSAGE_WAIT_MS);  // Give them a few seconds to read the instructions.
    verifySequence_drawButtons();    // Now, draw the buttons.
    // Set the test sequence and it's length.
    globals_setSequence(verifySequence_testSequence, MAX_TEST_SEQUENCE_LENGTH);
    globals_setSequenceIterationLength(sequenceLength);
    verifySequence_init();    // Initialize the verifySequence state machine
    // Enable the verifySequence state machine.
    verifySequence_enable();  // Everything is interlocked, so first enable the machine.
    // Need to hold button until it quits as you might be stuck in a delay.
    while (!(buttons_read() & BUTTONS_BTN0_MASK)) { 
        // verifySequence uses the buttonHandler state machine so you need to "tick" both of them.
        verifySequence_tick();  // Advance the verifySequence state machine.
        buttonHandler_tick();   // Advance the buttonHandler state machine.
        utils_msDelay(TICK_PERIOD_IN_MS);       // Wait for a tick period.
        // If the verifySequence state machine has finished, check the result, 
        // otherwise just keep ticking both machines.
        if (verifySequence_isComplete()) {
            if (verifySequence_isTimeOutError()) {                // Was the user too slow?
                verifySequence_printInfoMessage(user_time_out_e); // Yes, tell the user that they were too slow.
            } else if (verifySequence_isUserInputError()) {       // Did the user tap the wrong color?
                verifySequence_printInfoMessage(user_wrong_sequence_e); // Yes, tell them so.
            } else {
                verifySequence_printInfoMessage(user_correct_sequence_e); // User was correct if you get here.
            }
            utils_msDelay(MESSAGE_WAIT_MS);                            // Allow the user to read the message.
            sequenceLength = incrementSequenceLength(sequenceLength);  // Increment the sequence.
            globals_setSequenceIterationLength(sequenceLength);        // Set the length for the verifySequence state machine.
            verifySequence_printInstructions(sequenceLength, true);    // Print the instructions.
            utils_msDelay(MESSAGE_WAIT_MS);                            // Let the user read the instructions.
            verifySequence_drawButtons();                              // Draw the buttons.
            verifySequence_disable();                                  // Interlock: first step of handshake.
            verifySequence_tick();                                     // Advance the verifySequence machine.
            buttonHandler_tick();
            utils_msDelay(TICK_PERIOD_IN_MS);                          // Wait for tick period.
            verifySequence_enable();                                   // Interlock: second step of handshake.
            utils_msDelay(TICK_PERIOD_IN_MS);                          // Wait for tick period.
        }
    }
    verifySequence_printInfoMessage(user_quit_e);  // Quitting, print out an informational message.
}