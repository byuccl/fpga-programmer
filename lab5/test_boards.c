#include "minimax.h"
#include <stdio.h>

// Test the next move code, given several boards.
// You need to also create 10 boards of your own to test.
int main() {
  minimax_board_t board1; // Board 1 is the main example in the web-tutorial
                          // that I use on the web-site.
  board1.squares[0][0] = MINIMAX_O_SQUARE;
  board1.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board1.squares[0][2] = MINIMAX_X_SQUARE;
  board1.squares[1][0] = MINIMAX_X_SQUARE;
  board1.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board1.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board1.squares[2][0] = MINIMAX_X_SQUARE;
  board1.squares[2][1] = MINIMAX_O_SQUARE;
  board1.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board2;
  board2.squares[0][0] = MINIMAX_O_SQUARE;
  board2.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[0][2] = MINIMAX_X_SQUARE;
  board2.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board2.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board2.squares[2][0] = MINIMAX_X_SQUARE;
  board2.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board3;
  board3.squares[0][0] = MINIMAX_O_SQUARE;
  board3.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board3.squares[1][0] = MINIMAX_O_SQUARE;
  board3.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board3.squares[2][0] = MINIMAX_X_SQUARE;
  board3.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[2][2] = MINIMAX_X_SQUARE;

  minimax_board_t board4;
  board4.squares[0][0] = MINIMAX_O_SQUARE;
  board4.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board4.squares[2][0] = MINIMAX_X_SQUARE;
  board4.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[2][2] = MINIMAX_X_SQUARE;

  minimax_board_t board5;
  board5.squares[0][0] = MINIMAX_X_SQUARE;
  board5.squares[0][1] = MINIMAX_X_SQUARE;
  board5.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board5.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board5.squares[1][1] = MINIMAX_O_SQUARE;
  board5.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  uint8_t row, column;

  minimax_computeNextMove(&board1, true, &row,
                          &column); // true means X is current player.
  printf("next move for board1: (%d, %d)\n\r", row, column);
  minimax_computeNextMove(&board2, true, &row,
                          &column); // true means X is current player.
  printf("next move for board2: (%d, %d)\n\r", row, column);
  minimax_computeNextMove(&board3, true, &row,
                          &column); // true means X is current player.
  printf("next move for board3: (%d, %d)\n\r", row, column);
  minimax_computeNextMove(&board4, false, &row,
                          &column); // false means O is current player.
  printf("next move for board4: (%d, %d)\n\r", row, column);
  minimax_computeNextMove(&board5, false, &row,
                          &column); // false means O is current player.
  printf("next move for board5: (%d, %d)\n\r", row, column);
}
