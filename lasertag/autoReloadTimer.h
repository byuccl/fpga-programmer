/*
 * autoReloadTimer.h
 *
 *  Created on: Jun 13, 2017
 *      Author: hutch
 */

#ifndef AUTORELOADTIMER_H_
#define AUTORELOADTIMER_H_

#include "globalDefines.h"
#include <stdbool.h>

// The auto-reload timer is always looking at the remaining shot-count from the
// trigger state-machine. When it goes to 0, it starts a configurable delay and
// after the delay expires, it sets the remaining shots to a specific value.

#ifndef AUTO_RELOAD_EXPIRE_VALUE
// Default, Defined in terms of 100 kHz ticks.
// Can be changed in the globalDefines.h file.
#define AUTO_RELOAD_EXPIRE_VALUE 300000
#endif

#ifndef AUTO_RELOAD_SHOT_VALUE
#define AUTO_RELOAD_SHOT_VALUE 10 // Default, can be changed in globalDefines.h
#endif

// Need to init things.
void autoReloadTimer_init();

// Calling this starts the timer.
void autoReloadTimer_start();

// Returns true if the timer is currently running.
bool autoReloadTimer_running();

// Disables the autoReloadTimer and reinitializes it.
void autoReloadTimer_cancel();

// Standard tick function.
void autoReloadTimer_tick();

#endif /* AUTORELOADTIMER_H_ */
