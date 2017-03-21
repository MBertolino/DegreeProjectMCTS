#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "players.h"

#define C_NORMAL "\x1B[0m"
#define C_RED    "\x1B[31m"
#define C_GREEN  "\x1B[32m"

#define opponent s // p or s

void print_board(int N_rows, int *rows) {
  for (int i = 0; i < N_rows; i++) {
    printf("%i (%i):\t", i+1, rows[i]);
    for (int j = 0; j < rows[i]; j++)
      printf("|");
    printf("\n");
  }
}


int main(int argc, char* argv[]) {
  
  if (argc != 2) {
    printf("Expected number of rows as input argument.\n");
    return -1;
  }
  
  int N_rows = atoi(argv[1]);
  if (N_rows < 1) {
    printf("Numbe of rows must be > 0.\n");
    return -1;
  }
  
  int *rows = (int*)malloc(N_rows*sizeof(int));
  int total_sticks = 0;
  for (int i = 0; i < N_rows; i++) {
    rows[i] = 3 + i*2;
    total_sticks += rows[i];
  }
  
  //double p = 0.5;
  int player = 2;
  int row, sticks;
  move_t* res = (move_t*)malloc(sizeof(move_t));
  
  srand(time(NULL));
  
  while (1) {
    player = 3 - player;
    
    printf("\n");
    print_board(N_rows, rows);
    if (player == 1)
      printf("%sPlayer %i%s:s turn: \n", C_GREEN, player, C_NORMAL);
    else
      printf("%sPlayer %i%s:s turn: \n", C_RED, player, C_NORMAL);
    
    // Human player
    if (player == 1) {
      while (1) {
        int ret = scanf("%i%i", &row, &sticks); // cannot read strings
        row--;
        if (ret <= 0) {
          printf("Illegal input: Enter numbers plz. Try again.\n");
          print_board(N_rows, rows);
          continue;
        }
        if (row > N_rows-1 || row < 0) {
          printf("Illegal move: Rows out of bounds. Try again.\n");
          print_board(N_rows, rows);
          continue;
        } else if (sticks < 1 || sticks > rows[row]) {
          printf("Illegal move: Wrong number of sticks. Try again.\n");
          print_board(N_rows, rows);
          continue;
        }
        break;
      }
    
    // Computer
    } else {
      //p_player(res, rows, N_rows, p);
      s_player(res, rows, N_rows, total_sticks);
      row = res->row;
      sticks = res->sticks;
    }
    
    rows[row] -= sticks;
    total_sticks -= sticks;
    if (total_sticks <= 0) {
      printf("\nPlayer %i wins!\n", player);
      break;
    }
    
  }
  
  free(res);
  return 0;
}




