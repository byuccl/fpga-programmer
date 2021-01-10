/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef RUNNINGMODES_H_
#define RUNNINGMODES_H_

#define RUNNING_MODES_TWO_TEAM_TEAM0_FREQUENCY                                 \
  6 // Team-0 players shoot at this frequency.
#define RUNNING_MODES_TWO_TEAM_TEAM1_FREQUENCY                                 \
  9 // Team-1 players shoot at this frequency.

#include <stdint.h>

// This mode runs continously until btn3 is pressed.
// When btn3 is pressed, it exits and prints performance information to the TFT.
// During operation, it continously displays that received power on each
// channel, on the TFT. Frequency is selected via the slide-switches.
void runningModes_continuous();

// This mode runs continously until btn3 is pressed.
// When btn3 is pressed, it exits and prints performance information to the TFT.
// Game-playing mode. Each shot is registered on the histogram on the TFT.
// Press BTN0 or the gun-trigger to shoot.
// Frequency is selected via the slide-switches.
void runningModes_shooter();

// This mode supports two teams, Team-A and Team-B.
// Each team operates on its own configurable frequency that can be redefined
// in globalDefines.h.
// Each player has a fixed set of lives (globalDefines.h) and once they
// have expended all lives, their ceases operation and they are told
// to return to base to await the ultimate end of the game.
// The gun is clip-based and each clip contains a fixed number of shots
// (globalDefines.h) that takes a short time to reload a new clip
// (globalDefines.h). The clips are automatically loaded.
void runningModes_twoTeams();

void runningModes_zombie();

// Continuously cycles through all channels, shooting one pulse per channel.
void runningModes_testShootAllChannels();

// Returns the current switch-setting
uint16_t runningModes_getFrequencySetting();

// A simple test mode that continuously prints out raw ADC values.
void runningModes_dumpRawAdcValues();

#endif /* RUNNINGMODES_H_ */
