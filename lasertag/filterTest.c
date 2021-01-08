/*
 * filterTest.c

 *
 *  Created on: Mar 11, 2015
 *      Author: hutch
 */

/****************************************************************************************************
 * Comment out the line below to perform a filter-only verification. Leave the
 *line uncommented and you perform the verification from the adcBuffer to the
 *detector, for a more thorough test. Make sure to leave the line below
 *commented until you have completely coded detector.c and isr.c Otherwise this
 *file will not compile.
 ***************************************************************************************************/
//#define ADC_THROUGH_DETECTOR_FILTER_TEST

/****************************************************************************************************
 * Uncomment the line below if your IIR-A coefficient arrays contain a leading
 *'1'.
 ****************************************************************************************************/
//#define FILTER_TEST_USED_LEADING_1_IN_IIR_A_COEFFICIENT_ARRAY

/****************************************************************************************************
 * Uncomment the line below if you are using output queues of size 2001 to
 *incrementally compute power. In this case, the oldest value is located at
 *location 0 in the output queue, and you compute the current power from 1 to
 *1999 (all output-queue values except value 0. The default is to use a
 *queue-size of 2000.
 ****************************************************************************************************/
//#define FILTER_TEST_STORE_OLD_VALUE_IN_QUEUE

#include "filter.h"
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
#include "detector.h"
#include "isr.h"
#endif
#include "histogram.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

/****************************************************************************************************
 * filterTest.c provides a set of self-contained test functions that:
 * 1. test basic functionality of the fir and iir filters,
 * 2. plots the frequency response of the decimating fir filter.
 * 3. plots the frequency response of each of the iir filters.
 * A square-wave input is used for all plots.
 ****************************************************************************************************/

// A histogram bar for each frequency.
#define FILTER_TEST_HISTOGRAM_BAR_COUNT FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT
// Use additional out-of-band frequencies to test the FIR response.
#define FILTER_TEST_OUT_OF_BAND_TICK_COUNT 11
// Testing frequencies include the 10 user frequencies and some number of "out
// of band" frequencies.
#define FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT                                \
  (FILTER_FREQUENCY_COUNT + FILTER_TEST_OUT_OF_BAND_TICK_COUNT)
// Out of band frequencies defined similar to user frequencies, as tick counts.
// All are square waves.
static const uint16_t
    filterTest_firTestOutOfBandTickCounts[FILTER_TEST_OUT_OF_BAND_TICK_COUNT] =
        {22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2};
#define MAX_BUF 10 // Used for a temporary char buffer.
#define FILTER_TEST_PULSE_WIDTH_LENGTH                                         \
  20000 // The pulse-width is 20000 because everything in the test runs at 100
        // kHz.
#define FILTER_TEST_MIN_INPUT_VALUE                                            \
  (-1.0) // This is the top of the square wave.
#define FILTER_TEST_MAX_INPUT_VALUE                                            \
  (1.0) // This is the bottom of the square wave.
#define FILTER_TEST_INPUT_OFFSET                                               \
  (1.0) // Add this to the input if you need to make it unipolar (for plotting
        // the input).

// This array will contain the frequency tick counts that will be applied
// against the FIR filter. User frequencies + out-of-band frequencies.
static uint16_t
    filterTest_firTestTickCounts[FILTER_TEST_OUT_OF_BAND_TICK_COUNT +
                                 FILTER_FREQUENCY_COUNT];

/******************************************************************************************
 ******************************* Start of functions
 ****************************************
 ******************************************************************************************/

static bool filterTest_initFlag =
    false; // True if the filterTest_init() function was called.

// Everything has an init() function.
void filterTest_init() {
  // Copy the user and out-of-bound frequency tick counts to the test array.
  for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    filterTest_firTestTickCounts[i] = filter_frequencyTickTable[i];
  }
  for (uint16_t j = FILTER_FREQUENCY_COUNT;
       j < FILTER_FREQUENCY_COUNT + FILTER_TEST_OUT_OF_BAND_TICK_COUNT; j++) {
    filterTest_firTestTickCounts[j] =
        filterTest_firTestOutOfBandTickCounts[j - FILTER_FREQUENCY_COUNT];
  }
  filterTest_initFlag = true;
}

// Performs a floating-point compare that allows for some error.
#define TEST_FILTER_FLOATING_POINT_EPSILON 1.0E-12L
bool filterTest_floatingPointEqual(double a, double b) {
  return fabs(a - b) < TEST_FILTER_FLOATING_POINT_EPSILON;
}

// Returns the value most recently added to the queue.
// Returns 0 if the queue was empty but does not report an error.
queue_data_t filterTest_readMostRecentValueFromQueue(queue_t *q) {
  if (queue_empty(q))
    return 0.0;
  else
    return queue_readElementAt(q, queue_elementCount(q) -
                                      1); // Rely on queue_readElementAt().
}

// Used to normalize values prior to plotting.
void filterTest_normalizeArrayValues(double normalizedValues[],
                                     double origValues[], uint16_t size) {
  // First, find the indicies of the min. and max. value in the
  // currentPowerValue array.
  uint16_t maxIndex = 0;
  for (int i = 0; i < size; i++) {
    if (origValues[i] > origValues[maxIndex])
      maxIndex = i;
  }
  double maxValue = origValues[maxIndex];
  // Normalize everything between 0.0 and 1.0.
  for (int i = 0; i < size; i++)
    normalizedValues[i] = origValues[i] / maxValue;
}

// Returns the decimation value.
uint16_t filterTest_getDecimationValue() {
  return FILTER_FIR_DECIMATION_FACTOR;
}

// Invokes FIR-filter according to the decimation factor.
// Returns true if it actually invokes filter_firFilter().
// Used when not using the detector() function.
static uint16_t firDecimationCount = 0;
bool filterTest_decimatingFirFilter() {
  if (firDecimationCount ==
      filterTest_getDecimationValue() - 1) { // Time to run the FIR filter?
    filter_firFilter();                      // Run the FIR filter.
    firDecimationCount = 0;                  // Reset the decimation count.
    return true; // Return true because you ran the FIR filter.
  }
  firDecimationCount++; // Increment the decimation count each time you call
                        // this filter.
  return false;         // Didn't run the FIR filter.
}

