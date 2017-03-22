#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include <math.h>

int N_plays;
const double c = sqrt(2);

int random_move(int* rows, int N_rows, int total_sticks, int row, int sticks) {
  
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


int monte_carlo(tree_t* tree, int* rows, int N_rows) {
  int total_sticks = tree->total_sticks;
  
  // See if a winning move is possible
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] == 0)
      continue;
    if (rows[i] == total_sticks) {
      tree->win += 1;
      tree->plays += 1;
      return 1;
    }
    break;
  }
  
  // If this is a leaf node
  if (tree->children[0] == NULL) {
    int index = 0;
    for (int i = 0; i < N_rows; i++) {
      for (int j = 1; j <= rows[i]; j++) {
        tree->children[index] = (tree_t*)malloc(sizeof(tree_t));
        tree->children[index]->children = (tree_t**)malloc((total_sticks-j)*sizeof(tree_t*));
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
    int win = 1 - random_move(rows, N_rows, total_sticks, tree->children[index]->row, tree->children[index]->sticks);
    tree->children[index]->win += 1 - win;
    tree->children[index]->plays++;
    tree->win += win;
    tree->plays++;
    return win;
  }
  
  // If this is an internal node
  else {
    
    // Find child not yet played
    for (int i = 0; i < total_sticks; i++) {
      if (tree->children[i]->plays == 0) {
        int win = 1 - monte_carlo(tree->children[i], rows, N_rows);
        tree->win += win;
        tree->plays++;;
        return win;
      }
    }
    
    // Find the child with highest ucb
    tree_t* max_child = tree->children[0];
    double ucb_max = max_child->wins/max_child->plays + c*sqrt(log(max_child->plays)/N_plays);
    double ucb;
    for (int i = 1; i < total_sticks; i++) {
      ucb = tree->children[i]->wins/tree->children[i]->plays + c*sqrt(log(tree->children[i]->plays)/N_plays);
      if (ucb > ucb_max) {
        ucb = ucb_max;
        max_child = tree->children[i];
      }
    }
    
    // Traverse down the tree
    int win = 1 - monte_carlo(max_child, rows, N_rows);
    tree->win += win;
    tree->plays++;
    return win;
  }
}


// Freez the treez
void free_tree(tree_t* tree) {
  if (tree != NULL) {
    for (int i = 0; i < (*tree)->total_sticks; i++) {
      free_tree(tree->children[i]);
    }
    free(tree->children);
    free(tree);
  }
}


// The extended s-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks) {
  
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
  root->children = (tree_t**)malloc(total_sticks*sizeof(tree_t*));
  root->wins = 0;
  root->plays = 0;
  root->total_sticks = total_sticks;
  N_plays = 0;
  
  N_sims = 1000;
  for (int k = 0; k < N_sims; k++) {
    monte_carlo(root, rows, N_rows);
  }
  
  // Decide which move to make
  tree_t* max_child = root->children[0];
  for (int i = 1; i < total_sticks; i++) {
    if (max_child->plays < root->children[i]->plays)
      max_child = root->children[i];
  }
  res->row = max_child->row;
  res->sticks = max_child->sticks;
  
  // Freez the treez
  free_tree(root);
}


// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks) {
  
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
  
  // Simulate moves using Monte Carlo
  for (int k = 0; k < 10000; k++)
    for (int i = 0; i < N_rows; i++)
      for (int j = 0; j < rows[i]; j++)
        stats[i][j] += random_move(rows, N_rows, total_sticks, i, j+1);
  
  printf("stats = \n");
  for (int m = 0; m < N_rows; m++) {
    for (int j = rows[m]-1; j >= 0; j--) {
      printf("%i ", stats[m][j]);
    }
    printf("\n");
  }
  printf("\n");
  
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













