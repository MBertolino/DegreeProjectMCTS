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
#define PLAYER2 2


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
  
  int row, sticks;
  move_t* res = (move_t*)malloc(sizeof(move_t));
  
  // Set seed
  srand(time(NULL));
  
  // Progress bar
  int prog_max = 83;
  printf("Progress:\n");
  for (int i = 0; i < 4; i++)
    printf("|--------------------");
  printf("|\n|");
  
  int N_vals1 = 1;
  int N_vals2 = 100;
  int N_games = 100;
  int** wins = (int**)malloc(N_vals1*sizeof(int*));
  for (int i = 0; i < N_vals1; i++)
    wins[i] = (int*)malloc(N_vals2*sizeof(int));
  
  for (int i = 0; i < N_vals1; i++) {
    for (int j = 0; j < N_vals2; j++) {
      for (int k = 0; k < N_games; k++) {
        int *rows = (int*)malloc(N_rows*sizeof(int));
        int total_sticks = 0;
        for (int i = 0; i < N_rows; i++) {
          rows[i] = 3 + i*2;
          total_sticks += rows[i];
        }
        
        #if PLAYER1 == 1
          double p1 = (double)i/N_vals1;
        #endif
        #if PLAYER2 == 1
          double p2 = (double)j/N_vals2;
        #endif
        
        int player = 1 + 2.*rand()/RAND_MAX;
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
            wins[i][j] += 2 - player;
            break;
          }
        }
      }
    }
    
    // Update progress bar
    printf("\r|");
    int N_symbols = (int)((double)(i+1)/N_vals1*prog_max);
    for (int ip = 0; ip < N_symbols; ip++) {
      printf("#");
    }
    printf("%*s| %i%% ", prog_max - N_symbols, "", (int)(100.*(i+1)/N_vals1 + 0.5));
    fflush(stdout);
  }
  printf("\n");
  
  // Write the statistics into a file
  FILE* f = fopen("../statistics/stats_splayer.csv", "wb");
  for (int i = 0; i < N_vals1; i++) {
    fprintf(f, "%lf", (double)wins[i][0]/N_games);
    for (int j = 1; j < N_vals2; j++)
      fprintf(f, ",%lf", (double)wins[i][j]/N_games);
    if (i != N_vals1 - 1)
      fprintf(f, "\n");
  }
  fclose(f);
  printf("Statistics done.\n");
  
  free(res);
  return 0;
}




