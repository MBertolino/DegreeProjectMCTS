#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include <math.h>

int N_plays;
const double c = 1.4;


// Randomly perturb the board (Ska kanske läggas nån annanstans)
void perturb_board(int N_rows, int* rows, int* total_sticks, int perturb) {
  
  if ((double)rand()/RAND_MAX < perturb/100.) {  
    int row = (double)N_rows*rand()/RAND_MAX;
    rows[row]++;
    (*total_sticks)++;    
  }
    
  return;
}

int random_move(int* rows, int N_rows, int total_sticks, int row, int sticks, int perturb) {
  
  int win = 1;
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
  rows_temp[row] -= sticks;
  total_sticks -= sticks;
  
  // Simulate the perturb
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
      if (perturb == 1)
        perturb_board(N_rows, rows_temp, &total_sticks, perturb);
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


int monte_carlo(tree_t* tree, int* rows, int N_rows, int perturb) {
  int total_sticks = tree->total_sticks;
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      tree->wins += 1;
      tree->plays += 1;
      return 1;
    }
    break;
  }
  
  // If this is a leaf node
  if (tree->children == NULL) {
    
    // Allocate memory for the child nodes
    tree->children = (tree_t**)malloc(total_sticks*sizeof(tree_t*));
    int index = 0;
    for (int i = 0; i < N_rows; i++) {
      for (int j = 1; j <= rows[i]; j++) {
        tree->children[index] = (tree_t*)malloc(sizeof(tree_t));
        tree->children[index]->children = NULL;
        tree->children[index]->wins = 0;
        tree->children[index]->plays = 0;
        tree->children[index]->row = i;
        tree->children[index]->sticks = j;
        tree->children[index]->total_sticks = total_sticks - j;
        index++;
      }
    }
    
    // Simulate random moves
    index = (double)total_sticks*rand()/RAND_MAX;
    int win = random_move(rows, N_rows, total_sticks, tree->children[index]->row, tree->children[index]->sticks, perturb);
    tree->wins += win;
    tree->plays++;
    
    return win;
  }
  
  // If this is an internal node
  else {
    
    // Find child not yet played
    for (int i = 0; i < total_sticks; i++) {
      if (tree->children[i]->plays == 0) {
        
        int* rows_temp = (int*)malloc(N_rows*sizeof(int));
        for (int m = 0; m < N_rows; m++)
          rows_temp[m] = rows[m];
        rows_temp[tree->children[i]->row] -= tree->children[i]->sticks;
        
        int win = 1 - monte_carlo(tree->children[i], rows_temp, N_rows, perturb);
        tree->wins += win;
        tree->plays++;
        
        free(rows_temp);
        return win;
      }
    }
    
    // Find the child with highest ucb
    tree_t* max_child = tree->children[0];
    double ucb_max = (double)max_child->wins/max_child->plays + c*sqrt(log(N_plays)/max_child->plays);
    double ucb;
    for (int i = 1; i < total_sticks; i++) {
      ucb = (double)tree->children[i]->wins/tree->children[i]->plays + c*sqrt(log(N_plays)/tree->children[i]->plays);
      if (ucb > ucb_max) {
        ucb = ucb_max;
        max_child = tree->children[i];
      }
    }
    
    // Update the rows
    int* rows_temp = (int*)malloc(N_rows*sizeof(int));
    for (int m = 0; m < N_rows; m++)
      rows_temp[m] = rows[m];
    rows_temp[max_child->row] -= max_child->sticks;
    
    // Traverse down the tree
    int win = 1 - monte_carlo(max_child, rows_temp, N_rows, perturb);
    tree->wins += win;
    tree->plays++;
    
    free(rows_temp);
    return win;
  }
}


// Free a tree
void free_tree(tree_t* tree) {
  if (tree != NULL) {
    if (tree->children != NULL) {
      for (int i = 0; i < tree->total_sticks; i++) {
        free_tree(tree->children[i]);
      }
      free(tree->children);
    }
    free(tree);
  }
  return;
}


// The extended s-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks, int perturb) {
  
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
  root->children = NULL;
  root->wins = 0;
  root->plays = 0;
  root->row = -1;
  root->sticks = -1;
  root->total_sticks = total_sticks;
  
  N_plays = 0;
  int N_sims = 5000;
  for (int k = 0; k < N_sims; k++) {
    monte_carlo(root, rows, N_rows, perturb);
    N_plays++;
  }
  
  // Decide which move to make
  // (Choose the child so that the opponent has the lowest chance of winning)
  tree_t* min_child = root->children[0];
  for (int i = 1; i < total_sticks; i++) {
    if (min_child->plays > root->children[i]->plays)
      min_child = root->children[i];
  }
  res->row = min_child->row;
  res->sticks = min_child->sticks;
  
  // Free and return
  free_tree(root);
  return;
}


// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks, int perturb) {
  
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

