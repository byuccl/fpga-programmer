/*
 * runningModes.c

 *
 *  Created on: Mar 11, 2015
 *      Author: hutch
 */
#include "runningModes.h"
#include "detector.h"
#include "display.h"
#include "filter.h"
#include "histogram.h"
#include "hitLedTimer.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "isr.h"
#include "ledTimer.h"
#include "leds.h"
#include "lockoutTimer.h"
#include "mio.h"
#include "my_libs/buttons.h"
#include "my_libs/switches.h"
#include "queue.h"
#include "sound.h"
#include "transmitter.h"
#include "trigger.h"
#include "utils.h"
#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HIT_COUNT 100

#define MAX_BUFFER_SIZE 100 // Used for a generic message buffer.

#define DETECTOR_HIT_ARRAY_SIZE                                                \
  FILTER_FREQUENCY_COUNT // The array contains one location per user frequency.

#define HISTOGRAM_BAR_COUNT                                                    \
  FILTER_FREQUENCY_COUNT // As many histogram bars as user filter frequencies.

#define ISR_CUMULATIVE_TIMER INTERVAL_TIMER_TIMER_0 // Used by the ISR.
#define TOTAL_RUNTIME_TIMER                                                    \
  INTERVAL_TIMER_TIMER_1 // Used to compute total run-time.
#define MAIN_CUMULATIVE_TIMER                                                  \
  INTERVAL_TIMER_TIMER_2 // Used to compute cumulative run-time in main.

#define SYSTEM_TICKS_PER_HISTOGRAM_UPDATE                                      \
  30000 // Update the histogram about 3 times per second.

#define RUNNING_MODE_WARNING_TEXT_SIZE 2 // Upsize the text for visibility.
#define RUNNING_MODE_WARNING_TEXT_COLOR DISPLAY_RED // Red for more visibility.
#define RUNNING_MODE_NORMAL_TEXT_SIZE 1 // Normal size for reporting.
#define RUNNING_MODE_NORMAL_TEXT_COLOR DISPLAY_WHITE // White for reporting.
#define RUNNING_MODE_SCREEN_X_ORIGIN 0 // Origin for reporting text.
#define RUNNING_MODE_SCREEN_Y_ORIGIN 0 // Origin for reporting text.

// Detector should be invoked this often for good performance.
#define SUGGESTED_DETECTOR_INVOCATIONS_PER_SECOND 70000
// ADC queue should have no more than this number of unprocessed elements for
// good performance.
#define SUGGESTED_REMAINING_ELEMENT_COUNT 500

// Defined to make things more readable.
#define INTERRUPTS_CURRENTLY_ENABLED true
#define INTERRUPTS_CURRENTLY_DISABLE false

// Keep track of detector invocations.
static uint32_t detectorInvocationCount = 0;

// This array is indexed by frequency number. If array-element[freq_no] == true,
// the frequency is ignored, e.g., no hit will ever occur at that frequency.
// static bool ignoredFrequenciesArray[FILTER_FREQUENCY_COUNT] =
//  {false, false, false, false, false, false, false, false, false, false};