// Plot the power (frequency response) for the FIR filter.
// This plotting routine assumes that:
// 1. The size of the array is FILTER_FIR_POWER_TEST_PERIOD_COUNT and it
// contains power for these tested frequencies.
// 2. The first 10 frequencies are the user frequencies.
// 3. The remaining frequencies are between 4 kHz and 50 kHz.
// 4. The periods of the frequencies are those contained in
// filter_testPeriodTickCounts[], assuming a tick-rate of 100 kHz.
void filterTest_plotFirFrequencyResponse(double firPowerValues[]) {
  // Now we start plotting the results.
  double normalizedPowerValues
      [FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT]; // Normalized power values
                                                 // stored here.
  filterTest_normalizeArrayValues(
      normalizedPowerValues, firPowerValues,
      FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT);    // Normalize the values.
  histogram_init(FILTER_TEST_HISTOGRAM_BAR_COUNT); // Init the histogram.
  // Set labels and colors (blue) for the standard frequencies 0-9.
  for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    histogram_setBarColor(i, DISPLAY_BLUE); // Sets the color of the bar.
    char tempLabel[MAX_BUF]; // Temp variable for label generation.
    snprintf(
        tempLabel, MAX_BUF, "%d",
        i); // Create the label that represents one of the user frequencies.
    histogram_setBarLabel(i, tempLabel); // Finally, set the label.
  }
  // Set the colors for the other nonstandard frequencies to be red so that the
  // stand out. This loop prints out all of the
  for (int i = 10; i < FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT; i++) {
    histogram_setBarColor(i, DISPLAY_RED);
    char tempLabel[MAX_BUF]; // Used to create labels.
    // Create three kinds of labels.
    // 1. Just label the first set of defined frequencies 0-9.
    // 2. This is the start of the frequencies outside the actual transmitted
    // frequencies. The bounds are printed at the start and end of this range,
    // using the labels so that they display OK in the limited space.
    // 3. The lower bound for out-of-bound testing frequencies.
    if (i == FILTER_FREQUENCY_COUNT) { // Now, you are past the user frequencies
                                       // - provides a context to show frequency
                                       // response.
      snprintf(tempLabel, MAX_BUF, "%2.1fkHz",
               ((double)FILTER_SAMPLE_FREQUENCY_IN_KHZ) /
                   filterTest_firTestTickCounts[i]);
      histogram_setBarLabel(i, tempLabel);
      // 4. Print the upper bound for out-of-bound testing frequencies further
      // to the left to make readable.
    } else if (i == FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT - 4) {
      snprintf(tempLabel, MAX_BUF, "%dkHz",
               (FILTER_SAMPLE_FREQUENCY_IN_KHZ) /
                   (filterTest_firTestTickCounts
                        [FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT - 1]));
      histogram_setBarLabel(i, tempLabel);
      // This abuses the labels a bit to create something that explains the
      // range of frequencies outside the IIR frequencies. Print a "-" so
      // separate the bounds for readability.
    } else if (i == FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT - 6) {
      histogram_setBarLabel(i, "-");
      // Print blank space everywhere else for labels everywhere else.
    } else {
      histogram_setBarLabel(i, "");
    }
  }
  // Set the actual histogram-bar data to be power values.
  for (uint16_t barIndex = 0; barIndex < FILTER_TEST_HISTOGRAM_BAR_COUNT;
       barIndex++) {
    histogram_setBarData(
        barIndex,
        normalizedPowerValues[barIndex] * HISTOGRAM_MAX_BAR_DATA_IN_PIXELS, "");
  }
  histogram_redrawBottomLabels(); // Need to redraw the bottom labels because I
                                  // changed the colors.
  histogram_updateDisplay();      // Redraw the histogram.
}

// Finds the max in values[].
double findMax(double values[], uint32_t size) {
  double maxValue = values[0];
  for (uint32_t i = 1; i < size; i++)
    if (values[i] > maxValue)
      maxValue = values[i];
  return maxValue;
}

// Find the min in values[].
double findMin(double values[], uint32_t size) {
  double minValue = values[0];
  for (uint32_t i = 1; i < size; i++)
    if (values[i] < minValue)
      minValue = values[i];
  return minValue;
}

// This assumes that the TFT display is in mode 1 (landscape-mode with the
// origin at the top-left). Incoming data are assumed unipolar: 0 to some max
// value Incoming data are scaled in X and Y and mapped so that the plotted
// range is: -1.0 to 1.0. Negative values are plotted on the bottom half of the
// screen, positive values are plotted on the top-half. A single horizontal line
// marks the y-value of 0.
#define ONE_HALF(x) ((x) / 2)      // Divide by 2.
#define ONE_HALF_FP(x) ((x) / 2.0) // Divide by 2.0
#define TIMES2_FP(x) ((x)*2.0)     // Floating point multiply by 2.0
#define PLOT_COLOR DISPLAY_GREEN
#define PLOT_Y0_LINE_COLOR DISPLAY_WHITE
void filterTest_plotInputValues(double xValues[], double yValues[],
                                uint32_t size) {
  // Init the display and set the rotation.
  display_init();
  display_setRotation(DISPLAY_LANDSCAPE_MODE_ORIGIN_UPPER_LEFT);
  display_fillScreen(DISPLAY_BLACK); // Clear the screen.
  double xScale =
      findMax(xValues, size) - findMin(xValues, size); // Scale in x.
  double yScale =
      findMax(yValues, size) - findMin(yValues, size); // Scale in y.
  //  uint16_t y0Point = display_height()/2;                        // y0-point
  //  is always from the y=0 line on the TFT display.
  uint16_t y0Point = ONE_HALF(display_height()); // y0-point is always from the
                                                 // y=0 line on the TFT display.
  display_drawLine(0, y0Point, display_width() - 1, y0Point,
                   PLOT_Y0_LINE_COLOR); // Draw the y=0 line.
  for (uint32_t i = 0; i < size; i++) {
    //    double_t y1PointFl = ((yValues[i]/yScale) * 2.0) - 1.0;     // Convert
    //    y-coordinate range to -1.0 to 1.0. Conceptually easier.
    double_t y1PointFl =
        TIMES2_FP(yValues[i] / yScale) -
        1.0; // Convert y-coordinate range to -1.0 to 1.0. Conceptually easier.
    uint16_t x0Point = (xValues[i] / xScale) * display_width() -
                       1; // Scale X by the width of the display.
    uint16_t x1Point = x0Point;
    if (y1PointFl < 0.0) { // negative is drawn in lower half.
      uint16_t y1Point =
          (-y1PointFl * ((double)ONE_HALF(display_height()) - 1)) +
          ONE_HALF(display_height());
      display_drawLine(x0Point, y0Point, x1Point, y1Point, PLOT_COLOR);
    } else { // positive is drawn in upper half.
      uint16_t y1Point =
          y0Point - (y1PointFl)*ONE_HALF(
                        display_height()); // y = 1.0 = 0 (top of TFT display).
      display_drawLine(x0Point, y0Point, x1Point, y1Point, PLOT_COLOR);
    }
  }
}

