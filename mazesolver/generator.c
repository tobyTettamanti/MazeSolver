#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"

/*
 * Given a direction, get its opposite
 *
 * Parameters:
 *  - dir: direction
 *
 * Returns:
 *  - the opposite direction to dir
 */
Direction get_opposite_dir(Direction dir) {
switch (dir) {
    case WEST: return EAST;
    case EAST: return WEST;
    case NORTH: return SOUTH;
    case SOUTH: return NORTH;
    }
    return EAST;
}

/*
 * Given an array of the four directions, randomizes the order of the directions
 *
 * Parameters:
 *  - directions: an array of Direction enums of size 4
 *
 * Returns:
 *  - nothing - the array should be shuffled in place
 */
void shuffle_array(Direction directions[]) { // NOT YET TESTED
    for (int i = 0; i < 4; i++) {
        int r = rand();
        r = (r % (4 - i));

        Direction d1 = directions[i];
        Direction d2 = directions[r];
        directions[i] = d2;
        directions[r] = d1;
    }
}

/*
 * Recursive function for stepping through a maze and setting its
 *connections/walls using the drunken_walk algorithm
 *
 * Parameters:
 *  - row: row of the current room
 *  - col: column of the current room
 *  - num_rows: number of rows in the maze
 *  - num_cols: number of columns in the maze
 *  - maze: a 2D array of maze_room structs representing your maze
 * Returns:
 *  - nothing - the state of your maze should be saved in the maze array
 *being passed in (make sure to use pointers correctly!).
 */
void drunken_walk(int row, int col, int num_rows, int num_cols,
                  struct maze_room maze[num_rows][num_cols]) {
    struct maze_room *room = &maze[row][col];
    room->visited = 1;
    Direction d[4] = {NORTH, WEST, SOUTH, EAST};
    shuffle_array(d);
    for (int i = 0; i < 4; i++) {
        int horMovement = 0;
        int verMovement = 0;
        switch (d[i]) {
            case WEST: {
              horMovement = -1;
              break;
            }
            case EAST: {
              horMovement = 1;
              break;
            }
            case NORTH: {
              verMovement = -1;
              break;
            }
            case SOUTH: {
              verMovement = 1;
              break;
            }
        }
        if (row + verMovement >= num_rows || col + horMovement >= num_cols || row + verMovement < 0 || col + horMovement < 0) {
            change_room_side(num_rows, num_cols, maze, row, col, d[i], 1);
        }
        else {
            struct maze_room *neighbor = &maze[row + verMovement][col + horMovement];
            neighbor->row = row + verMovement;
            neighbor->column = col + horMovement;
            if (neighbor->visited == 0) {
                change_room_side(num_rows, num_cols, maze, row, col, d[i], 0);
                drunken_walk(neighbor->row, neighbor->column, num_rows, num_cols, maze);
            }
            else {
                Direction opposite = get_opposite_dir(d[i]);
                int wallOrOpening = get_room_side(neighbor, opposite);
                change_room_side(num_rows, num_cols, maze, row, col, d[i], wallOrOpening);
            }
        }
    }
}

/*
 * Represents a maze_room struct as an integer based on its connections
 *
 * Parameters:
 *  - room: a struct maze_room to be converted to an integer
 *
 * Returns:
 *  - the integer representation of a room
 */
int encode_room(struct maze_room room) {
  int roomcode = room.connections[0] + (room.connections[1] * 2) +
  (room.connections[2] * 4) + (room.connections[3] * 8);
  return roomcode;
}

/*
 * Represents a maze_room array as a hexadecimal array based on its connections
 *
 * Parameters:
 *  - num_rows: number of rows in the maze
 *  - num_cols: number of columns in the maze
 *  - maze: a 2D array of maze_room structs representing the maze
 *  - result: a 2D array of hexadecimal numbers representing the maze
 *
 * Returns:
 *  - nothing - each maze_room in the maze should be represented
 *    as a hexadecimal number  and put in nums at the corresponding index
 */
void encode_maze(int num_rows, int num_cols,
                 struct maze_room maze[num_rows][num_cols],
                 int result[num_rows][num_cols]) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            result[i][j] = encode_room(maze[i][j]);
        }
    }
}

