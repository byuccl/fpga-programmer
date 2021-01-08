#ifndef GAMEMODES_H_
#define GAMEMODES_H_

#define GAME_MODES_TWO_TEAM_TEAM0_FREQUENCY                                    \
  6 // Team-0 players shoot at this frequency.
#define GAME_MODES_TWO_TEAM_TEAM1_FREQUENCY                                    \
  9 // Team-1 players shoot at this frequency.

#include <stdint.h>

// This mode supports two teams, Team-A and Team-B.
// Each team operates on its own configurable frequency that can be redefined
// in globalDefines.h.
// Each player has a fixed set of lives (globalDefines.h) and once they
// have expended all lives, their ceases operation and they are told
// to return to base to await the ultimate end of the game.
// The gun is clip-based and each clip contains a fixed number of shots
// (globalDefines.h) that takes a short time to reload a new clip
// (globalDefines.h). The clips are automatically loaded.
void gameModes_twoTeams();

#endif /* GAMEMODES_H_ */