// Prints out various run-time statistics on the TFT display.
// Assumes the following:
// main is keeping track of detected interrupts with
// countInterruptsViaInterruptsIsrFlag, interval_timer(0) is the cumulative
// run-time of the ISR, interval_timer(1) is the total run-time,
// interval_timer(2) is the time spent in main running the filters, updating the
// display, and so forth. No comments in the code, the print statements are
// self-explanatory.
void runningModes_printRunTimeStatistics() {
  char sprintfBuffer[MAX_BUFFER_SIZE]; // Generic message buffer.
  // Setup the screen.
  display_setTextSize(RUNNING_MODE_NORMAL_TEXT_SIZE);
  display_setTextColor(RUNNING_MODE_NORMAL_TEXT_COLOR);
  display_setCursor(RUNNING_MODE_SCREEN_X_ORIGIN, RUNNING_MODE_SCREEN_Y_ORIGIN);
  display_fillScreen(DISPLAY_BLACK);
  if (interrupts_getAdcInputMode() == INTERRUPTS_ADC_UNIPOLAR_MODE) {
    display_println("ADC mode: unipolar.\n\r");
  } else if (interrupts_getAdcInputMode() == INTERRUPTS_ADC_BIPOLAR_MODE) {
    display_println("ADC mode: bipolar.\n\r");
  }
  // Print out the number of unprocessed elements in ADC queue.
  display_print("Unprocessed elements in ADC queue:");
  uint32_t remainingElementCount = isr_adcBufferElementCount();
  display_printlnDecimalInt(remainingElementCount);
  display_printChar('\n');
  double runningSeconds, isrRunningSeconds, mainLoopRunningSeconds;
  runningSeconds = intervalTimer_getTotalDurationInSeconds(TOTAL_RUNTIME_TIMER);
  // Print out total running time in seconds.
  display_print("Measured run time in seconds: ");
  sprintf(sprintfBuffer, "%5.2f", runningSeconds);
  display_print(sprintfBuffer);
  display_printChar('\n');
  display_printChar('\n');
  isrRunningSeconds =
      intervalTimer_getTotalDurationInSeconds(ISR_CUMULATIVE_TIMER);
  // Print out cumulative time spent in timer ISR.
  display_print("Cumulative run time in timerIsr: ");
  sprintf(sprintfBuffer, "%5.2f", isrRunningSeconds);
  display_print(sprintfBuffer);
  display_print(" (");
  sprintf(sprintfBuffer, "%5.2f", isrRunningSeconds / runningSeconds * 100);
  display_print(sprintfBuffer);
  display_println("%)");
  display_printChar('\n');
  mainLoopRunningSeconds =
      intervalTimer_getTotalDurationInSeconds(MAIN_CUMULATIVE_TIMER);
  // Print out cumulative spent in detector.
  display_print("Cumulative run-time in detector: ");
  sprintf(sprintfBuffer, "%5.2f", mainLoopRunningSeconds / runningSeconds);
  display_print(" (");
  display_print(sprintfBuffer);
  display_println("%)");
  display_printChar('\n');
  uint32_t interruptCount = interrupts_isrInvocationCount();
  // Print out total interrupt count.
  display_print("Total interrupts:            ");
  display_printlnDecimalInt(interruptCount);
  display_printChar('\n');
  display_print("Detector invocation count: ");
  // Print out detector invocations per second.
  display_printlnDecimalInt(detectorInvocationCount);
  display_printChar('\n');
  display_print("Detector invocations per second: ");
  sprintf(sprintfBuffer, "%5.2f", detectorInvocationCount / runningSeconds);
  display_print(sprintfBuffer);
  display_printChar('\n');
  display_printChar('\n');
  // If the detector invocation rate is too low, inform the user.
  if (detectorInvocationCount / runningSeconds <
      SUGGESTED_DETECTOR_INVOCATIONS_PER_SECOND) {
    display_setTextColor(RUNNING_MODE_WARNING_TEXT_COLOR);
    display_setTextSize(RUNNING_MODE_WARNING_TEXT_SIZE);
    display_print("Detector should be called at least ");
    display_printDecimalInt(SUGGESTED_DETECTOR_INVOCATIONS_PER_SECOND);
    display_println(" times per second.");
    display_printChar('\n');
  }
  // If the unprocessed element count is too high, inform the user.
  if (remainingElementCount >= SUGGESTED_REMAINING_ELEMENT_COUNT) {
    display_setTextColor(RUNNING_MODE_WARNING_TEXT_COLOR);
    display_setTextSize(RUNNING_MODE_WARNING_TEXT_SIZE);
    display_println("ADC queue should contain ");
    display_print("less than ");
    display_printDecimalInt(SUGGESTED_REMAINING_ELEMENT_COUNT);
    display_println(" elements.");
  }
}

// Group all of the inits together to reduce visual clutter.
void runningModes_initAll() {
  buttons_init();
  switches_init();
  mio_init(false);
  intervalTimer_initAll();
  histogram_init(HISTOGRAM_BAR_COUNT);
  leds_init(true);
  transmitter_init();
  filter_init();
  isr_init();
  hitLedTimer_init();
  trigger_init();
  ledTimer_init();
}

