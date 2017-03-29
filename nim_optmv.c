#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "players.h"

// Define colors
#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

// Choose which game to play: 0 = normal, (0 1] = perturbed
#define PERTURB 1

// Define players: Human = 0, p = 1, s = 2, x = 3, r = 4
#define PLAYER 3

// p-value of the p-players
#if PLAYER == 1
  double p = 0.5;
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
  move_t* res_opt = (move_t*)malloc(sizeof(move_t));
  
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
  
  // Initalize stats variables
  int N_games = 100000;
  int length = 0;
  for (int i = 0; i < N_rows; i++)
    length += 3 + i*2;
  int* optmv = (int*)calloc(length, sizeof(int));
  int* plays = (int*)calloc(length, sizeof(int));
  int* rows = (int*)malloc(N_rows*sizeof(int));
  
  // Begin simulations
  for (int k = 0; k < N_games; k++) {
    
    // Create the board
    int total_sticks = 0;
    for (int i = 0; i < N_rows; i++) {
      rows[i] = 3 + i*2;
      total_sticks += rows[i];
    }
    
    int player = 1 + 2.*rand()/RAND_MAX;
    while (1) {
      player = 3 - player;
      
      #if PLAYER == 0
        h_player(res, rows, N_rows);
      #elif PLAYER == 1
        p_player(res, rows, N_rows, p, total_sticks);
      #elif PLAYER == 2
        s_player(res, rows, N_rows, total_sticks, PERTURB);
      #elif PLAYER == 3
        x_player(res, rows, N_rows, total_sticks, PERTURB);
      #elif PLAYER == 4
        r_player(res, rows, N_rows, total_sticks);
      #endif
      
      // Optimal move?
      p_player(res_opt, rows, N_rows, 1, total_sticks);
      int X = 0;
      for (int i = 0; i < N_rows; i++)
        X ^= rows[i];
      if (X != 0) {
        if (res->row == res_opt->row && res->sticks == res_opt->sticks)
          optmv[total_sticks - 1]++;
        plays[total_sticks - 1]++;
      }
      
      // Update board
      row = res->row;
      sticks = res->sticks;
      rows[row] -= sticks;
      total_sticks -= sticks;
      if (total_sticks <= 0)
        break;
    }
    
    // Randomly perturb board
    #if PERTURB != 0
      if ((double)rand()/RAND_MAX < PERTURB) {
        perturb_board(N_rows, rows, &total_sticks);
      }
    #endif
    
    // Update progress bar
    printf("\r|");
    int N_symbols = (int)((double)(k+1)/N_games*prog_max);
    for (int ip = 0; ip < N_symbols; ip++) {
      printf("#");
    }
    printf("%*s| %i%% ", prog_max - N_symbols, "", (int)(100.*(k+1)/N_games + 0.5));
    fflush(stdout);
  }
  printf("\n");
  
  // Write the statistics into a file
  #if PLAYER == 0
    FILE* f = fopen("../statistics/optmv_hplayer.csv", "wb");
  #elif PLAYER == 1
    FILE* f = fopen("../statistics/optmv_pplayer.csv", "wb");
  #elif PLAYER == 2
    FILE* f = fopen("../statistics/optmv_splayer.csv", "wb");
  #elif PLAYER == 3
    FILE* f = fopen("../statistics/optmv_xplayer.csv", "wb");
  #elif PLAYER == 4
    FILE* f = fopen("../statistics/optmv_rplayer.csv", "wb");
  #endif
  fprintf(f, "%lf", (double)optmv[0]/plays[0]);
  for (int i = 0; i < length; i++) {
    fprintf(f, "\n%lf", (double)optmv[i]/plays[i]);
  }
  fclose(f);
  printf("Statistics done.\n");
  
  // Free
  free(res);
  free(res_opt);
  free(rows);
  free(optmv);
  free(plays);
  return 0;
}




