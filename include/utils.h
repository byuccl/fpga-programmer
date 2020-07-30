/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/
#ifndef UTILS_H_
#define UTILS_H_

void utils_msDelay(long ms);

// Sleeps the userThread while waiting for an event, that may be:
// a timer, or a mouse-event, for example. This function is implemented
// in utils.cpp in the emulator.
void utils_sleep();
#endif /* UTILS_H_ */
