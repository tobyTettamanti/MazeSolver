#include "common.h"

Direction get_opposite_dir(Direction dir);

void shuffle_directions(Direction directions[]);

void drunken_walk(int row, int col, int num_rows, int num_cols,
                  struct maze_room maze[num_rows][num_cols]);

int encode_room(struct maze_room room);

void encode_maze(int num_rows, int num_cols,
                 struct maze_room maze[num_rows][num_cols],
                 int result[num_rows][num_cols]);
void change_room_side(int num_rows, int num_cols, struct maze_room maze[num_rows][num_cols],
                        int row, int col, Direction dir, int wallOrOpening);
int get_room_side(struct maze_room *room, Direction dir);
