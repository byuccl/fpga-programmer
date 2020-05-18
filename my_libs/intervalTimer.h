// Provides an API for accessing the three hardware timers that are installed
// in the ZYNQ fabric.

#ifndef INTERVALTIMER_H_
#define INTERVALTIMER_H_

#include <stdint.h>

// Used to indicate status that can be checked after invoking the function.
typedef uint32_t
    intervalTimer_status_t; // Use this type for the return type of a function.

#define INTERVAL_TIMER_STATUS_OK 1   // Return this status if successful.
#define INTERVAL_TIMER_STATUS_FAIL 0 // Return this status if failure.

#define INTERVAL_TIMER_TIMER_0 0
#define INTERVAL_TIMER_TIMER_1 1
#define INTERVAL_TIMER_TIMER_2 2
#define INTERVAL_TIMER_MAX_NUM INTERVAL_TIMER_TIMER_2

// You must initialize the timers before you use them.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber);

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll();

// This function starts the interval timer running.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber);

// This function stops the interval timer running.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber);

// This function resets the interval timer.
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber);

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll();

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber);

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll();

// Once the interval timer has stopped running, use this function to
// ascertain how long the timer was running.
// The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber);

#endif /* INTERVALTIMER_H_ */