// Helper function to create input waveform when testing only the filter.c code.
double computeFilterInput(uint16_t freqTick, uint16_t currentPeriodTickCount) {
  double filterValue;
  if (freqTick <
      ONE_HALF(currentPeriodTickCount)) // The period has two halves: the -1.0
                                        // part and the 1.0 part.
    filterValue = FILTER_TEST_MIN_INPUT_VALUE; // The first half of the period
                                               // is a minimum value.
  else
    filterValue = FILTER_TEST_MAX_INPUT_VALUE; // The second half of the period
                                               // is a maximum value.
  return filterValue;
}

#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
// When performing a complete system test, the input goes to the ADC buffer and
// must be computed accordingly.
uint16_t computeAdcBufferInput(uint16_t freqTick,
                               uint16_t currentPeriodTickCount) {
  uint16_t adcValue;
  //  if (freqTick < currentPeriodTickCount/2)// The period has two halves: the
  //  -1.0 part and the 1.0 part.
  if (freqTick < ONE_HALF(currentPeriodTickCount)// The period has two halves: the -1.0 part and the 1.0 part.
      adcValue = ISR_ADC_INTEGER_MIN_VALUE; // The first half of the period is a minimum value.
  else
    adcValue = ISR_ADC_INTEGER_MAX_VALUE; // The second half of the period is a maximum value.
  return adcValue;
}
#endif

// Plots the frequency response of the FIR filter on the TFT.
// Everything is defined assuming a 100 kHz sample rate.
// Frequencies run from 1.1 kHz to 50 kHz.
// Output values are retrieved from a FIR debug queue (see
// filter_getFirOutputDebugQueue()). Power is computed internally. Does not use
// the filter_computePower... functions. To plot the input as well as output,
// pass true to plotInputFlag.
#define PLOT_VALUE_MAX_COUNT                                                   \
  360 // You can collect up to this many values to plot.
#define PERIODS_TO_PLOT                                                        \
  2 // The number of period's worth of data to collect and plot.
#define INPUT_PLOT_VIEW_DELAY 1000 // The plot will be visible for this long.
void filterTest_runSquareWaveFirPowerTest(bool printMessageFlag,
                                          bool plotInputFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return;
  }
  double firPower = 0.0; // Power will be accumulated here.
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
  detector_init(); // You will be using the detector to invoke the filters.
  isr_init(); // You will be using adcBuffer to provide data to the filters.
#endif
  if (printMessageFlag) {
    // Tells you that this function is plotting the frequency response for the
    // FIR filter for a set of frequencies.
    printf(
        "running filter_runFirPowerTest() - plotting power values (frequency "
        "response) for frequencies %1.2lf kHz to %1.2lf kHz for FIR filter to "
        "TFT display.\n\r",
        ((double)((FILTER_SAMPLE_FREQUENCY_IN_KHZ)) /
         filterTest_firTestTickCounts[0]),
        ((double)((FILTER_SAMPLE_FREQUENCY_IN_KHZ)) /
         filterTest_firTestTickCounts[FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT -
                                      1]));
  }
  double testPeriodPowerValue
      [FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT]; // Computed power values will
                                                 // go here.
  uint16_t freqCount = 0;                        // Used to print info message.
  // Simulate running everything at 100 kHz. Simply add either 1.0 or -1.0 to
  // xQueue based upon the the frequency you are simulating. Iterate over all of
  // the test-periods.
  double xValues[PLOT_VALUE_MAX_COUNT]; // Store the x-values here.
  double yValues[PLOT_VALUE_MAX_COUNT]; // Store the y-values here.
  for (uint16_t testPeriodIndex = 0;
       testPeriodIndex < FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT;
       testPeriodIndex++) {
    firPower = 0.0; // Reset the power value.
    uint16_t currentPeriodTickCount =
        filterTest_firTestTickCounts[testPeriodIndex];
    uint32_t totalTickCount = 0; // Keep track of where you are in the period.
    // The #ifdef below allow the code to adapt to early usage when only the
    // filter.c code is being tested up through late-end verification when the
    // isr.c, detector.c and so forth have been coded and are being tested.
    while (
        totalTickCount <
        FILTER_TEST_PULSE_WIDTH_LENGTH) { // Keep going until you have completed
                                          // the entire pulse-width.
      for (uint16_t freqTick = 0; freqTick < currentPeriodTickCount;
           freqTick++) { // This loop completes a single period.
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
        uint16_t adcValue;
        adcValue = computeAdcBufferInput(freqTick, currentPeriodTickCount);
        isr_addDataToAdcBuffer(adcValue); // Put the data into the adcBuffer.
#else
        double filterValue;
        filterValue = computeFilterInput(freqTick, currentPeriodTickCount);
        filter_addNewInput(
            filterValue); // Put the data into the input queue of the filter.
        if (filterTest_decimatingFirFilter()) {
          double firOutput = filterTest_readMostRecentValueFromQueue(
              filter_getYQueue());           // Get the output from the yQueue.
          firPower += firOutput * firOutput; // Compute the power so far.
        }
#endif
        if (plotInputFlag &&
            (totalTickCount <
             currentPeriodTickCount *
                 PERIODS_TO_PLOT)) { // You can capture multiple periods of
                                     // data.
          xValues[totalTickCount] =
              totalTickCount; // You just need x to increment.
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
          yValues[totalTickCount] = adcValue; // y is the adcValue.
#else
          yValues[totalTickCount] =
              filterValue +
              FILTER_TEST_INPUT_OFFSET; // The offset ensure that the input is
                                        // unipolar for plotting purposes.
#endif
        }
        totalTickCount++; // This keeps track of the total ticks in the
                          // pulse-width.
      }
    }
    if (plotInputFlag) { // Only plot the data if the flag is true.
      printf("plotting input square wave.\n\r");
      filterTest_plotInputValues(xValues, yValues,
                                 currentPeriodTickCount * PERIODS_TO_PLOT);
      utils_msDelay(INPUT_PLOT_VIEW_DELAY);
    }
#ifdef DETECTOR_H_
    bool interruptsEnabled = false; // Need to tell the detector that interrupts
                                    // are not currently enabled.
    detector(interruptsEnabled, false); // Run the detector so that it runs the
                                        // decimating FIR and IIR filters.
    for (queue_index_t i = 0;
         i < queue_elementCount(filter_getFirOutputDebugQueue());
         i++) { // Iterate over the FIR output debug queue.
      double firOutput = queue_readElementAt(filter_getFirOutputDebugQueue(),
                                             i); // Read an output.
      firPower += firOutput * firOutput;         // Square the output.
    }
#endif
    testPeriodPowerValue[testPeriodIndex] =
        firPower; // Store the resulting power.
    printf("freqCount:%d, testPeriodPowerValue:%le\n\r", freqCount,
           testPeriodPowerValue[testPeriodIndex]); // Info. print.
    freqCount++;
  }
  // After running all of the data through the filters, plot it out.
  printf("Plotting response to square-wave input.\n\r");
  filterTest_plotFirFrequencyResponse(testPeriodPowerValue);
}

