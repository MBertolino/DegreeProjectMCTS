
// Return struct
typedef struct move {
  int row;
  int sticks;
} move_t;


// Tree struct
typedef struct tree {
  struct tree** children;
  int wins;
  int plays;
  int row;
  int sticks;
} tree_t;

