#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "players.h"

// Define colors
#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

// Define players: Human = 0, p = 1, q = 2, s = 3, x = 4, r = 5
#define PLAYER1 1
#define PLAYER2 2 // <-- change this value

// p-value, q-value and exploration parameter c for the x-player
double var1 = 1;
double var2 = 1;


int main(int argc, char* argv[]) {
  
  // Number of input arguments
  if (argc != 3) {
    printf("Usage: ./nim_pert N_rows total_sticks\n");
    return -1;
  }
  
  // Number of rows
  int N_rows = atoi(argv[1]);
  if (N_rows < 1) {
    printf("Number of rows must be > 0.\n");
    return -1;
  }
  
  // Total number of sticks
  int total_sticks_init = atoi(argv[2]);
  if (total_sticks_init < N_rows) {
    printf("Total sticks cannot be smaller than N_rows.\n");
    return -1;
  }
  
  // Return-variables from the players
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
  int N_games = 100;
  double perturb_max = 0.7;
  
  // Allocate memory
  int *wins = (int*)malloc(N_perturb*sizeof(int));
  int *rows = (int*)malloc(N_rows*sizeof(int));
    
  // Begin simulations
  for (int k = 0; k < N_games; k++) {
    for (int i = 0; i < N_perturb; i++) {
      
      // Initialize a random board
      int total_sticks = total_sticks_init;
      for (int m = 0; m < N_rows; m++)
        rows[m] = 1;
      for (int m = 0; m < total_sticks - N_rows; m++)
        rows[(int)((double)N_rows*rand()/((double)RAND_MAX + 1))] += 1;
      
      // Randomize who starts
      int player = 1 + 2.*rand()/((double)RAND_MAX + 1);
      
      // Run the game
      while (1) {
        player = 3 - player;
        
        // Player 1
        if (player == 1) {
          switch (PLAYER1) {
            case 0: h_player(res, rows, N_rows); break;
            case 1: p_player(res, rows, N_rows, total_sticks, var1); break;
            case 2: q_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, var1); break;
            case 3: s_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb); break;
            case 4: x_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, var1); break;
            case 5: r_player(res, rows, N_rows, total_sticks); break;
          }
        
        // Player 2
        } else {
          switch (PLAYER2) {
            case 0: h_player(res, rows, N_rows); break;
            case 1: p_player(res, rows, N_rows, total_sticks, var2); break;
            case 2: q_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, var2); break;
            case 3: s_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb); break;
            case 4: x_player(res, rows, N_rows, total_sticks, perturb_max*i/N_perturb, var2); break;
            case 5: r_player(res, rows, N_rows, total_sticks); break;
          }
        }
        
        // Randomly perturb the board
        if ((double)rand()/((double)RAND_MAX + 1) < perturb_max*i/N_perturb) {
          rows[0]++;
          total_sticks++;
        }
        
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
  char str[80] = "";
  strcat(str, "../statistics/stats_pert_");
  switch (PLAYER1) {
    case 0: strcat(str, "h"); break;
    case 1: strcat(str, "p"); break;
    case 2: strcat(str, "q"); break;
    case 3: strcat(str, "s"); break;
    case 4: strcat(str, "x"); break;
    case 5: strcat(str, "r"); break;
  }
  switch (PLAYER2) {
    case 0: strcat(str, "h"); break;
    case 1: strcat(str, "p"); break;
    case 2: strcat(str, "q"); break;
    case 3: strcat(str, "s"); break;
    case 4: strcat(str, "x"); break;
    case 5: strcat(str, "r"); break;
  }
  strcat(str, "_with.csv");
  
  FILE* f = fopen(str, "wb");
  fprintf(f, "%d,%lf,%d,%d,%d\n", N_perturb, perturb_max, N_games, N_rows, total_sticks_init);
  for (int i = 0; i < N_perturb; i++) {
    fprintf(f, "%lf", (double)wins[i]/N_games);
    if (i != N_perturb-1)
      fprintf(f, "\n");
  }
  fclose(f);
  printf("Statistics done.\n");
  
  // Free
  free(rows);
  free(res);
  free(wins);
  return 0;
}

