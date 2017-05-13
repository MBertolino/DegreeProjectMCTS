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

// Choose which game to play: 0 = normal, (0 1] = perturbed
double perturb = 0.1;

// Define players: Human = 0, p = 1, q = 2, s = 3, x = 4, r = 5
#define PLAYER1 1
#define PLAYER2 3 // <-- change this value

// p-, q- and c-values
double c_min = .01;
double c_max = 10000;
double p2 = 1;
double q2 = 1;
double c2 = 1;


int main(int argc, char* argv[]) {
  
  // Number of input arguments
  if (argc != 3) {
    printf("Usage: ./nim_stats N_rows total_sticks\n");
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
  int N_vals1 = 40;
  int N_vals2 = 0;
  int N_games = 1;
  
  // Allocate memory
  int** wins = (int**)malloc((N_vals1+1)*sizeof(int*));
  for (int i = 0; i <= N_vals1; i++)
    wins[i] = (int*)malloc((N_vals2+1)*sizeof(int));
  int* rows = (int*)malloc(N_rows*sizeof(int));
  
  // Begin simulations
  for (int i = 0; i <= N_vals1; i++) {
    for (int j = 0; j <= N_vals2; j++) {
            
      // p-value and q-value
      double var1 = (double)i/N_vals1;
      if (N_vals2 > 0)
        p2 = (double)j/N_vals2;
      
      // Play N_games
      for (int k = 0; k < N_games; k++) {
        
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
              case 2: q_player(res, rows, N_rows, total_sticks, perturb, var1); break;
              case 3: s_player(res, rows, N_rows, total_sticks, perturb); break;
              case 4: x_player(res, rows, N_rows, total_sticks, perturb, c_min*pow(10, var1*log10(c_max/c_min))); break;
              case 5: r_player(res, rows, N_rows, total_sticks); break;
            }
            
          // Player 2
          } else {
            switch (PLAYER2) {
              case 0: h_player(res, rows, N_rows); break;
              case 1: p_player(res, rows, N_rows, total_sticks, p2); break;
              case 2: q_player(res, rows, N_rows, total_sticks, perturb, p2); break;
              case 3: s_player(res, rows, N_rows, total_sticks, perturb); break;
              case 4: x_player(res, rows, N_rows, total_sticks, perturb, c2); break;
              case 5: r_player(res, rows, N_rows, total_sticks); break;
            }
          }
          
          // Randomly perturb the board
          if ((double)rand()/((double)RAND_MAX + 1) < perturb) {
            rows[0]++;
            total_sticks++;
          }
          
          // Update board
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
    
    // Update progress bar
    printf("\r|");
    int N_symbols = (int)((double)(i+1)/(N_vals1+1)*prog_max);
    for (int ip = 0; ip < N_symbols; ip++) {
      printf("#");
    }
    printf("%*s| %i%% ", prog_max - N_symbols, "", (int)(100.*(i+1)/(N_vals1+1) + 0.5));
    fflush(stdout);
  }
  printf("\n");
  
  // Write the statistics into a file
  char str[80] = "";
  strcat(str, "../statistics/stats_");
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
  strcat(str, ".csv");
  
  FILE* f = fopen(str, "wb");
  fprintf(f, "%lf,%d,%d,%d,%d,%d,%lf,%lf\n", perturb, N_vals1, N_vals2, N_games, N_rows, total_sticks_init, c_min, c_max);
  for (int i = 0; i <= N_vals1; i++) {
    fprintf(f, "%lf", (double)wins[i][0]/N_games);
    for (int j = 1; j <= N_vals2; j++)
      fprintf(f, ",%lf", (double)wins[i][j]/N_games);
    if (i != N_vals1)
      fprintf(f, "\n");
  }
  fclose(f);
  printf("Statistics done.\n");
  
  // Free
  free(rows);
  free(res);
  for (int i = 0; i <= N_vals1; i++)
    free(wins[i]);
  free(wins);
  return 0;
}