/*
 * Writes encoded maze to an output file
 *
 * Parameters:
 *  - num_rows: number of rows in the maze
 *  - num_cols: number of columns in the maze
 *  - encoded_maze: a maze_room array as a hexadecimal array based on its
 *    connections
 *  - file_name: the name of the output file for the encoded maze
 *
 * Returns:
 *  - 1 if an error occurs, 0 otherwise
 */
int write_encoded_maze_to_file(int num_rows, int num_cols,
                               int encoded_maze[num_rows][num_cols],
                               char *file_name) {
    int err = 0;

    // open file (create it if necessary)
    FILE *f = fopen(file_name, "w+");
    if (f == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }
    // write each room's hex value into file
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            int encoded_room = encoded_maze[i][j];
            err = fprintf(f, "%x", encoded_room);
            if (err < 0) {
                fprintf(stderr, "Writing to file failed: %i\n", err);
                return 1;
            }
        }
        // add a newline between rows
        err = fprintf(f, "\n");
        if (err < 0) {
            fprintf(stderr, "Writing to file failed: %i\n", err);
            return 1;
        }
    }
    // close file
    int close = fclose(f);
    if (close == EOF) {
        fprintf(stderr, "Could not close file.\n");
        return 1;
    }
    return 0;
}

/*
Changes the room side depending on the input wallOrOpening and its direction.

Parameters:
 - num_rows: the vertical dimension of the maze
 - num_cols: the horizontal dimension of the maze
 - maze: the maze being generated
 - row: the row of the desired room
 - col: the column of the desired room
 - dir: the direction of the room that we want to modify
 - wallOrOpening: 0 if the room's side is changed into an opening, 1 if the room's side is changed into a wall

 Returns:
  - Nothing: the room side is modified without returning any value.
*/
void change_room_side(int num_rows, int num_cols, struct maze_room maze[num_rows][num_cols],
                     int row, int col, Direction dir, int wallOrOpening) { // wallOrOpening = 0 if opening, 1 if wall)
    struct maze_room *r = &maze[row][col];
            switch (dir) {
            case EAST: {
              r->connections[3] = wallOrOpening;
              break;
            }
            case WEST: {
              r->connections[2] = wallOrOpening;
              break;
            }
            case SOUTH: {
              r->connections[1] = wallOrOpening;
              break;
            }
            case NORTH: {
              r->connections[0] = wallOrOpening;
              break;
            }
        }
}
/*
Gets the side of the room given the direction and the room:
it returns 0 if there is an opening in the room's direction, and 1 if there is a wall.

Parameters:
  - *room: the maze square that we want to find its side
  - dir: the direction in the room that we use to find the side

Returns:
  - 1 if the side is a wall, 0 if the side is an opening, and -1 if there is an error.
*/
int get_room_side(struct maze_room *room, Direction dir) { // return 0 if opening, 1 if wall
    switch (dir) {
    case EAST: {
      return room->connections[3];
    }
    case WEST: {
      return room->connections[2];
    }
    case SOUTH: {
      return room->connections[1];
    }
    case NORTH: {
      return room->connections[0];
    }
  }
  return -1; //ERROR if -1!
}
/*
 * Main function
 *
 * Parameters:
 *  - argc: the number of command line arguments - for this function 4
 *  - **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *            ["generator", <output file>, <number of rows>, <number of
 *columns>]
 *
 * Returns:
 *  - 0 if program exits correctly, 1 if there is an error
 */

int main(int argc, char **argv) {
    char *file_name;
    int num_rows;
    int num_cols;
    srand(time(NULL)); // set seed to current time
    if (argc != 4) {
        printf("Incorrect number of arguments.\n");
        printf(
            "./generator <output file> <number of rows> <number of columns>\n");
        return 1;
    } else {
        file_name = argv[1];
        num_rows = atoi(argv[2]);
        num_cols = atoi(argv[3]);
    }
    if (num_rows <= 0 || num_cols <= 0) {
    printf("given maze dimensions are negative");
    return 1;
    }
    struct maze_room maze[num_rows][num_cols];
    initialize_maze(num_rows, num_cols, maze);
    drunken_walk(0, 0, num_rows, num_cols, maze);

    int result[num_rows][num_cols];
    encode_maze(num_rows, num_cols, maze, result);
    int a = write_encoded_maze_to_file(num_rows, num_cols, result, file_name);
    if (a == 1) {
        printf("Error occured while encoding maze into %s", file_name);
        return 1;
    }

    return 0;
}
