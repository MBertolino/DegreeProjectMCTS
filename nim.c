#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void print_board(int N_rows, int *rows) {
	for (int i = 0; i < N_rows; i++) {
    printf("%i (%i):\t", i+1, rows[i]);
    for (int j = 0; j < rows[i]; j++)
      printf("|");
    printf("\n");
  }
}


int main(int argc, char *argv[]) {
  
  if (argc != 3) {
    printf("Expects 2 arguments, # rows and sticks\n");
    return -1;
  }
  int N_rows = atoi(argv[1]);
  int sticks = atoi(argv[2]);
  int total_sticks = N_rows*sticks;
  
  if (N_rows < 1 && sticks < 1) {
    printf("Rows and sticks must be > 0.\n");
    return -1;
  }
  
  int *rows = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows[i] = sticks;
  }
  
  double p = 0.4;
  int player = 2;
  int row, pick;
  int X;
  
  srand(time(NULL));
  
  while (1) {
    player = 3 - player;
    
    printf("\n");
    print_board(N_rows, rows);
    printf("Player %i:s turn: \n", player);
    
    if (player == 1) {
      while (1) {
        int ret = scanf("%i%i", &row, &pick); // cannot read strings
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
        } else if (pick < 1 || pick > rows[row]) {
          printf("Illegal move: Wrong number of sticks. Try again.\n");
          print_board(N_rows, rows);
          continue;
        }
        break;
      }
    } else {
      X = 0;
      row = 1;
      for (int i = 0; i < N_rows; i++)
        X ^= rows[i];
      if (X == 0 || p < (double)rand()/RAND_MAX) {
        do {
          row = (double)N_rows*rand()/RAND_MAX;
          printf("row = %i\n", row);
        } while (rows[row] == 0);
      pick = 1 + (double)rows[row]*rand()/RAND_MAX;
      printf("pick = %i\n", pick);
      } else {
        for (int i = 0; i < N_rows; i++) {
          if ((X^rows[i]) < rows[i]) {
            row = i+1;
            pick = rows[i] - (X^rows[i]);
            break;
          }
        }
      }
    }
    
    
    rows[row] -= pick;
    
    total_sticks -= pick;
    if (total_sticks <= 0) {
      printf("\nPlayer %i wins!\n", player);
      break;
    }
    
  }
  
  return 0;
}




