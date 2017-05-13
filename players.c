#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include <math.h>


// Simulate the game with random moves and see if this generates a win
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
    int rand_mov = 1 + (double)total_sticks*rand()/((double)RAND_MAX + 1);
    for (int i = 0; i < N_rows; i++) {
      if (rows_temp[i] >= rand_mov) {
        rows_temp[i] -= rand_mov;
        total_sticks -= rand_mov;
        break;
      } else {
        rand_mov -= rows_temp[i];
      }
    }

    // Randomly perturb the board
    if ((double)rand()/RAND_MAX < perturb) {
      rows_temp[0]++;
      total_sticks++;
    }
    
    // See if the game has ended
    if (total_sticks <= 0)
      break;
  }
  
  free(rows_temp);
  return win;
}


// Simulate the game with MCTS and see if this generates a win
int monte_carlo(tree_t* tree, int* rows, int N_rows, double perturb, double c, int perturb_board) {
  int total_sticks = tree->total_sticks;
  
  // If the board is to be perturbed
  if (perturb_board == 1) {
    
    // Allocate memory (if needed)
    if (tree->perturbation == NULL) {
      tree->perturbation = (tree_t*)malloc(sizeof(tree_t));
      tree->perturbation->children = NULL;
      tree->perturbation->perturbation = NULL;
      tree->perturbation->wins = 0;
      tree->perturbation->plays = 0;
      tree->perturbation->row = tree->row;
      tree->perturbation->sticks = tree->sticks;
      tree->perturbation->total_sticks = total_sticks + 1;
    }
    
    // Update the board according to the perturbation
    int* rows_temp = (int*)malloc(N_rows*sizeof(int));
    for (int m = 0; m < N_rows; m++)
      rows_temp[m] = rows[m];
    rows_temp[0]++;
    
    // Traverse down the tree
    int win = monte_carlo(tree->perturbation, rows_temp, N_rows, perturb, c, 0);
    tree->wins += win;
    tree->plays++;
    
    free(rows_temp);
    return win;
  }
  
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
        tree->children[index]->perturbation = NULL;
        tree->children[index]->wins = 0;
        tree->children[index]->plays = 0;
        tree->children[index]->row = i;
        tree->children[index]->sticks = j;
        tree->children[index]->total_sticks = total_sticks - j;
        index++;
      }
    }
    
    // Simulate random moves
    index = (double)total_sticks*rand()/((double)RAND_MAX + 1);
    int win = random_move(rows, N_rows, total_sticks, tree->children[index]->row, tree->children[index]->sticks, perturb);
    tree->wins += win;
    tree->plays++;
    
    return win;
  }
  
  // If this is an internal node
  else {
    
    // Find the child with highest ucb
    tree_t* max_child = tree->children[0];
    double ucb_max = (double)max_child->wins/max_child->plays + c*sqrt(log(tree->plays)/max_child->plays);
    //double ucb_max = 2*max_child->wins - max_child->plays + c*sqrt(log(tree->plays)/max_child->plays);
    //double ucb_max = 2.*max_child->wins/max_child->plays - 1 + c*sqrt(log(tree->plays)/max_child->plays);
    double ucb;
    for (int i = 1; i < total_sticks; i++) {
      
      // If no plays has been made
      if (tree->children[i]->plays == 0) {
        max_child = tree->children[i];
        break;
      }
      
      // Compute ucb
      ucb = (double)tree->children[i]->wins/tree->children[i]->plays + c*sqrt(log(tree->plays)/tree->children[i]->plays);
      //ucb = 2*tree->children[i]->wins - tree->children[i]->plays + c*sqrt(log(tree->plays)/tree->children[i]->plays);
      //ucb = 2.*tree->children[i]->wins/tree->children[i]->plays - 1 + c*sqrt(log(tree->plays)/tree->children[i]->plays);
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
    
    // Perturb the tree
    perturb_board = (double)rand()/((double)RAND_MAX + 1) < perturb ? 1 : 0;
    
    // Traverse down the tree
    int win = 1 - monte_carlo(max_child, rows_temp, N_rows, perturb, c, perturb_board);
    tree->wins += win;
    tree->plays++;
    
    free(rows_temp);
    return win;
  }
}


