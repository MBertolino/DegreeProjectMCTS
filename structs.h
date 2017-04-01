// Return struct
typedef struct move {
  int row;
  int sticks;
} move_t;

// Tree struct
typedef struct tree {
  struct cell** cell;
  int wins;
  int plays;
  int row;
  int sticks;
} tree_t;

// Cell struct
typedef struct cell {
  struct tree** children;
  //int row;
  //int sticks;
  int total_sticks;
} cell_t;
