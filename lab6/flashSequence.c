// This will set the sequence to a simple sequential pattern.
// It starts by flashing the first color, and then increments the index and flashes the first
// two colors and so forth. Along the way it prints info messages to the LCD screen.
#define TEST_SEQUENCE_LENGTH 8	// Just use a short test sequence.
uint8_t flashSequence_testSequence[TEST_SEQUENCE_LENGTH] = {
    SIMON_DISPLAY_REGION_0,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_0};	// Simple sequence.
#define INCREMENTING_SEQUENCE_MESSAGE1 "Incrementing Sequence"	// Info message.
#define RUN_TEST_COMPLETE_MESSAGE "Runtest() Complete"          // Info message.
#define MESSAGE_TEXT_SIZE 2     // Make the text easy to see.
#define TWO_SECONDS_IN_MS 2000  // Two second delay.
#define TICK_PERIOD 75          // 200 millisecond delay.
#define TEXT_ORIGIN_X 0                  // Text starts from far left and
#define TEXT_ORIGIN_Y (DISPLAY_HEIGHT/2) // middle of screen.

// Print the incrementing sequence message.
void flashSequence_printIncrementingMessage() {
  display_fillScreen(DISPLAY_BLACK);  // Otherwise, tell the user that you are incrementing the sequence.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Roughly centered.
  display_println(INCREMENTING_SEQUENCE_MESSAGE1);// Print the message.
  utils_msDelay(TWO_SECONDS_IN_MS);   // Hold on for 2 seconds.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
}

// Run the test: flash the sequence, one square at a time
// with helpful information messages.
void flashSequence_runTest() {
  display_init();	                // We are using the display.
  display_fillScreen(DISPLAY_BLACK);	// Clear the display.
  globals_setSequence(flashSequence_testSequence, TEST_SEQUENCE_LENGTH);	// Set the sequence.
  flashSequence_init();               // Initialize the flashSequence state machine
  flashSequence_enable();             // Enable the flashSequence state machine.
  int16_t sequenceLength = 1;         // Start out with a sequence of length 1.
  globals_setSequenceIterationLength(sequenceLength);	// Set the iteration length.
  display_setTextSize(MESSAGE_TEXT_SIZE); // Use a standard text size.
  while (1) {                             // Run forever unless you break.
    flashSequence_tick();	          // tick the state machine.
    utils_msDelay(TICK_PERIOD);   // Provide a 1 ms delay.
    if (flashSequence_isComplete()) {	// When you are done flashing the sequence.
      flashSequence_disable();          // Interlock by first disabling the state machine.
      flashSequence_tick();             // tick is necessary to advance the state.
      utils_msDelay(TICK_PERIOD);       // don't really need this here, just for completeness.
      flashSequence_enable();           // Finish the interlock by enabling the state machine.
      utils_msDelay(TICK_PERIOD);       // Wait 1 ms for no good reason.
      sequenceLength++;                 // Increment the length of the sequence.
      if (sequenceLength > TEST_SEQUENCE_LENGTH)  // Stop if you have done the full sequence.
        break;
      // Tell the user that you are going to the next step in the pattern.
      flashSequence_printIncrementingMessage();
      globals_setSequenceIterationLength(sequenceLength);  // Set the length of the pattern.
    }
  }
  // Let the user know that you are finished.
  display_fillScreen(DISPLAY_BLACK);              // Blank the screen.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Set the cursor position.
  display_println(RUN_TEST_COMPLETE_MESSAGE);     // Print the message.
}