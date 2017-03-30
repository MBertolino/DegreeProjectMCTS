#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include <math.h>

int N_plays;
const double c = 1.4;


// Randomly perturb the board (Ska kanske läggas nån annanstans)
void perturb_board(int N_rows, int* rows, int* total_sticks, double perturb) {
  if ((double)rand()/RAND_MAX < perturb) {
    int row = (double)N_rows*rand()/RAND_MAX;
    rows[row]++;
    (*total_sticks)++;    
  }
  
  return;
}


int random_move(int* rows, int N_rows, int total_sticks, int row, int sticks, double perturb) {

  int win = 1;
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
  rows_temp[row] -= sticks;
  total_sticks -= sticks;
  
  // Simulate the game
  while (1) {
    win = 1 - win;
    
    // See if a winning move is possible
    for (int i = 0; i < N_rows; i++) {
      if (rows_temp[i] == 0)
        continue;
      if (rows_temp[i] == total_sticks) {
        free(rows_temp);
        return win;
      }
      break;
    }
    
    // Make random move
    do {
      row = (double)N_rows*rand()/RAND_MAX;
    } while (rows_temp[row] == 0);
    sticks = 1 + (double)rows_temp[row]*rand()/RAND_MAX;
  
    rows_temp[row] -= sticks;
    total_sticks -= sticks;
    if (total_sticks <= 0) {
      break;
    }
    
    // Randomly perturb board
    perturb_board(N_rows, rows_temp, &total_sticks, perturb);
  }
  
  free(rows_temp);
  return win;
}


int monte_carlo(tree_t* tree, int* rows, int N_rows, double perturb, int row_p) {
  
  int total_sticks = tree->cell[0]->total_sticks;
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
    for (int m = 0; m < N_rows; m++)
      rows_temp[m] = rows[m];
  
  if (row_p > 0) {
    // Count for perturbation
    total_sticks++;
    rows[row_p]++;
    
    // Allocate new (perturbed) cell
    if (tree->cell[row_p] == NULL) {
      tree->cell[row_p] = (cell_t*)malloc(sizeof(cell_t));
      tree->cell[row_p]->children = NULL;
      tree->cell[row_p]->row = tree->cell[0]->row;
      tree->cell[row_p]->sticks = tree->cell[0]->sticks;
      tree->cell[row_p]->total_sticks = total_sticks;
    }
  }
  
  // See if winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows_temp[i] == 0)
      continue;
    if (rows_temp[i] == total_sticks) {
      tree->wins += 1;
      tree->plays += 1;
      return 1;
    }
    break;
  }
    printf("total_sticks = %i\n", total_sticks);
  
  // If this is a leaf node
  if (tree->cell[row_p]->children == NULL) {
    // Allocate memory for the child nodes
    tree->cell[row_p]->children = (tree_t**)malloc(total_sticks*sizeof(tree_t*));
    int index = 0;
    for (int i = 0; i < N_rows; i++) {
      for (int j = 1; j <= rows[i]; j++) {
        tree->cell[row_p]->children[index] = (tree_t*)malloc(sizeof(tree_t));
        tree->cell[row_p]->children[index]->wins = 0;
        tree->cell[row_p]->children[index]->plays = 0;
        tree->cell[row_p]->children[index]->cell = (cell_t**)malloc((N_rows+1)*sizeof(cell_t*));
        tree->cell[row_p]->children[index]->cell[0] = (cell_t*)malloc(sizeof(cell_t));
        tree->cell[row_p]->children[index]->cell[0]->children = NULL;
        tree->cell[row_p]->children[index]->cell[0]->row = i;
        tree->cell[row_p]->children[index]->cell[0]->sticks = j;
        tree->cell[row_p]->children[index]->cell[0]->total_sticks = total_sticks - j;
        index++;
      }
    }
    printf("leaf\n");
    
    // Simulate random moves
    index = (double)total_sticks*rand()/RAND_MAX;
    int win = random_move(rows_temp, N_rows, total_sticks, tree->cell[row_p]->children[index]->cell[0]->row, tree->cell[row_p]->children[index]->cell[0]->sticks, perturb);
    tree->wins += win;
    tree->plays++;
    
    return win;
  }
  
  // If this is an internal node
  else {
    printf("internal\n");

    // Find the child with highest ucb
    tree_t* max_child = tree->cell[row_p]->children[0];
    printf("hej row_p = %i\n", row_p);
    printf("hej row = %i\n", tree->cell[row_p]->row);
    double ucb_max = (double)max_child->wins/max_child->plays + c*sqrt(log(N_plays)/max_child->plays);
    double ucb;
    printf("test\n");
    
    
    for (int i = 1; i < total_sticks; i++) {
      // If no plays has been made
      if (tree->cell[row_p]->children[i]->plays == 0) {
        max_child = tree->cell[row_p]->children[i];
        break;
      }
      
      // Compute ucb
      ucb = (double)tree->cell[row_p]->children[i]->wins/tree->cell[row_p]->children[i]->plays + c*sqrt(log(N_plays)/tree->cell[row_p]->children[i]->plays);
      if (ucb > ucb_max) {
        ucb = ucb_max;
        max_child = tree->cell[row_p]->children[i];
      }
    }
    
    // Update the rows
    rows_temp[max_child->cell[0]->row] -= max_child->cell[0]->sticks;
    
    int row_p_temp = 0;
    if ((double)rand()/RAND_MAX < perturb)
      row_p_temp = (double)N_rows*rand()/RAND_MAX;
    
    // Traverse down the tree
    int win = 1 - monte_carlo(max_child, rows_temp, N_rows, perturb, row_p_temp);
    tree->wins += win;
    tree->plays++;
    free(rows_temp);
    return win;
  }
}


