#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "players.h"

// Define colors
#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

// Choose which game to play: 0 = normal, (0 1] = perturbed
double perturb = 0;

// Define players: Human = 0, p = 1, q = 2, s = 3, x = 4, r = 5
#define PLAYER1 0
#define PLAYER2 5

// p-value of the p-players
double var1 = 0.5;
double var2 = 0.5;

// c-parameter for the x-player
double c1 = 100;
double c2 = 100;


// Header for the function that prints the board
void print_board(int N_rows, int* rows);


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
  
  // Create the board
  int *rows = (int*)malloc(N_rows*sizeof(int));
  int total_sticks = 0;
  for (int i = 0; i < N_rows; i++) {
    rows[i] = 2+i;
    total_sticks += rows[i];
  }
  
  // Return-variables from the players
  int player = 2;
  int row, sticks;
  move_t* res = (move_t*)malloc(sizeof(move_t));
  
  // Set seed
  srand(time(NULL));
  
  // Run the game
  while (1) {
    player = 3 - player;
    
    printf("\n");
    print_board(N_rows, rows);
    if (player == 1)
      printf("%sPlayer %i%s:s turn: \n", C_GREEN, player, C_NORMAL);
    else
      printf("%sPlayer %i%s:s turn: \n", C_RED, player, C_NORMAL);
    
    // Player 1
    if (player == 1) {
      switch (PLAYER1) {
        case 0: h_player(res, rows, N_rows); break;
        case 1: p_player(res, rows, N_rows, total_sticks, var1); break;
        case 2: q_player(res, rows, N_rows, total_sticks, perturb, var1); break;
        case 3: s_player(res, rows, N_rows, total_sticks, perturb); break;
        case 4: x_player(res, rows, N_rows, total_sticks, perturb, c1); break;
        case 5: r_player(res, rows, N_rows, total_sticks); break;
      }
    
    // Player 2
    } else {
      switch (PLAYER2) {
        case 0: h_player(res, rows, N_rows); break;
        case 1: p_player(res, rows, N_rows, total_sticks, var1); break;
        case 2: q_player(res, rows, N_rows, total_sticks, perturb, var1); break;
        case 3: s_player(res, rows, N_rows, total_sticks, perturb); break;
        case 4: x_player(res, rows, N_rows, total_sticks, perturb, c2); break;
        case 5: r_player(res, rows, N_rows, total_sticks); break;
      }
    }
    
    // Randomly perturb board
    if ((double)rand()/(RAND_MAX+1) < perturb) {
      rows[0]++;
      total_sticks++;
    }
    
    // Update board
    row = res->row;
    sticks = res->sticks;
    rows[row] -= sticks;
    total_sticks -= sticks;
    if (total_sticks <= 0) {
      printf("\nPlayer %i wins!\n", player);
      break;
    }
  }
  
  // Free
  free(rows);
  free(res);
  return 0;
}

  
// Print the board
void print_board(int N_rows, int* rows) {
  
  for (int i = 0; i < N_rows; i++) {
    printf("%i (%i):\t", i+1, rows[i]);
    for (int j = 0; j < rows[i]; j++)
      printf("|");
    printf("\n");
  }
  return;
}

