
// Return struct
typedef struct move {
  int row;
  int sticks;
} move_t;


// Tree struct
typedef struct tree {
  struct tree** children;
  struct tree** perturbations;
  int wins;
  int plays;
  int row;
  int sticks;
  int total_sticks;
} tree_t;

