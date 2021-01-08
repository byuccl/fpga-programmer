/*
 * invincibilityTimer.h
 *
 *  Created on: Aug 17, 2017
 *      Author: hutch
 */

#ifndef INVINCIBILITYTIMER_H_
#define INVINCIBILITYTIMER_H_

#include <stdbool.h>
#include <stdint.h>

// Calling this starts the timer.
void invincibilityTimer_start(uint16_t seconds);

// Perform any necessary inits for the invincibility timer.
void invincibilityTimer_init();

// Returns true if the timer is running.
bool invincibilityTimer_running();

// Standard tick function.
void invincibilityTimer_tick();

#endif /* INVINCIBILITYTIMER_H_ */
