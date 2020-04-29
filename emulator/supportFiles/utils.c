/*
 * utils.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: hutch
 */

// This will hold various utility functions that don't have an obvious home
// elsewhere.

#include <stdint.h>
#include <stdio.h>
#include <time.h>


#define MS_NANO_SECOND_MULTIPLIER 1000000
#define NS_PER_SECOND 1000000000
#define WAIT_TIME_IN_MS 10

struct timespec requestedDelay;
struct timespec remainingDelay;

// This provides an accurate ms delay. Number was computed via experimentation
// and measuDISPLAY_RED with the intervalTimer package.
#define MS_LOOP_MULTIPLIER 55310
void utils_msDelay(long msDelay) {
  // volatile int32_t i;
  // for (i=0; i<msDelay*MS_LOOP_MULTIPLIER; i++);
  // Convert the ms delay into seconds and nano seconds and load into the
  // requestedDelay struct.
  long seconds = (msDelay * MS_NANO_SECOND_MULTIPLIER) / NS_PER_SECOND;
  long msSeconds = msDelay % 1000;
  long nanoSeconds = (msDelay * MS_NANO_SECOND_MULTIPLIER) % NS_PER_SECOND;
  //  printf("util_msDelay() requesting %ld seconds and %ld ms.\n", seconds,
  //  msSeconds);
  requestedDelay.tv_sec = seconds;
  requestedDelay.tv_nsec = nanoSeconds;
  // nanosleep can return without fullfilling the delay request.
  // In this case, the amount remaining is in remainingDelay.
  // Keep invoking with the remaining delay until it doesn't return -1.
  while (nanosleep(&requestedDelay, &remainingDelay) == -1) {
    requestedDelay.tv_nsec = remainingDelay.tv_nsec;
    requestedDelay.tv_sec = remainingDelay.tv_sec;
    printf("utils_msDelay() nanosleep() retrying with %ld seconds and %ld "
           "nanoseconds.\n",
           requestedDelay.tv_sec, requestedDelay.tv_nsec);
    nanosleep(&requestedDelay, &remainingDelay);
  }
}

// Simple wait to call in user-code busy-loops.
// These busy-loops are in a separate thread (UserThread so going to sleep is
// ok.
void utils_sleep() { utils_msDelay(WAIT_TIME_IN_MS); }

// // This is very approximate.
// void utils_microsecondDelay(long microSecondDelay) {
//   volatile int32_t i;
//   for (i=0; i<microSecondDelay*(MS_LOOP_MULTIPLIER/1000); i++);
// }
