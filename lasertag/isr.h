/*
 * isr.h
 *
 *  Created on: Jan 2, 2015
 *      Author: hutch
 */

#ifndef ISR_H_
#define ISR_H_
#include <stdint.h>

typedef uint32_t
    isr_AdcValue_t; // Used to represent ADC values in the ADC buffer.

// isr provides the isr_function() where you will place functions that require
// accurate timing. A buffer for storing values from the Analog to Digital
// Converter (ADC) is implemented in isr.c Values are added to this buffer by
// the code in isr.c. Values are removed from this queue by code in detector.c

// Performs inits for anything in isr.c
void isr_init();

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function();

// This adds data to the ADC queue. Data are removed from this queue and used by
// the detector.
void isr_addDataToAdcBuffer(uint32_t adcData);

// This removes a value from the ADC buffer.
uint32_t isr_removeDataFromAdcBuffer();

// This returns the number of values in the ADC buffer.
uint32_t isr_adcBufferElementCount();

#endif /* ISR_H_ */
