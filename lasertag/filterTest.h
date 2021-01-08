/*
 * filterTest.h
 *
 *  Created on: Mar 11, 2015
 *      Author: hutch
 */

#ifndef FILTERTEST_H_
#define FILTERTEST_H_

// Invoke init before calling filterTest_runTest().
void filterTest_init();

// Performs a comprehensive test of the FIR, IIR filters and plots frequency
// response on the TFT.
bool filterTest_runTest();

#endif /* FILTERTEST_H_ */
