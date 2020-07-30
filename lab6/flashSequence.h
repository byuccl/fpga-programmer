/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef FLASHSEQUENCE_H_
#define FLASHSEQUENCE_H_

#include <stdbool.h>

// Turns on the state machine. Part of the interlock.
void flashSequence_enable();

// Turns off the state machine. Part of the interlock.
void flashSequence_disable();

// Standard init function.
void flashSequence_init();

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete();

// Standard tick function.
void flashSequence_tick();

#endif /* FLASHSEQUENCE_H_ */