/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef LEDTIMER_H_
#define LEDTIMER_H_

#include <stdbool.h>
#include <stdint.h>

// Provides an led timer with adjustable frequency and adjustable on/off times.
// All time-based numbers are expressed in milliseconds.
// Minimum period is 1 millisecond.

// Controls both hitLed and JF-10. Make sure to disable the hitLedTimer when
// using this timer.
#define LED_TIMER_LED_PIN 15 // This is the MIO pin number.

// Initialize the ledTimer before you use it.
void ledTimer_init();

// Starts the ledTimer running.
void ledTimer_start();

// Returns true if the timer is currently running, false otherwise.
bool ledTimer_isRunning();

// Terminates operation of the ledTimer.
void ledTimer_stop();

// Specfies how long the LED is on in milliseconds.
void ledTimer_setOnTimeInMs(uint32_t milliseconds);

// Specfies the period of the ledTimer.
void ledTimer_setPeriodInMs(uint32_t milliseconds);

// Specified the number of ticks per millisecond for the tick function.
// E.G., if ticksPerMillisecond is 10, the tick function will be called 10 times
// each millisecond.
void ledTimer_setTicksPerMs(uint32_t ticksPerMillisecond);

// Invoking this causes the led timer to also control the hit-LED.
// Flag = true means that the ledTimer will control the hitLed.
// Flag = false means that the ledTimer does not control the hitLed.
void ledTimer_controlHitLed(bool flag);

// Standard state-machine tick function. Call this to advance the state machine.
void ledTimer_tick();

// Standard test function.
void ledTimer_runTest();

// Debug function.
void ledTimer_dumpDebugValues();

#endif /* LEDTIMER_H_ */
