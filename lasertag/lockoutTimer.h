/*
 * lockoutTimer.h
 *
 *  Created on: Jan 21, 2015
 *      Author: hutch
 */

#ifndef LOCKOUTTIMER_H_
#define LOCKOUTTIMER_H_
#include <stdbool.h>

#define LOCKOUT_TIMER_EXPIRE_VALUE 50000 // Defined in terms of 100 kHz ticks.

// Calling this starts the timer.
void lockoutTimer_start();

// Perform any necessary inits for the lockout timer.
void lockoutTimer_init();

// Returns true if the timer is running.
bool lockoutTimer_running();

// Standard tick function.
void lockoutTimer_tick();

// Test function assumes interrupts have been completely enabled and
// lockoutTimer_tick() function is invoked by isr_function().
// Prints out pass/fail status and other info to console.
// Returns true if passes, false otherwise.
// This test uses the interval timer to determine correct delay for
// the interval timer.
bool lockoutTimer_runTest();

#endif /* LOCKOUTTIMER_H_ */
