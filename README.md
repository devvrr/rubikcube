# rubikcube
This program finds optimal solutions for Rubik's cube upto limited depth. Currently it can find optimal solutions for nodes upto depth 14.

The program exhaustively generates nodes of the cube starting from Goal state.
- It can generate nodes upto depth 7 from "Goal node" --- called Graph (this takes about 2 minutes)
- And then, for a given "problem node"/"scrambled state",
    -- it takes each node from the list of neighbours of "Goal" generated earlier(Graph) - call it "reference node"
       -- "reference node" is obtained when a sequence of moves are made on "Goal"
       -- finds the "resultant node" that would be obtained when the same sequence of moves were made on "problem node"
          -- the function that finds the "res node" is called "config" function. This function takes just the "problem node"
          and "ref node" as input
       -- checks if the "res node" is in the "Graph"
       -- if yes -- we have found a path

## Notation:
Goal node: the solved state of the Rubik cube

It is assumed that the Rubik cube has faces with the 6 colours:
        Red,
        Pink,
        Green,
        Blue,
        Yellow,
        White
        
Red and Pink opposite to each other
Green and Blue opposite to each other
Yellow and White opposite to each other

We can make 12 turns from any cube position/state/configuration
The 12 turns are:
  RED_IN 0
  RED_OUT 1
  PINK_IN 2
  PINK_OUT 3
  
  GREEN_IN 4
  GREEN_OUT 5
  BLUE_IN 6
  BLUE_OUT 7
  
  WHITE_IN 8
  WHITE_OUT 9
  YELLOW_IN 10
  YELLOW_OUT 11
 
 Meaning of "in" and "out" in a move::
     Assume that a Red move is made by holding the Red face/layer with right hand. And let us say the thumb is pointed as in "Thumbs-up". Now if the thumb is pointing "into" the cube, we call it "Red-in" move. Basically the axis of rotation is pointing "into" the cube. If the thumb is pointing out of the cube, the move is called "Red-out". Similarly for all the faces.

## Compiling the Program:
use the file "rb_full_hash_linked_list_compile" to compile the code/project from the "rubikcube" folder

## Running the program:
    1. run "a.out"
    2. enter 7 for "number of levels"
    3. The program takes about 2 minutes for its calculations
    4. now it asks if you want a user config or generated config.
        Enter "0" (user config) if you have a cube in a particular state to which you want to try to find solution using
        the program. Plea refer to comments in rubikcube/user_desc/convert_user_form.c to know how to input a user config
        in the form readable by the program.
    5. The program may take 10 to 30 seconds to calculate the path. It may not be able to find the path for some nodes.
    It can't find path for nodes farther 14 moves from the Goal
        

# Tripod state
 Let's call a state where all the positions are solved except the cubelets on three edges coming out of a particular corner as
 tripod state
 
 It is found using this program that many of the tripod states have shortest path to the Goal of length less than or equal
 to 14.
 
 tripod3.txt - has a state with all the cubelets in solved position and orientation except 2 corner cubelets with
 un-solved orientation. This state has a shortest path to Goal of length 14.
