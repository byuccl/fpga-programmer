/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef SIMONCONTROL_H_
#define SIMONCONTROL_H_

#include <stdbool.h>
#include <stdint.h>

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
