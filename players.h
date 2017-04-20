#include "structs.h"

// Perturb board (move this function?)
void perturb_board(int N_rows, int* rows, int* total_sticks, double perturb);

// Probaility of nimsum
double prob_nimsum(int* rows, int N_rows, int total_sticks, int phi, double perturb);

// The p-player
void p_player(move_t* res, int* rows, int N_rows, double p, int total_sticks);

// The q-player
void q_player(move_t* res, int* rows, int N_rows, double q, int total_sticks, double perturb);

// The s-player
void s_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb);

// The x-player
void x_player(move_t* res, int* rows, int N_rows, int total_sticks, double perturb);

// The h-player
void h_player(move_t* res, int* rows, int N_rows);

// The r-player
void r_player(move_t* res, int* rows, int N_rows, int total_sticks);