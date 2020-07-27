/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/
#ifndef CLOCKCONTROL_H
#define CLOCKCONTROL_H

// Standard tick function.
void clockControl_tick();

// Call this before you call clockControl_tick().
void clockControl_init();

#endif /* CLOCKCONTROL_H */