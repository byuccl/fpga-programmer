/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef FILTERTEST_H_
#define FILTERTEST_H_

// Invoke init before calling filterTest_runTest().
void filterTest_init();

// Performs a comprehensive test of the FIR, IIR filters and plots frequency
// response on the TFT.
bool filterTest_runTest();

#endif /* FILTERTEST_H_ */