// Plots the output power for a given filter across the standard 10 user
// frequencies. iirPowerValues[] contains the computed power for
// iir-filter(filterNumber) for all 10 user frequencies. Histogram bars are
// drawn in red and blue. Red bars represent frequencies where you want a
// minimal response. Blue histogram bars represent frequencies where you want a
// maximal response (when filterNumber matches the user-frequency).
// iirPowerValues are indexed using user-frequency numbers:
// iirPowerValue[0] contains computed power for user-frequency 0, for
// iir-filter(filterNumber). iirPowerValue[9] contains computed power for
// user-frequency 9, for iir-filter(filterNumber).
void filterTest_plotIirFrequencyResponse(double iirPowerValues[],
                                         uint16_t filterNumber) {
  // Make a copy of the power values that you will normalize.
  double normalizedPowerValue[FILTER_FREQUENCY_COUNT];
  for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    normalizedPowerValue[i] = iirPowerValues[i];
  }
  filterTest_normalizeArrayValues(normalizedPowerValue, iirPowerValues,
                                  FILTER_FREQUENCY_COUNT);
  histogram_init(FILTER_FREQUENCY_COUNT);
  // Set labels and colors (red) for the standard frequencies 0-9.
  // Default is red but will change the color for the desired frequency to be
  // blue.
  for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    histogram_setBarColor(i, DISPLAY_RED);
    char tempLabel[MAX_BUF];
    snprintf(tempLabel, MAX_BUF, "%d", i);
    histogram_setBarLabel(i, tempLabel);
  }
  histogram_setBarColor(filterNumber,
                        DISPLAY_BLUE); // Desired frequency drawn in blue.
  for (uint16_t barIndex = 0; barIndex < FILTER_FREQUENCY_COUNT; barIndex++) {
    char label[HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS]; // Get a buffer for
                                                            // the label.
    // Create the top-label, based upon the actual power value. Use floor() +0.5
    // to round the number instead of truncate.
    if (snprintf(label, HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS, "%0.0e",
                 floor(iirPowerValues[barIndex]) + ONE_HALF_FP(1.0)) == -1)
      printf("Error: snprintf encountered an error during conversion.\n\r");
    // Pull out the 'e' from the exponent to make better use of your characters.
    trimLabel(label);
    histogram_setBarData(barIndex,
                         normalizedPowerValue[barIndex] *
                             HISTOGRAM_MAX_BAR_DATA_IN_PIXELS,
                         label); // No top-label.
  }
  histogram_redrawBottomLabels(); // Need to redraw the bottom labels because I
                                  // changed the colors.
  histogram_updateDisplay();      // Redraw the histogram.
}

// Fills the queue with the fillValue, overwriting all previous contents.
void filterTest_fillQueue(queue_t *q, double fillValue) {
  for (queue_size_t i = 0; i < q->size; i++) {
    queue_overwritePush(q, fillValue);
  }
}

// Plots frequency response for the selected filterNumber against the 10
// standard frequencies (square-wave). Plots the IIR power for a specific
// filterNumber for the supplied iirPowerValues. IIR outputs are retrieved via
// filter_getIirOutputQueue(filterNumber). Power is computed internally. Does
// not use the filter_compute... functions to compute power.
#define FILTER_IIR_POWER_TEST_PERIOD_COUNT FILTER_FREQUENCY_COUNT
void filterTest_runSquareWaveIirPowerTest(uint16_t filterNumber,
                                          bool printMessageFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return;
  }
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST
  detector_init(); // You will be using the detector.
  isr_init(); // You will be using adcBuffer to provide data to the detector.