// Free a tree
void free_tree(tree_t* tree) {
  if (tree != NULL) {
    if (tree->cell[0]->children != NULL) {
      for (int i = 0; i < tree->cell[0]->total_sticks; i++) {
        free_tree(tree->cell[0]->children[i]);
      }
      free(tree->cell[0]->children);
    }
    free(tree);
  }
  return;
}


// The extended s-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb) {
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      res->row = i;
      res->sticks = rows[i];
      return;
    }
    break;
  }
  
  // Initialize
  tree_t* root = (tree_t*)malloc(sizeof(tree_t));
  root->wins = 0;
  root->plays = 0;
  root->cell = (cell_t**)malloc((N_rows+1)*sizeof(cell_t*));
  root->cell[0] = (cell_t*)malloc(sizeof(cell_t));
  root->cell[0]->children = NULL;
  root->cell[0]->row = -1;
  root->cell[0]->sticks = -1;
  root->cell[0]->total_sticks = total_sticks;
  
  N_plays = 0;
  int N_sims = 5000;
  for (int k = 0; k < N_sims; k++) {
    monte_carlo(root, rows, N_rows, perturb, 0);
    N_plays++;
  }
  
  // Decide which move to make
  // (Choose the child so that the opponent has the lowest chance of winning)
  tree_t* min_child = root->cell[0]->children[0];
  for (int i = 1; i < total_sticks; i++) {
    if (min_child->plays > root->cell[0]->children[i]->plays)
      min_child = root->cell[0]->children[i];
  }
  res->row = min_child->cell[0]->row;
  res->sticks = min_child->cell[0]->sticks;
  
  // Free and return
  free_tree(root);
  return;
}


// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb) {
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      res->row = i;
      res->sticks = rows[i];
      return;
    }
    break;
  }
  
  int** stats = (int**)malloc(N_rows*sizeof(int*));
  for (int i = 0; i < N_rows; i++)
    stats[i] = (int*)calloc(rows[i], sizeof(int));
  
  // Simulate moves using random simulations
  for (int k = 0; k < 1000; k++)
    for (int i = 0; i < N_rows; i++)
      for (int j = 0; j < rows[i]; j++)
        stats[i][j] += random_move(rows, N_rows, total_sticks, i, j+1, perturb);

  // Find the "best" move
  int row = 0;
  int sticks = 1;
  int stats_max = 0;
  for (int i = 0; i < N_rows; i++) {
    for (int j = 0; j < rows[i]; j++) {
      if (stats[i][j] >= stats_max) {
        stats_max = stats[i][j];
        row = i;
        sticks = j+1;
      }
    }
  }
  res->row = row;
  res->sticks = sticks;
  
  for (int i = 0; i < N_rows; i++)
    free(stats[i]);
  free(stats);
  
  return;
}


// The p-player
void p_player(move_t* res, int* rows, int N_rows, double p, int total_sticks) {
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      res->row = i;
      res->sticks = rows[i];
      return;
    }
    break;
  }
  
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
    
  // Optimal move
  } else {
    for (int i = 0; i < N_rows; i++) {
      if ((X^rows[i]) < rows[i]) {
        row = i;
        sticks = rows[i] - (X^rows[i]);
        break;
      }
    }
  }
  res->row = row;
  res->sticks = sticks;
  
  return;
}


// The human player
void h_player(move_t* res, int* rows, int N_rows) {
  int row, sticks;
  
  while (1) {
    int ret = scanf("%i%i", &row, &sticks); // cannot read strings
    row--;
    if (ret <= 0) {
      printf("Illegal input: Enter numbers plz. Try again:\n");
      continue;
    }
    if (row > N_rows-1 || row < 0) {
      printf("Illegal move: Rows out of bounds. Try again:\n");
      continue;
    } else if (sticks < 1 || sticks > rows[row]) {
      printf("Illegal move: Wrong number of sticks. Try again:\n");
      continue;
    }
    break;
  } 
  res->row = row;
  res->sticks = sticks;
  
  return;
}


// The random player
void r_player(move_t* res, int* rows, int N_rows, int total_sticks) {
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      res->row = i;
      res->sticks = rows[i];
      return;
    }
    break;
  }
    
  // Make a random move
  int row, sticks;
  do {
    row = (double)N_rows*rand()/RAND_MAX;
  } while (rows[row] == 0);
  sticks = 1 + (double)rows[row]*rand()/RAND_MAX;
  res->row = row;
  res->sticks = sticks;
  
  return;
}

