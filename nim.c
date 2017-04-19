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
double perturb = 0.5;

// Define players: Human = 0, p = 1, q = 2, s = 3, x = 4, r = 5
#define PLAYER1 0
#define PLAYER2 2

// p-value of the p-players
#if PLAYER1 == 1
  double p1 = 0.5;
#endif
#if PLAYER2 == 1
  double p2 = 0.5;
#endif

// q-value of the q-players
#if PLAYER1 == 2
  double q1 = 0.5;
#endif
#if PLAYER2 == 2
  double q2 = 0.5;
#endif


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
    rows[i] = 1;
    total_sticks += rows[i];
  }
  
  // Return variables from the players
  int player = 2;
  int row, sticks;
  move_t* res = (move_t*)malloc(sizeof(move_t));
  
  // Set seed
  srand(time(NULL));
  
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
      #if PLAYER1 == 0
        h_player(res, rows, N_rows);
      #elif PLAYER1 == 1
        p_player(res, rows, N_rows, p1, total_sticks);
      #elif PLAYER1 == 2
        q_player(res, rows, N_rows, q1, total_sticks, perturb);
      #elif PLAYER1 == 3
        s_player(res, rows, N_rows, total_sticks, perturb);
      #elif PLAYER1 == 4
        x_player(res, rows, N_rows, total_sticks, perturb);
      #elif PLAYER1 == 5
        r_player(res, rows, N_rows, total_sticks);
      #endif
    
    // Player 2
    } else {
      #if PLAYER2 == 0
        h_player(res, rows, N_rows);
      #elif PLAYER2 == 1
        p_player(res, rows, N_rows, p2, total_sticks);
      #elif PLAYER2 == 2
        q_player(res, rows, N_rows, q2, total_sticks, perturb);
      #elif PLAYER2 == 3
        s_player(res, rows, N_rows, total_sticks, perturb);
      #elif PLAYER2 == 4
        x_player(res, rows, N_rows, total_sticks, perturb);
      #elif PLAYER2 == 5
        r_player(res, rows, N_rows, total_sticks);
      #endif
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
    
    // Randomly perturb board
    perturb_board(N_rows, rows, &total_sticks, perturb);
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

