#include <stdlib.h>
#include <stdio.h>


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
  
  int player = 2;
  int row, pick;
  
  while (1) {
    player = 3 - player;
    
    printf("\n");
    print_board(N_rows, rows);
    printf("Player %i:s turn: \n", player);
    
    while (1) {
      int ret = scanf("%i%i", &row, &pick); // cannot read strings
      
      if (ret <= 0) {
        printf("Illegal input: Enter numbers plz. Try again.\n");
        print_board(N_rows, rows);
        continue;
      }
      if (row > N_rows || row < 1) {
        printf("Illegal move: Rows out of bounds. Try again.\n");
        print_board(N_rows, rows);
        continue;
      } else if (pick < 1 || pick > rows[row-1]) {
        printf("Illegal move: Wrong number of sticks. Try again.\n");
        print_board(N_rows, rows);
        continue;
      }
      break;
    }
    rows[row-1] -= pick;
    
    total_sticks -= pick;
    if (total_sticks <= 0) {
      printf("\nPlayer %i wins!\n", player);
      break;
    }
    
  }
  
  return 0;
}