#endif
  if (printMessageFlag) {
    printf("running filter_runFirPowerTest(%d) - plotting power for all player "
           "frequencies for IIR filter(%d) to TFT display.\n\r",
           filterNumber, filterNumber);
  }
  double testPeriodPowerValue
      [FILTER_IIR_POWER_TEST_PERIOD_COUNT]; // Keep track of power values here.
  uint16_t freqCount = 0;
  // Simulate running everything at 100 kHz.
  for (uint16_t testPeriodIndex = 0; testPeriodIndex < FILTER_FREQUENCY_COUNT;
       testPeriodIndex++) { // Only use the first 10 standard frequencies.
    double power = 0.0;
    filterTest_fillQueue(filter_getXQueue(), 0.0); // zero out the x-queue.
    filterTest_fillQueue(filter_getYQueue(), 0.0); // zero out the x-queue.
    filterTest_fillQueue(
        filter_getZQueue(filterNumber),
        0.0); // zero out the z-queue for the IIR filter under test.
    uint16_t currentPeriodTickCount =
        filterTest_firTestTickCounts[testPeriodIndex]; // You will be generating
                                                       // a frequency with this
                                                       // period.
    uint32_t totalTickCount =
        0; // Keep track of where you are in the given period.
    while (
        totalTickCount <
        FILTER_TEST_PULSE_WIDTH_LENGTH) { // Generate a pulse-width of periods.
      for (uint16_t freqTick = 0; freqTick < currentPeriodTickCount;
           freqTick++) { // This loop generates one period of the frequency.
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST // from adc-buffer to detector.
        uint16_t adcValue;
        adcValue = computeAdcBufferInput(freqTick, currentPeriodTickCount);
        isr_addDataToAdcBuffer(adcValue); // Put the data into the adcBuffer.
#else // From fir-filter input to iir-filter output only.
        double filterValue; // Values will be fed directly to the filter.
        double
            iirOutput; // This will hold the latest output from the iir filter.
        filterValue = computeFilterInput(
            freqTick, currentPeriodTickCount); // Compute a filter value.
        filter_addNewInput(
            filterValue); // Put the data into the input queue of the filter.
        if (filterTest_decimatingFirFilter()) { // Run the IIR filter if the
                                                // fir-filter ran.
          filter_iirFilter(filterNumber);
          // Get the latest output from the iir-filter.
          iirOutput = filterTest_readMostRecentValueFromQueue(
              filter_getZQueue(filterNumber));
          power += iirOutput *
                   iirOutput; // Multiply-accumulate the iir-filter output
        }
#endif
        totalTickCount++; // Go to the next tick.
      }
    }
#ifdef ADC_THROUGH_DETECTOR_FILTER_TEST // Run the detector to run the filters.
    bool interruptsEnabled = false; // Need to tell the detector that interrupts
                                    // are not currently enabled.
    detector(interruptsEnabled, false); // Run the detector so that it runs the
                                        // decimating FIR and IIR filters.
    for (queue_index_t i = 0;
         i < queue_elementCount(filter_getIirOutputQueue(filterNumber));
         i++) { // Iterate over the IIR output debug queue.
      double iirOutput = queue_readElementAt(
          filter_getIirOutputQueue(filterNumber), i); // Read an output.
      power += iirOutput * iirOutput;                 // Square the output.
    }
#endif
    testPeriodPowerValue[testPeriodIndex] =
        power;   // keep track of the power for each frequency.
    freqCount++; // Next frequency.
  }
  filterTest_plotIirFrequencyResponse(
      testPeriodPowerValue, filterNumber); // Finally, plot the results.
}

// Pushes a single 1.0 through the xQueue. Golden output data are just the FIR
// coefficients in reverse order. If this test passes, you are multiplying the
// coefficient with the correct element of xQueue. This is equivalent to passing
// the filter over an input containing only a delta function and thus returns
// the impulse response.
bool filterTest_runFirAlignmentTest(bool printMessageFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return false;
  }
  bool success = true;                           // Be optimistic.
  filterTest_fillQueue(filter_getXQueue(), 0.0); // zero-out the xQueue.
  filter_addNewInput(1.0); // Place a single 1.0 in the xQueue.
  for (uint32_t i = 0; i < filter_getFirCoefficientCount();
       i++) { // Push the single 1.0 through the queue.
    double firValue = filter_firFilter(); // Run the FIR filter.
    double firGoldenOutput =
        filter_getFirCoefficientArray()[i]; // Golden output is simply the FIR
                                            // coefficient.
    if (!filterTest_floatingPointEqual(
            firValue,
            firGoldenOutput)) { // If the output from the FIR filter does not
                                // match the golden value, print an error.
      success = false;          // The test failed.
      printf("filter_runAlignmentTest: Output from FIR Filter(%20.24le) does "
             "not match test-data(%20.24le).\n\r",
             firValue, firGoldenOutput);
    }
    filter_addNewInput(
        0.0); // Shift the 1.0 value over one position in the queue.
  }
  // Print informational messages.
  if (printMessageFlag) {
    printf("filter_runFirAlignmentTest ");
    if (success)
      printf("passed.\n\r");
    else
      printf("failed.\n\r");
  }
  return success; // Return the success of failure of this test.
}

// Pushes a series of 1.0 values though xQueue. Golden output data is the sum of
// the coefficients in reverse order. The FIR-filter is probably computing
// outputs correctly if you pass this test.
bool filterTest_runFirArithmeticTest(bool printMessageFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return false;
  }
  bool success = true;                       // Be optimistic.
  filter_fillQueue(filter_getXQueue(), 0.0); // zero-out the xQueue.
  double firGoldenOutput =
      0.0; // You will compute the golden output by accumulating the FIR
           // coefficients in reverse order.
  for (uint32_t i = 0; i < filter_getFirCoefficientCount();
       i++) { // Loop enough times to go through the coefficients.
    double newTestInput = 1.0;            // Only value in the xQueue is 1.0.
    filter_addNewInput(newTestInput);     // Add a 1.0 in the xQueue.
    double firValue = filter_firFilter(); // Run the FIR filter.
    firGoldenOutput +=
        newTestInput *
        filter_getFirCoefficientArray()[i]; // Golden output is simply the
                                            // coefficient.
    if (!filterTest_floatingPointEqual(
            firValue,
            firGoldenOutput)) { // Test fails if the FIR output does not match
                                // the computed golden value. Print message to
                                // note failure.
      success = false;          // Test failed.
      printf("filter_runArithmeticTest: Output from FIR Filter(%24.20le) does "
             "not match test-data(%24.20le) at index(%d).\n\r",
             firValue, firGoldenOutput, i);
    }
  }
  // Print informational messages.
  if (printMessageFlag) {
    printf("filter_runFirArithmeticTest ");
    if (success)
      printf("passed.\n\r");
    else {
      printf("failed.\n\r");
      printf("Ensure that your FIR coefficients have at least 20 significant "
             "digits.\n\r");
    }
  }
  return success; // Return the success or failure of the test.
}