// Returns the current switch-setting
uint16_t runningModes_getFrequencySetting() {
  uint16_t switchSetting = switches_read() & 0xF; // Bit-mask the results.
  // Provide a nice default if the slide switches are in error.
  if (!(switchSetting < FILTER_FREQUENCY_COUNT))
    return FILTER_FREQUENCY_COUNT - 1;
  else
    return switchSetting;
}

// This mode runs continuously until btn3 is pressed.
// When btn3 is pressed, it exits and prints performance information to the TFT.
// During operation, it continuously displays that received power on each
// channel, on the TFT.
void runningModes_continuous() {
  runningModes_initAll(); // All necessary inits are called here.
  bool ignoredFrequenciesArray[FILTER_FREQUENCY_COUNT];
  // setup the ignore frequencies array so you don't ignore any frequency.
  for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++)
    ignoredFrequenciesArray[i] = false;
#ifdef IGNORE_OWN_FREQUENCY
  printf("Ignoring own frequency.\n\r");
  ignoredFrequenciesArray[runningModes_getFrequencySetting()] = true;
#endif
  detector_init(ignoredFrequenciesArray);

  // sound_init();
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true); // Init all interrupts (but does not enable the
                            // interrupts at the devices).

  interrupts_enableTimerGlobalInts(); // Allows the timer to generate
                                      // interrupts.
  interrupts_startArmPrivateTimer();  // Start the private ARM timer running.
  uint16_t histogramSystemTicks =
      0; // Only update the histogram display every so many ticks.
  intervalTimer_reset(
      ISR_CUMULATIVE_TIMER); // Used to measure ISR execution time.
  intervalTimer_reset(
      TOTAL_RUNTIME_TIMER); // Used to measure total program execution time.
  intervalTimer_reset(
      MAIN_CUMULATIVE_TIMER); // Used to measure main-loop execution time.
  intervalTimer_start(
      TOTAL_RUNTIME_TIMER);            // Start measuring total execution time.
  transmitter_setContinuousMode(true); // Run the transmitter continuously.
  interrupts_enableArmInts();  // The ARM will start seeing interrupts after
                               // this.
  transmitter_run();           // Start the transmitter.
  detectorInvocationCount = 0; // Keep track of detector invocations.
  while (!(buttons_read() &
           BUTTONS_BTN3_MASK)) { // Run until you detect btn3 pressed.
    transmitter_setFrequencyNumber(runningModes_getFrequencySetting());
    detectorInvocationCount++; // Used for run-time statistics.
    histogramSystemTicks++;    // Keep track of ticks so you know when to update
                               // the histogram.
    // Run filters, compute power, etc.
    intervalTimer_start(MAIN_CUMULATIVE_TIMER); // Measure run-time when you are
                                                // doing something.
    detector(INTERRUPTS_CURRENTLY_ENABLED); // Interrupts are currently enabled.
    intervalTimer_stop(MAIN_CUMULATIVE_TIMER);
    // If enough ticks have transpired, update the histogram.
    if (histogramSystemTicks >= SYSTEM_TICKS_PER_HISTOGRAM_UPDATE) {
      double powerValues[FILTER_FREQUENCY_COUNT]; // Copy the current power
                                                  // values to here.
      filter_getCurrentPowerValues(
          powerValues); // Copy the current power values.
      histogram_plotUserFrequencyPower(
          powerValues); // Plot the power values on the TFT.
      histogramSystemTicks =
          0; // Reset the tick count and wait for the next update time.
    }
  }
  interrupts_disableArmInts();           // Stop interrupts.
  runningModes_printRunTimeStatistics(); // Print the run-time statistics.
}

void runningModes_shooter() {
  runningModes_initAll();
  // Init the ignored-frequencies so no frequencies are ignored.
  bool ignoredFrequencies[FILTER_FREQUENCY_COUNT];
  for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++)
    ignoredFrequencies[i] = false;