// Free a tree
void free_tree(tree_t* tree, int N_rows) {
  if (tree != NULL) {
    if (tree->children != NULL) {
      for (int i = 0; i < tree->total_sticks; i++)
        free_tree(tree->children[i], N_rows);
      free(tree->children);
    }
    if (tree->perturbation != NULL) {
      free_tree(tree->perturbation, N_rows);
    }
    free(tree);
  }
  return;
}


// The extended s-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb, double c) {
  
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
  root->perturbation = NULL;
  root->wins = 0;
  root->plays = 0;
  root->row = -1;
  root->sticks = -1;
  root->total_sticks = total_sticks;
  
  // Simulate games
  int N_sims = 1000;
  for (int k = 0; k < N_sims; k++) {
    monte_carlo(root, rows, N_rows, perturb, c, -1);
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
  free_tree(root, N_rows);
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
  int N_sims = 1000./total_sticks;
  for (int k = 0; k < N_sims; k++)
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
void p_player(move_t* res, int* rows, int N_rows, int total_sticks, double p) {
  
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
  if (X == 0 || p < (double)rand()/((double)RAND_MAX + 1)) {
    do {
      row = (double)N_rows*rand()/((double)RAND_MAX + 1);
    } while (rows[row] == 0);
    sticks = 1 + (double)rows[row]*rand()/((double)RAND_MAX + 1);
    
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


// Returns the probability that the nim sum is equal to phi
double prob_nimsum(int* rows, int N_rows, int total_sticks, int phi, double perturb) {
  
  // If phi is negative
  if (phi < 0)
    return 0;
  
  // Base case: empty board
  if (total_sticks == 0)
    return pow(perturb, phi)*(1 - perturb);
  
  // Special case: all sticks in the alpha heap
  if (rows[0] == total_sticks) {
    if (phi > rows[0])
      return pow(perturb, phi - rows[0])*(1 - perturb)*(2 - perturb);
    else if (phi == rows[0])
      return (1 - perturb)*(1 - perturb);
    else
      return 0;
  }
  
  // Params for recursive formula
  int alpha = rows[0];
  int b = 0;
  for (int i = 1; i < N_rows; i++) {
    b ^= rows[i];
  }
  
  // Allocate rows_temp
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
  
  /* Compute probabilities */
  // Case 1: Not perturbed alpha
  double prob1 = 0;
  for (int i = 0; i <= alpha-1; i++) {
    rows_temp[0] = i;
    prob1 += prob_nimsum(rows_temp, N_rows, total_sticks - alpha + i, (i + (b^phi) - alpha)^b, perturb);
  }
  prob1 *= (1 - perturb)/total_sticks;
  
  // Case 2: Perturbed row alpha
  double prob2 = 0;
  for (int i = 0; i <= alpha; i++) {
    rows_temp[0] = i;
    prob2 += prob_nimsum(rows_temp, N_rows, total_sticks - alpha + i, (i + (b^phi) - alpha - 1)^b, perturb);
  }
  prob2 *= perturb/(total_sticks + 1);
  
  // Case 3: Not perturbed beta
  rows_temp[0] = rows[0];
  double prob3 = 0;
  for (int i = 1; i < N_rows; i++) {
    if (rows[i] == total_sticks) {
      prob3 = pow(perturb, phi^rows[i])*(1 - perturb)*(1 - perturb);
      break;
    }
  }
  if (prob3 == 0) {
    for (int j = 1; j < N_rows; j++) {
      for (int i = 0; i < rows[j]; i++) {
        rows_temp[j] = i;
        prob3 += prob_nimsum(rows_temp, N_rows, total_sticks - rows[j] + i, (i^rows[j]^phi), perturb);
      }
      rows_temp[j] = rows[j];
    }
    prob3 *= (1 - perturb)/total_sticks;
  }
  
  // Case 4: Perturbed beta
  rows_temp[0] = rows[0] + 1;
  double prob4 = 0;
  for (int j = 1; j < N_rows; j++) {
    for (int i = 0; i < rows[j]; i++) {
      rows_temp[j] = i;
      prob4 += prob_nimsum(rows_temp, N_rows, total_sticks - rows[j] + i + 1, (i^rows[j]^phi), perturb);
    }
    rows_temp[j] = rows[j];
  }
  prob4 *= perturb/(total_sticks + 1);
  
  free(rows_temp);
  return prob1 + prob2 + prob3 + prob4;
}


// Probability that a move generates a win
double prob_win(int* rows, int N_rows, int total_sticks, double perturb) {
  
  // Base case: empty board
  if (total_sticks == 0)
    return 1./(1 + perturb);
  
  // Special case: all sticks in the alpha heap
  if (rows[0] == total_sticks) {
    return (double)perturb/(1 + perturb);
  }
  
  // Allocate rows_temp
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
  
  /* Compute probabilities */
  double prob1 = 0;
  for (int i = 0; i <= rows[0]-1; i++) {
    rows_temp[0] = i;
    prob1 += 1 - prob_win(rows_temp, N_rows, total_sticks - rows[0] + i, perturb);
  }
  prob1 *= (1 + total_sticks - perturb)/(double)total_sticks;
  
  rows_temp[0] = rows[0];
  double prob2 = 0;
  int flag = 0;
  for (int i = 1; i < N_rows; i++) {
    if (rows[i] == total_sticks) {
      prob2 = 1 - 1./(1 + perturb);
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    for (int j = 1; j < N_rows; j++) {
      for (int i = 0; i < rows[j]; i++) {
        rows_temp[j] = i;
        prob2 += 1 - prob_win(rows_temp, N_rows, total_sticks - rows[j] + i, perturb);
      }
      rows_temp[j] = rows[j];
    }
    prob2 *= (1 + total_sticks)*(1 - perturb)/(double)total_sticks;
  }
  
  rows_temp[0] = rows[0] + 1;
  double prob3 = 0;
  for (int j = 1; j < N_rows; j++) {
    for (int i = 0; i < rows[j]; i++) {
      rows_temp[j] = i;
      prob3 += 1 - prob_win(rows_temp, N_rows, total_sticks - rows[j] + i + 1, perturb);
    }
    rows_temp[j] = rows[j];
  }
  prob3 *= perturb;
  
  free(rows_temp);
  return (double)(prob1 + prob2 + prob3 + perturb)/(1 + total_sticks + perturb);
}


// The q-player
void q_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb, double q) {
  
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
  
  // Initialize res with a random move
  do {
    res->row = (double)N_rows*rand()/((double)RAND_MAX + 1);
  } while (rows[res->row] == 0);
  res->sticks = 1 + (double)rows[res->row]*rand()/((double)RAND_MAX + 1);
  
  // Return the random move with probability q
  if ((double)rand()/((double)RAND_MAX + 1) >= q)
    return;
  
  // Allocate rows_temp
  int* rows_temp = (int*)malloc(N_rows*sizeof(int));
  for (int i = 0; i < N_rows; i++) {
    rows_temp[i] = rows[i];
  }
    
  // Choose the move with the best probability
  double prob;
  double prob_max = 0;
  for (int i = 0; i < N_rows; i++) {
    for (int j = 1; j <= rows[i]; j++) {
      rows_temp[i] = rows[i] - j;
      //prob = prob_nimsum(rows_temp, N_rows, total_sticks - j, 0, perturb);
      prob = prob_win(rows_temp, N_rows, total_sticks - j, perturb);
      
      if (prob > prob_max) {
        prob_max = prob;
        res->row = i;
        res->sticks = j;
      }
    }
    rows_temp[i] = rows[i];
  }
  
  free(rows_temp);
  return;
}


// The human player
void h_player(move_t* res, int* rows, int N_rows) {
  int row, sticks;
  
  while (1) {
    int ret = scanf("%i%i", &row, &sticks); // cannot read strings
    row--;
    if (ret <= 0) {
      printf("Illegal input: Enter numbers please. Try again:\n");
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
  int rand_mov = 1 + (double)total_sticks*rand()/((double)RAND_MAX + 1);
  for (int i = 0; i < N_rows; i++) {
    if (rows[i] >= rand_mov) {
      res->row = i;
      res->sticks = rand_mov;
      break;
    } else {
      rand_mov -= rows[i];
    }
  }
  
  return;
}