// This test checks to see that the B coefficients are multiplied with the
// correct values of the yQueue. If it passes, the coefficients are properly
// aligned with the data in yQueue. This test only checks the coefficients for
// filterNumber (frequency).
bool filterTest_runIirBAlignmentTest(uint16_t filterNumber,
                                     bool printMessageFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return false;
  }
  bool success = true;                       // Be optimistic.
  filter_fillQueue(filter_getYQueue(), 0.0); // zero-out the yQueue.
  filter_fillQueue(filter_getZQueue(filterNumber),
                   0.0); // zero out the zQueue for filterNumber.
  queue_overwritePush(filter_getYQueue(),
                      1.0); // Place a single 1.0 in the yQueue.
  for (uint32_t i = 0; i < filter_getIirBCoefficientCount(); i++) {
    double iirValue = filter_iirFilter(filterNumber); // Run the IIR filter.
    double iirGoldenOutput = filter_getIirBCoefficientArray(
        filterNumber)[i]; // Golden output is simply the coefficient.
    if (!filterTest_floatingPointEqual(
            iirValue, iirGoldenOutput)) { // Make sure they IIR output matches
                                          // the golden output.
      success = false; // Note test failure and print message.
      printf("filter_runIirBlignmentTest: Output from IIR Filter[%d](%24.20le) "
             "does not match test-data(%24.20le) at index(%d).\n\r",
             filterNumber, iirValue, iirGoldenOutput, i);
    }
    filter_fillQueue(filter_getZQueue(filterNumber),
                     0.0); // zero out the zQueue for filterNumber so the
                           // A-summation is always 0.
    queue_overwritePush(filter_getYQueue(),
                        0.0); // Shift the 1.0 over one position in the yQueue.
  }
  // Print informational messages.
  if (printMessageFlag) {
    printf("filter_runIirBAlignmentTest ");
    if (success)
      printf("passed.\n\r");
    else
      printf("failed.\n\r");
  }
  return success; // Return the success or failure of the test.
}

// Returns the starting index for the A-coefficient array for the IIR-filter.
// Some students use the leading 1, others did not.
uint16_t filterTest_getIirACoefficientArrayStartingIndex() {
#ifdef FILTER_TEST_USED_LEADING_1_IN_IIR_A_COEFFICIENT_ARRAY
  return 1;
#else
  return 0;
#endif
}

// Checks to see that the A-coefficients are properly aligned with the zQueue.
// This test checks the A-coefficients for a specific filter-number (frequency).
// It is a little more work to create a sliding 1 in the zQueue because the IIR
// updates the zQueue. Thus as you move across coefficients, the zQueue is
// cleared out. 1.0 is added and shifted over the correct number of spaces.
bool filterTest_runIirAAlignmentTest(uint16_t filterNumber,
                                     bool printMessageFlag) {
  if (!filterTest_initFlag) {
    printf("Must call filterTest_init() before running any filter tests.\n\r");
    return false;
  }
  bool success = true; // Be optimistic.
  filter_fillQueue(filter_getYQueue(),
                   0.0); // zero-out the yQueue so the B-summation is always 0.
  uint16_t startingIndex =
      filterTest_getIirACoefficientArrayStartingIndex(); // Varies according to
                                                         // student.
  for (uint32_t i = 0; i < filter_getIirACoefficientCount() - startingIndex;
       i++) { // Loop through all of the A-coefficients.
    filter_fillQueue(filter_getZQueue(filterNumber),
                     0.0); // zero out the zQueue for filterNumber.
    queue_overwritePush(filter_getZQueue(filterNumber),
                        1.0);          // Add a single 1.0 to the queue.
    for (uint32_t j = 0; j < i; j++) { // Move over the 1.0 an additional
                                       // position each time through the loop.
      queue_overwritePush(
          filter_getZQueue(filterNumber),
          0.0); // Move the 1.0 over by writing the correct number of zeros.
    }
    double iirValue = filter_iirFilter(filterNumber); // Run the IIR filter.
    double iirGoldenOutput = filter_getIirACoefficientArray(
        filterNumber)[i + startingIndex]; // Golden output is simply the
                                          // coefficient.
    if (!filterTest_floatingPointEqual(
            iirValue,
            -iirGoldenOutput)) { // Check to see that the IIR-output matches the
                                 // correct coefficient value.
      success =
          false; // Note the failure of the test and print an info message.
      printf(
          "filter_runIirAAlignmentTest: Output from IIR Filter[%d](%20.24le) "
          "does not match test-data(%20.24le) at index(%d).\n\r",
          filterNumber, iirValue, -iirGoldenOutput, i + startingIndex);
    }
  }
  // Print informational messages.
  if (printMessageFlag) {
    printf("filter_runIirAAlignmentTest ");
    if (success)
      printf("passed.\n\r");
    else
      printf("failed.\n\r");
  }
  return success; // Return the failure or success of the test.
}

// Normalizes the values in the array argument.
// void filterTest_normalizeArrayValues(double *array, uint16_t size) {
//   // Find the maximum value
//   uint16_t maxIndex = 0;
//   // Find the index of the max value in the array.
//   for (int i = 0; i < size; i++) {
//     if (array[i] > array[maxIndex])
//       maxIndex = i;
//   }
//   double maxPowerValue = array[maxIndex];
//   // Normalize everything between 0.0 and 1.0, based upon the max value.
//   for (int i = 0; i < size; i++)
//     array[i] = array[i] / maxPowerValue;
// }

// Converts rand into a value between 0 and 1.
double filterTest_randomValue0To1() {
  return ((double)rand()) / ((double)RAND_MAX);
}

// Test function that completely fills a queue with random values.
void filterTest_fillQueueWithRandomValues(queue_t *q) {
  for (uint16_t i = 0; i < queue_size(q); i++) // Iterate across all elements.
    queue_overwritePush(
        q, filterTest_randomValue0To1()); // Use rand to generate random values.
}

// Golden compute power function.
// Accepts a queue and computes the power value by computing the sum of the
// squares. Returns the computer power value.
double filterTest_computeGoldenPowerValue(queue_t *q) {
  double powerValue = 0.0; // Result held here.
  // queueSizeOffset = 0 if output-queues are sized 2000, = 1 if you are storing
  // an extra value in the queue to hold the oldest value each time for a power
  // computation. You need to skip over that old value when computing the golden
  // power value.
  queue_size_t queueSizeOffset = 0;
#ifdef FILTER_TEST_STORE_OLD_VALUE_IN_QUEUE
  queueSizeOffset = 1;
#endif
  for (uint16_t i = queueSizeOffset; i < queue_size(q);
       i++) { // Iterate over all elements.
    double elementValue =
        queue_readElementAt(q, i);               // Read the value from queue.
    powerValue += (elementValue * elementValue); // Compute sum of squares.
  }
  return powerValue;
}

