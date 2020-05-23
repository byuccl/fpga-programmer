/*
 * simonControl.h
 *
 *  Created on: Oct 29, 2014
 *      Author: hutch
 */

#ifndef SIMONCONTROL_H_
#define SIMONCONTROL_H_

// Used to init the state machine. Always provided though it may not be
// necessary.
void simonControl_init();

// Standard tick function.
void simonControl_tick();

// Enables the control state machine.
void simonControl_enable();

// Disables the controll state machine.
void simonControl_disable();

// If you want to stop the game after a mistake, check this function after each
// tick.
bool simonControl_isGameOver();

// Use this to set the sequence length. This the sequence set by this function
// will only be recognized when the controller passes through its init state.
void simonControl_setSequenceLength(uint16_t length);

#endif /* SIMONCONTROL_H_ */
