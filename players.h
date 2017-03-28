#include "structs.h"

// Perturb board (move this function?)
void perturb_board(int N_rows, int* rows, int* total_sticks);

// The p-player
void p_player(move_t* res, int* rows, int N_rows, double p, int total_sticks);

// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks, int game);

// The x-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks, int game);

// The h-player
void h_player(move_t* res, int* rows, int N_rows);

// The r-player
void r_player(move_t* res, int* rows, int N_rows, int total_sticks);