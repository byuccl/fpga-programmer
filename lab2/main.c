#include "my_libs/buttons.h"
#include "my_libs/switches.h"

int main() {
  switches_runTest();
  buttons_runTest();
}

// Interrupt routine
void isr_function() {
  // Empty for now.
}
