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