/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// This file is provided for use in Milestone 3, Task 2.
// Modify as you see fit.

#include "interrupts.h"

int main() {
  interrupts_initAll(true);           // main interrupt init function.
  interrupts_enableTimerGlobalInts(); // enable global interrupts.
  interrupts_startArmPrivateTimer();  // start the main timer.
  interrupts_enableArmInts(); // now the ARM processor can see interrupts.
  while (1)
    ; // Just a forever-while loop for now. Modify as you see fit.
}