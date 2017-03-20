#include <stdlib.h>
#include <stdio.h>
#include "structs.h"


int monte_carlo(int* rows, int N_rows, int total_sticks) {
  printf("test\n");
  int win = 1;
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  //*rows_temp = *rows;
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
  
  // Print rows
  printf("--COPY------------\n");
  for (int i = 0; i < N_rows; i++) {
    printf("%i (%i):\t", i+1, rows_temp[i]);
    for (int j = 0; j < rows_temp[i]; j++)
      printf("|");
    printf("\n");
  }
  printf("--COPY------------\n");

  // Simulate the game
  int row, sticks;
  while (1) {
    win = 1 - win;
    do {
      row = (double)N_rows*rand()/RAND_MAX;
    } while (rows_temp[row] == 0);
    sticks = 1 + (double)rows_temp[row]*rand()/RAND_MAX;
  
    rows_temp[row] -= sticks;
    total_sticks -= sticks;
    if (total_sticks <= 0) {
      break;
    }
  }
  
  free(rows_temp);
  return win;
}

// The p-player
void p_player(move_t* res, int* rows, int N_rows, double p) {
  int X = 0;
  int row = 1;
  int sticks = 0;
  
  // Compute the nim-sum
  for (int i = 0; i < N_rows; i++)
    X ^= rows[i];
  
  // Random move
  if (X == 0 || p < (double)rand()/RAND_MAX) {
    do {
      row = (double)N_rows*rand()/RAND_MAX;
    } while (rows[row] == 0);
    sticks = 1 + (double)rows[row]*rand()/RAND_MAX;
    printf("Random move\n");
    
    // Optimal move
  } else {
    for (int i = 0; i < N_rows; i++) {
      if ((X^rows[i]) < rows[i]) {
        row = i;
        sticks = rows[i] - (X^rows[i]);
        break;
      }
    }
    printf("Optimal move\n");
  }
  
  printf("row = %i\n", row+1);
  printf("sticks = %i\n", sticks);
  
  res->row = row;
  res->sticks = sticks;
}


// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks) {
  
  int** stats = (int**)malloc(N_rows*sizeof(int*));
  for (int i = 0; i < N_rows; i++)
    stats[i] = (int*)malloc(rows[i]*sizeof(int));
  
  // Simulate moves using Monte Carlo
  int i;
  for (i = 0; i < N_rows; i++) {
    for (int j = 1; j <= rows[i]; j++) {
      stats[i][j] += monte_carlo(rows, N_rows, total_sticks);
    }
  }
  
  // Find the "best" move
  int row = i;
  int sticks = rows[i];
  int stats_max = 0;
  for (int i = 0; i < N_rows; i++) {
    for (int j = 1; j <= rows[i]; j++) {
      if (stats[i][j] > stats_max) {
        stats_max = stats[i][j];
        row = i;
        sticks = j;
      }
    }
  }
  
  res->row = row;
  res->sticks = sticks;
  
  for (int i = 0; i < N_rows; i++)
    free(stats[i]);
  free(stats);
}










