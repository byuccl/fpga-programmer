/*
 * utils.h
 *
 *  Created on: Jul 18, 2014
 *      Author: hutch
 */

#ifndef UTILS_H_
#define UTILS_H_

void utils_msDelay(long ms);

// Sleeps the userThread while waiting for an event, that may be:
// a timer, or a mouse-event, for example. This function is implemented
// in utils.cpp in the emulator.
void utils_sleep();
#endif /* UTILS_H_ */