#ifdef IGNORE_OWN_FREQUENCY
  printf("Ignoring own frequency.\n\r");
  ignoredFrequencies[runningModes_getFrequencySetting()] = true;
#endif
  detector_init(ignoredFrequencies);
  uint16_t hitCount = 0;
  detectorInvocationCount = 0; // Keep track of detector invocations.
  sound_init();
  trigger_enable();         // Makes the trigger state machine responsive to the
                            // trigger.
  interrupts_initAll(true); // Inits all interrupts but does not enable them.
  interrupts_enableTimerGlobalInts(); // Allows the timer to generate
                                      // interrupts.
  interrupts_startArmPrivateTimer();  // Start the private ARM timer running.
  uint16_t histogramSystemTicks =
      0; // Only update the histogram display every so many ticks.
  intervalTimer_reset(
      ISR_CUMULATIVE_TIMER); // Used to measure ISR execution time.
  intervalTimer_reset(
      TOTAL_RUNTIME_TIMER); // Used to measure total program execution time.
  intervalTimer_reset(
      MAIN_CUMULATIVE_TIMER); // Used to measure main-loop execution time.
  intervalTimer_start(
      TOTAL_RUNTIME_TIMER);   // Start measuring total execution time.
  interrupts_enableArmInts(); // The ARM will start seeing interrupts after
                              // this.
  lockoutTimer_start(); // Ignore erroneous hits at startup (when all power
                        // values are essentially 0).
  while ((!(buttons_read() & BUTTONS_BTN3_MASK)) &&
         hitCount < MAX_HIT_COUNT) { // Run until you detect btn3 pressed.
    transmitter_setFrequencyNumber(
        runningModes_getFrequencySetting());    // Read the switches and switch
                                                // frequency as required.
    intervalTimer_start(MAIN_CUMULATIVE_TIMER); // Measure run-time when you are
                                                // doing something.
    histogramSystemTicks++; // Keep track of ticks so you know when to update
                            // the histogram.
    // Run filters, compute power, run hit-detection.
    detectorInvocationCount++;              // Used for run-time statistics.
    detector(INTERRUPTS_CURRENTLY_ENABLED); // Interrupts are currently enabled.
    if (detector_hitDetected()) {           // Hit detected
      hitCount++;                           // increment the hit count.
      detector_clearHit();                  // Clear the hit.
      detector_hitCount_t
          hitCounts[DETECTOR_HIT_ARRAY_SIZE]; // Store the hit-counts here.
      detector_getHitCounts(hitCounts);       // Get the current hit counts.
      histogram_plotUserHits(hitCounts);      // Plot the hit counts on the TFT.
    }
    intervalTimer_stop(
        MAIN_CUMULATIVE_TIMER); // All done with actual processing.
  }
  interrupts_disableArmInts(); // Done with loop, disable the interrupts.
  hitLedTimer_turnLedOff();    // Save power :-)
  runningModes_printRunTimeStatistics(); // Print the run-time statistics to the
                                         // TFT.
  printf("Shooter mode terminated after detecting %d shots.\n\r", hitCount);
}

// This mode simply dumps raw ADC values to the console.
// I wrote this to determine if bipolar mode was working for the ADC.
// Just loops forever, you must manually stop the program with an external
// reset.
void runningModes_dumpRawAdcValues() {
  runningModes_initAll();
  interrupts_initAll(true); // Sets up interrupts and the XADC.
  // We don't need interrupts, so just loop and print out raw ADC values.
  while (1) {
    // In bipolar mode, the ADC returns a 12-bit signed value.
    // As such, you will need to do your own sign-extension out to 16 bits
    // in order to get to a C primitive type.
    // Thus, if bit-11 is a 1, bits 15-12 must also be set to 1, and
    // if bit-11 is a 0, bits 15-12 must also be set to a 0.
    int16_t signExtendedValue = interrupts_getAdcData();
    signExtendedValue |= (signExtendedValue & 0x800) ? 0xF000 : 0x0000;
    printf("raw ADC value: %d\n", signExtendedValue);
  }
}