// Performs a test of the filter_computePower() function.
// This test:
// 1. fills all 10 IIR output queues with random values,
// 2. compares the results of filter_computePower with a golden computed output
//    for all 10 output queues.
// Tests both forced and incremental modes.
#define TEST_PASS_EPSILON 10E-11 // Should be in this range.
#define TEST_INCREMENTAL_LOOP_COUNT                                            \
  3000 // Loop over the incremental test this many times.
#define OUTPUT_QUEUE_SIZE 2000
bool filterTest_runPowerTest() {
  bool firstComputeStatus = true; // Be optimistic.
  filter_init();
  printf("===== Starting filter_runPowerTest() =====\n\r");
  printf(
      "Testing to see that the power is computed correctly when forced.\n\r");
  // This tests starting from the beginning.
  for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    queue_t *q = filter_getIirOutputQueue(i); // Get the output queue.
    // Check to make sure that the output queue is the correct size.
    queue_size_t size;
    // queueSizeOffset = 0 if output-queues are sized 2000, = 1 if you are
    // storing an extra
    /// value in the queue to hold the oldest value each time for a power
    /// computation.
    queue_size_t queueSizeOffset = 0;
#ifdef FILTER_TEST_STORE_OLD_VALUE_IN_QUEUE
    queueSizeOffset = 1;
#endif
    if ((size = queue_size(q)) != (OUTPUT_QUEUE_SIZE + queueSizeOffset)) {
      printf("Output queue for queue number %d is incorrect (%d). It should "
             "be %d\n\r",
             i, size, (OUTPUT_QUEUE_SIZE + queueSizeOffset));
      printf("Fix this problem before proceeding.\n\r");
      return false;
    }
    filterTest_fillQueueWithRandomValues(
        q); // Fill the queue with random values.
    double goldenValue =
        filterTest_computeGoldenPowerValue(q); // Compute the golden value.
    // Compute power with the filter function.
    double testValue = filter_computePower(
        i, true, false);            // true, false = no force, no debug print.
    if (testValue != goldenValue) { // Check for errors.
      printf("filter_runPowerTest failed for index: %d: , golden value: %lf, "
             "filter_computePower(): %lf\n\r",
             i, goldenValue, testValue);
      firstComputeStatus = false; // Keep track of pass/fail.
      break;
    }
  }
  if (firstComputeStatus)
    printf("Output queues are the correct size.\n\r");
  printf("Power values were properly computed when forced.\n\r");
  // Add a single value to each output queue and try again.
  // This tests the incremental computation.
  printf("Testing to see that the power is computed correctly incrementally "
         "over %d trials.\n\r",
         TEST_INCREMENTAL_LOOP_COUNT);
  bool incrementalComputeStatus =
      true; // Be optimistic for the incremental computation.
  // Perform many incrementat power computations.
  for (uint32_t loopCount = 0; loopCount < TEST_INCREMENTAL_LOOP_COUNT;
       loopCount++) {
    for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
      queue_t *q = filter_getIirOutputQueue(i); // Get an output queue.
      queue_overwritePush(
          q,
          filterTest_randomValue0To1()); // Add a random value to every queue.
      double goldenValue =
          filterTest_computeGoldenPowerValue(q); // Compute the golden value.
      double testValue = filter_computePower(
          i, false, false); // false, false = no force, no debug print.
      if (fabs(testValue - goldenValue) >
          TEST_PASS_EPSILON) { // See if the value is in error beyond some
                               // epsilon.
        printf("Loop count:%d\n\r",
               loopCount); // Print out the current loop count for reference.
        // Print out values that indicates the failure.
        printf("filter_runPowerTest failed for index: %d\n\rgolden value:      "
               "    %lf\n\rfilter_computePower(): %20.24lf\n\r",
               i, goldenValue, testValue);
        printf("Difference between golden value and incrementally computed "
               "value: %20.24le\n\r",
               fabs(goldenValue - testValue));
        incrementalComputeStatus = false; // Keep track of status.
      }
    }
  }
  if (incrementalComputeStatus) // Print OK message if there were no errors.
    printf("Power values were properly computed incrementally.\n\r");
  printf("+++++ Exiting filter_runPowerTest +++++\n\r");
  // Return the combined status for both the first-compute test and the
  // incremental test.
  return firstComputeStatus & incrementalComputeStatus;
}

// Copies powerValues to currentPowerValues, the same array
// that is used to hold the values after power has been computed
// by filter_computePower().
// void filterTest_setCurrentPowerValues(double powerValues[]) {
//  for (uint32_t i=0; i<FILTER_FREQUENCY_COUNT; i++) {
//    filter_getCurrentPowerValueArray()[i] = powerValues[i];
//  }
//}

// Performs several tests of the filter code.
// 1. Test alignment of FIR constants with input.
// 2. Test the arithmetic performed by the FIR filter.
// 3. Test alignment of the IIR A and B coefficients.
// 4. Plots the frequency response of the FIR filter on the TFT display.
// 5. Plots the frequency response of each of the IIR bandpass filters on the
// TFT display. Returns true if all tests passed, false otherwise. Various
// informational prints are provided in the console during the run of the test.
#define TEN_SECONDS 10000
#define TWO_SECONDS 2000
#define FOUR_SECONDS 4000
#define PRINT_INFO_MESSAGES true
#define DONT_PRINT_INFO_MESSAGES false
#define PLOT_INPUT false
#define TEST_IIR_FILTER_NUMBER 0
bool filterTest_runTest() {
  printf("******** filterTest_runTest() **********\n");
  bool success = true; // Be optimistic.
  filter_init();       // Always must init stuff.
  filterTest_init();   // More init stuff.
  // Confirm that the FIR coefficients are properly aligned with the incoming
  // data.
  success &= filterTest_runFirAlignmentTest(PRINT_INFO_MESSAGES);
  // Confirm that the FIR properly computes its output.
  success &= filterTest_runFirArithmeticTest(PRINT_INFO_MESSAGES);
  // Confirm that the IIR A coefficients are properly aligned with the incoming
  // data.
  success &= filterTest_runIirAAlignmentTest(TEST_IIR_FILTER_NUMBER,
                                             PRINT_INFO_MESSAGES);
  // Confirm that the IIR B coefficients are properly aligned with the incoming
  // data.
  success &= filterTest_runIirBAlignmentTest(TEST_IIR_FILTER_NUMBER,
                                             PRINT_INFO_MESSAGES);
  // Verifies correct functionality of the power computation.
  success &= filterTest_runPowerTest();
  // Plots the frequency response of the FIR filter against all user and other
  // test frequencies. All frequencies are expressed as a square wave.
  filterTest_runSquareWaveFirPowerTest(PRINT_INFO_MESSAGES, PLOT_INPUT);
  utils_msDelay(FOUR_SECONDS); // Leave on the display for a couple of seconds.
  for (int i = 0; i < FILTER_FREQUENCY_COUNT;
       i++) { // Plot all 10 IIR filters against the test freqs.
    filterTest_runSquareWaveIirPowerTest(
        i, true);               // This plots the individual filter response.
    utils_msDelay(TWO_SECONDS); // Leave on the display for a few seconds.
  }
  return success;
}

