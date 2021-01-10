/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#define TRANSMITTER_OUTPUT_PIN 13     // JF1 (pg. 25 of ZYBO reference manual).
#define TRANSMITTER_PULSE_WIDTH 20000 // Based on a system tick-rate of 100 kHz.
#include <stdbool.h>
#include <stdint.h>

// The transmitter state machine generates a square wave output at the chosen
// frequency as set by transmitter_setFrequencyNumber(). The step counts for the
// frequencies are provided in filter.h

// Standard init function.
void transmitter_init();

// Starts the transmitter.
void transmitter_run();

// Returns true if the transmitter is still running.
bool transmitter_running();

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber);

// Returns the current frequency setting.
uint16_t transmitter_getFrequencyNumber();

// Standard tick function.
void transmitter_tick();

// Tests the transmitter.
void transmitter_runTest();

// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise,
// transmits one pulse-width and stops. To set continuous mode, you must invoke
// this function prior to calling transmitter_run(). If the transmitter is in
// currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false. It can stop immediately or wait
// until the last 200 ms pulse is complete. NOTE: while running continuously,
// the transmitter will change frequencies at the end of each 200 ms pulse.
void transmitter_setContinuousMode(bool continuousModeFlag);

// Tests the transmitter in non-continuous mode.
// The test runs until BTN1 is pressed.
// To perform the test, connect the oscilloscope probe
// to the transmitter and ground probes on the development board
// prior to running this test. You should see about a 300 ms dead
// spot between 200 ms pulses.
// Should change frequency in response to the slide switches.
void transmitter_runNoncontinuousTest();

// Tests the transmitter in continuous mode.
// To perform the test, connect the oscilloscope probe
// to the transmitter and ground probes on the development board
// prior to running this test.
// Transmitter should continuously generate the proper waveform
// at the transmitter-probe pin and change frequencies
// in response to changes to the changes in the slide switches.
// Test runs until BTN1 is pressed.
void transmitter_runContinuousTest();

#endif /* TRANSMITTER_H_ */
