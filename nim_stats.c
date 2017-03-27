#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "players.h"

#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

// Define players: Human = 0, p = 1, s = 2, x = 3
#define PLAYER1 1
#define PLAYER2 1


int main(int argc, char* argv[]) {
  
  if (argc != 2) {
    printf("Expected number of rows as input argument.\n");
    return -1;
  }
  
  int N_rows = atoi(argv[1]);
  if (N_rows < 1) {
    printf("Number of rows must be > 0.\n");
    return -1;
  }
  
  int player = 2;
  int row, sticks;
  move_t* res = (move_t*)malloc(sizeof(move_t));
  
  srand(time(NULL));
  
  int N_games = 100;
  int N_sims = 1;
  int** wins = (int**)malloc(N_games*sizeof(int*));
  for (int i = 0; i < N_games; i++)
    wins[i] = (int*)malloc(N_games*sizeof(int));
  
  for (int i = 0; i < N_games; i++) {
    for (int j = 0; j < N_games; j++) {
      for (int k = 0; k < N_sims; k++) {
        int *rows = (int*)malloc(N_rows*sizeof(int));
        int total_sticks = 0;
        for (int i = 0; i < N_rows; i++) {
          rows[i] = 3 + i*2;
          total_sticks += rows[i];
        }

        #if PLAYER1 == 1
          double p1 = (double)i/N_games;
        #endif
        #if PLAYER2 == 1
          double p2 = (double)j/N_games;
        #endif

        while (1) {
          player = 3 - player;

          // Player 1
          if (player == 1) {
            #if PLAYER1 == 0
              h_player(res, rows, N_rows);
            #elif PLAYER1 == 1
              p_player(res, rows, N_rows, p1);
            #elif PLAYER1 == 2
              s_player(res, rows, N_rows, total_sticks);
            #elif PLAYER1 == 3
              x_player(res, rows, N_rows, total_sticks);
            #endif

          // Player 2
          } else {
            #if PLAYER2 == 0
              h_player(res, rows, N_rows);
            #elif PLAYER2 == 1
              p_player(res, rows, N_rows, p2);
            #elif PLAYER2 == 2
              s_player(res, rows, N_rows, total_sticks);
            #elif PLAYER2 == 3
              x_player(res, rows, N_rows, total_sticks);
            #endif
          }
          row = res->row;
            sticks = res->sticks;
          rows[row] -= sticks;
          total_sticks -= sticks;
          if (total_sticks <= 0) {
            wins[i][j] += player - 1;
            break;
          }
        }
      }
    }
  }
  
  free(res);
  return 0;
}