// Plots the frequency response of the FIR filter on the TFT.
// Everything is defined assuming a 100 kHz sample rate.
// Frequencies run from 1.1 kHz to 50 kHz.
// Output values are retrieved from a FIR debug queue (see
// filter_getFirOutputDebugQueue()). Power is computed internally. Does not use
// the filter_computePower... functions.
// void filterTest_runSinusoidalWaveFirPowerTest(bool printMessageFlag, bool
// plotInputFlag) {
//  detector_init();  // You will be using the detector.
//  isr_init();       // You will be using adcBuffer to provide data to the
//  detector. if (printMessageFlag) {
//    // Tells you that this function is plotting the frequency response for the
//    FIR filter for a set of frequencies. printf("running
//    filter_runFirPowerTest() - plotting power values (frequency response) for
//    frequencies %1.2lf kHz to %1.2lf kHz for FIR filter to TFT display.\n\r",
//        ((double)
//        ((FILTER_SAMPLE_FREQUENCY_IN_KHZ))/filterTest_firTestTickCounts[0]),
//        ((double)
//        ((FILTER_SAMPLE_FREQUENCY_IN_KHZ))/filterTest_firTestTickCounts[FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT-1]));
//  }
//  double testPeriodPowerValue[FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT];  //
//  Computed power values will go here. uint16_t freqCount=0; // Used to print
//  info message.
//  // Simulate running everything at 100 kHz. Simply add either 1.0 or -1.0 to
//  xQueue based upon the
//  // the frequency you are simulating.
//  // Iterate over all of the test-periods.
//  for (uint16_t testPeriodIndex=0;
//  testPeriodIndex<FILTER_TEST_FIR_POWER_TEST_PERIOD_COUNT; testPeriodIndex++)
//  {
//    uint16_t currentPeriodTickCount =
//    filterTest_firTestTickCounts[testPeriodIndex];   // Get the current
//    ticksPerPeriodCount. double sinusoidalStep = (2.0 * M_PI) / ((double)
//    currentPeriodTickCount);         // Compute how to step x for sin(x).
//    uint32_t totalTickCount = 0;  // Keep track of where you are in the
//    period.
//    // Plotting the sinusoid.
//    double xValues[PLOT_VALUE_MAX_COUNT]; // Store the x-values here.
//    double yValues[PLOT_VALUE_MAX_COUNT]; // Store the y-values here.
//    double sinusoidalValue = 0.0;
//    while (totalTickCount < FILTER_TEST_PULSE_WIDTH_LENGTH) { // Keep going
//    until you have completed the entire pulse-width.
//      for (uint16_t freqTick = 0; freqTick < currentPeriodTickCount;
//      freqTick++) {  // This loop completes a single period.
//        if (currentPeriodTickCount == 2)  // Special case, only 2
//        samples/period (Nyquist), make sure non-zero by shifting over PI/2.
//          sinusoidalValue = sin(freqTick * sinusoidalStep + M_PI/2);
//        else
//          sinusoidalValue = sin(freqTick * sinusoidalStep);       // Step
//          value in sine value.
//        uint16_t adcValue;
//        adcValue = ((sinusoidalValue + 1.0)/2.0) * MAX_ADC_VALUE; // Convert
//        to a legal ADC value. isr_addDataToAdcBuffer(adcValue); // Send the
//        value to the ADC buffer. if (plotInputFlag && (plotInputFlag &
//        (totalTickCount < currentPeriodTickCount*PERIODS_TO_PLOT))) {
//          xValues[totalTickCount] = totalTickCount; // x-values only need to
//          increment. yValues[totalTickCount] = adcValue;       // y-values are
//          the adc-values.
//        }
//        totalTickCount++; // Keep track of the total number of ticks in the
//        pulse-width.
//      }
//    }
//    if (plotInputFlag) {  // Only plot the input signals if the flag is true.
//      printf("plotting input sinusoid\n\r");
//      filterTest_plotInputValues(xValues, yValues,
//      currentPeriodTickCount*PERIODS_TO_PLOT);
//      utils_msDelay(INPUT_PLOT_VIEW_DELAY);
//    }
//    bool interruptsEnabled = false; // Need to tell the detector that
//    interrupts are not currently enabled. detector(interruptsEnabled, false);
//    // Run the detector so that it runs the decimating FIR and IIR filters.
//    double firPower = 0.0;          // Accumulate power here.
//    for (queue_index_t i=0;
//    i<queue_elementCount(filter_getFirOutputDebugQueue()); i++) { // Iterate
//    over the FIR output debug queue.
//      double firOutput = queue_readElementAt(filter_getFirOutputDebugQueue(),
//      i);         // Read an output. firPower += firOutput * firOutput; //
//      Square the output.
//    }
//    testPeriodPowerValue[testPeriodIndex] = firPower; // Store the resulting
//    power. printf("freqCount:%d, testPeriodPowerValue:%le\n\r", freqCount,
//    testPeriodPowerValue[testPeriodIndex]); // Info. print. freqCount++;
//  }
//  printf("Plotting FIR frequency response to sinusoidal inputs.\n\r");
//  filterTest_plotFirFrequencyResponse(testPeriodPowerValue);
//}
