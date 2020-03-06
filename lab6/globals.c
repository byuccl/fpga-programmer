#include "globals.h"

// The length of the sequence.
// The static keyword means that globals_sequenceLength can only be accessed
// by functions contained in this file. 
static uint16_t globals_sequenceLength = 0;  // The length of the sequence.

// This is the length of the sequence that you are currently working on, 
// not the maximum length but the interim length as
// the user works through the pattern one color at a time.
void globals_setSequenceIterationLength(uint16_t length) {globals_sequenceIterationLength = length;}
  
// You will need to implement the other functions.