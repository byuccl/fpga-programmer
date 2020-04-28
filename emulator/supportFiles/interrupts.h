/*
 * interrupts.h
 *
 *  Created on: Mar 24, 2014
 *      Author: hutch
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <stdbool.h>
#include <stdint.h>

#define INTERRUPT_CUMULATIVE_ISR_INTERVAL_TIMER_NUMBER 0

typedef uint32_t u32;

  int interrupts_initAll(__attribute__((unused)) bool printFailedStatusFlag);

// User can set the load value on the private timer.
// Also updates ticks per heart beat so that the LD4 heart-beat toggle rate remains constant.
void interrupts_setPrivateTimerLoadValue(u32 loadValue);

u32 interrupts_getPrivateTimerTicksPerSecond();

// Enable/disable ARM ints.
int interrupts_enableArmInts();
int interrupts_disableArmInts();

// Starts/stops the interrupt timer.
int interrupts_startArmPrivateTimer();
int interrupts_stopArmPrivateTimer();

// Keep track of total number of times interrupt_timerIsr is invoked.
u32 interrupts_isrInvocationCount();

// Returns the number of private timer ticks that occur in 1 second.
u32 interrupts_getPrivateTimerTicksPerSecond();

// These two functions are provided mainly to remain compatible
// with code given to students.
void interrupts_enableTimerGlobalInts();
void interrupts_disableTimerGlobalInts();

void isr_function();

extern volatile int interrupts_isrFlagGlobal;

#endif /* INTERRUPTS_H_ */
