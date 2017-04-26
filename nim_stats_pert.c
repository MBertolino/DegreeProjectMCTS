#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "players.h"

// Define colors
#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

// Define players: Human = 0, p = 1, q = 2, s = 3, x = 4, r = 5
#define PLAYER1 3
#define PLAYER2 4 // <-- change this value

// p-value of the p-players
#if PLAYER1 == 1
  double p1 = 1;
#endif
#if PLAYER2 == 1
  double p2 = 1;
#endif

// q-value of the q-players
#if PLAYER1 == 2
  double q1 = 1;
#endif
#if PLAYER2 == 2
  double q2 = 1;
#endif

// c parameter for the x-player
#if PLAYER1 == 4
  double c1 = 1;
#endif
#if PLAYER2 == 4
  double c2 = 1;
#endif


int main(int argc, char* argv[]) {
  
  // Number of input arguments
  if (argc != 2) {
    printf("Expected number of rows as input argument.\n");
    return -1;
  }
  
  // Number of rows
  int N_rows = atoi(argv[1]);
  if (N_rows < 1) {
    printf("Number of rows must be > 0.\n");
    return -1;
  }
  
  // Return variables from the players
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
  printf("%*s| %i%% ", prog_max, "", 0);
  fflush(stdout);
  
  // Simulation parameters
  int N_perturb = 20;
  int N_games = 5000;
  double perturb_max = 0.7;
  
  // Allocate memory
  int *wins = (int*)malloc(N_perturb*sizeof(int));
  int *rows_init = (int*)malloc(N_rows*sizeof(int));
  int *rows = (int*)malloc(N_rows*sizeof(int));
  
  // Create the initial board
  for (int i = 0; i < N_rows; i++)
    rows_init[i] = 3 + 2*i;
    //rows_init[i] = 1.5 + 0.5*i; // good for q-player
  
  // Begin simulations
  for (int k = 0; k < N_games; k++) {
    for (int i = 0; i < N_perturb; i++) {
      
      // Create the board
      int total_sticks = 0;
      for (int i = 0; i < N_rows; i++) {
        rows[i] = rows_init[i];
        total_sticks += rows[i];
      }

      int player = 1 + 2.*rand()/RAND_MAX;
      while (1) {
        player = 3 - player;

          // Player 1
          if (player == 1) {
            #if PLAYER1 == 0
              h_player(res, rows, N_rows);
            #elif PLAYER1 == 1
              p_player(res, rows, N_rows, total_sticks, p1);
            #elif PLAYER1 == 2
              q_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, q1);
            #elif PLAYER1 == 3
              s_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb);
            #elif PLAYER1 == 4
              x_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, c1);
            #elif PLAYER2 == 5
              r_player(res, rows, N_rows, total_sticks);
            #endif
            
          // Player 2
          } else {
            #if PLAYER2 == 0
              h_player(res, rows, N_rows);
            #elif PLAYER2 == 1
              p_player(res, rows, N_rows, total_sticks, p2);
            #elif PLAYER2 == 2
              q_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, q2);
            #elif PLAYER2 == 3
              s_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb);
            #elif PLAYER2 == 4
              x_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, c2);
            #elif PLAYER2 == 5
              r_player(res, rows, N_rows, total_sticks);
            #endif
          }

        // Randomly perturb board
        perturb_board(N_rows, rows, &total_sticks, perturb_max*i/N_perturb);

        // Update board
        row = res->row;
        sticks = res->sticks;
        rows[row] -= sticks;
        total_sticks -= sticks;
        if (total_sticks <= 0) {
          wins[i] += player - 1;
          break;
        }
      }
    }
    
    // Update progress bar
    printf("\r|");
    int N_symbols = (int)((double)(k+1)/(N_games)*prog_max);
    for (int ip = 0; ip < N_symbols; ip++) {
      printf("#");
    }
    printf("%*s| %i%% ", prog_max - N_symbols, "", (int)(100.*(k+1)/(N_games) + 0.5));
    fflush(stdout);
  }
  printf("\n");
  
  // Write the statistics into a file
  char str[10] = "";
  int p_to_print = 0;
  for (int i = 0; i < 2; i++) {
    if (i == 0) {
      p_to_print = PLAYER1;
    } else {
      p_to_print = PLAYER2;
    }
    switch (p_to_print) {
      case 0:
        strcat(str, "h");
        break;
      case 1:
        strcat(str, "p");
        break;
      case 2:
        strcat(str, "q");
        break;
      case 3:
        strcat(str, "s");
        break;
      case 4:
        strcat(str, "x");
        break;
      case 5:
        strcat(str, "r");
        break;
    }
  }
  char str2[80];
  strcat(str2, "../statistics/stats_");
  strcat(str2, str);
  strcat(str2, "_pert.csv");
  FILE* f = fopen(str2, "wb");
  fprintf(f, "%d,%lf,%d,%d", N_perturb, perturb_max, N_games, N_rows);
  for (int i = 0; i < N_rows; i++)
    fprintf(f, ",%d", rows_init[i]);
  fprintf(f, "\n");
  for (int i = 0; i < N_perturb; i++) {
    fprintf(f, "%lf", (double)wins[i]/N_games);
    if (i != N_perturb-1)
      fprintf(f, "\n");
  }
  fclose(f);
  printf("Statistics done.\n");
  
  // Free
  free(rows);
  free(rows_init);
  free(res);
  free(wins);
  return 0;
}




