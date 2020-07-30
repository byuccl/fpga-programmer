/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "my_libs/buttons.h"
#include "my_libs/switches.h"

// Run switches and buttons runTest functions
int main() {
  switches_runTest();
  buttons_runTest();

  return 0;
}

// Interrupt routine
void isr_function() {
  // Empty for now.
}
