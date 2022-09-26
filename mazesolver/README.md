# 1_maze

Algorithms and Data Structures:

My maze was created by using a 2D array of "rooms", whose most prominent attribute is their four edges, or "connections" as they will be called from now on. 
Each connection is an int with either a '0' (an opening) or a '1' (a wall). To fill out the maze, we use the drunken_walk algorithm, which stumbles across the maze 
randomly until every room is discovered, creating openings whenever it goes from one room to another, assuming that room has not been visited yet. 
This way, we are guaranteed to have a path from one room to another.

Once our maze has been generated, we now can solve it from any room in the maze. We use a depth-first search to go from room to room and, 
by marking where we go, we eventually reach the desired end goal. Once we have our path, we use a linked-list of each room in the path's next 
room to store the path. From there, we either use print said pathway (pruned path) or we print each new room that we visited in our depth-first search(full path).


Misc :

9/19/21
I just tested my generator code and got

6666666666
6666666666
6666666666
6666666666
6666666666
6666666666
6666666666
6666666666
6666666666
6666666666

My code is actually cursed please send help

