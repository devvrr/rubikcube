#ifndef _RBDEFS_H_
#define _RBDEFS_H_
#include <stdio.h>
#include <stdlib.h>
#define NMOVES 12
#define NCOLORS 6
#define NEDGECUBELETS 12
#define NCORNERCUBELETS 8
#define TRUE 1
#define FALSE 0
#define GET_EDGE_DIR(X) (X>>16)
#define GET_CORNER_DIR(X) ( X & 0x0000FFFF )
#define GOAL_STATE {{0,1,2,3,4,5,6,7,8,9,10,11},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,1,2,3,4,5,6,7},{0,0,0,0,0,0,0,0}}

/*moves*/
#define RED_IN 0
#define RED_OUT 1
#define PINK_IN 2
#define PINK_OUT 3

#define GREEN_IN 4
#define GREEN_OUT 5
#define BLUE_IN 6
#define BLUE_OUT 7

#define WHITE_IN 8
#define WHITE_OUT 9
#define YELLOW_IN 10
#define YELLOW_OUT 11


typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef unsigned char Boolean;

typedef char INT8;
typedef int INT32;
typedef short INT16;

typedef enum{
   red,
   pink,
   green,
   blue,
   white,
   yellow
} rbCubeColours;

typedef enum{
   red_green,
   green_red=0,
   red_blue,
   blue_red=1,
   green_pink,
   pink_green=2,
   pink_blue,
   blue_pink=3,

   yellow_blue,
   blue_yellow=4,   
   yellow_green,
   green_yellow=5,
   white_green,
   green_white=6,
   white_blue,
   blue_white=7,

   pink_yellow,
   yellow_pink=8,
   white_pink,
   pink_white=9,
   red_white,
   white_red=10,
   yellow_red,
   red_yellow=11,
   invalid_edgepos
} EDGEPOS;
typedef enum
{
   edge_dir_zero,
   edge_dir_one,
   invalid_edgedir
} EDGEDIR;
typedef enum
{
   green_pink_yellow,
   green_yellow_pink=0,
   yellow_pink_green=0,
   yellow_green_pink=0,
   pink_green_yellow=0,
   pink_yellow_green=0,
   
   red_white_green,
   red_green_white=1,
   white_green_red=1,
   white_red_green=1,
   green_red_white=1,
   green_white_red=1,
   

   blue_white_pink,
   blue_pink_white=2,
   pink_blue_white=2,
   pink_white_blue=2,
   white_blue_pink=2,
   white_pink_blue=2,

   red_yellow_blue,
   red_blue_yellow=3,
   yellow_red_blue=3,
   yellow_blue_red=3,
   blue_red_yellow=3,
   blue_yellow_red=3,
   

   red_white_blue,
   red_blue_white=4,
   blue_red_white=4,
   blue_white_red=4,
   white_blue_red=4,
   white_red_blue=4,
   
   red_green_yellow,
   red_yellow_green=5,
   yellow_green_red=5,
   yellow_red_green=5,
   green_red_yellow=5,
   green_yellow_red=5,
   
   white_green_pink,
   white_pink_green=6,
   pink_white_green=6,
   pink_green_white=6,
   green_white_pink=6,
   green_pink_white=6,
   
   yellow_blue_pink,
   yellow_pink_blue=7,
   pink_blue_yellow=7,
   pink_yellow_blue=7,
   blue_pink_yellow=7,
   blue_yellow_pink=7,
   invalid_cornerpos
} CORNERPOS;
typedef enum
{
   corner_dir_zero,
   corner_dir_one,
   corner_dir_two,
   invalid_cornerdir
} CORNERDIR;
typedef struct edge{
   EDGEPOS pos;
   EDGEDIR dir;
} EDGECUBELET;
typedef struct {
   CORNERPOS pos;
   CORNERDIR dir;
} CORNERCUBELET;
typedef struct{
   EDGECUBELET edgeSet[NEDGECUBELETS];
   CORNERCUBELET cornetSet[NCORNERCUBELETS];
} rbCubeStateOld;

typedef struct
{
   UINT8 edgePos[12];/*This takes only values 0 to 11*/
   UINT8 edgeDir[12];/*This takes only values 0 and 1*/
   UINT8 cornerPos[8];/*This takes only values 0 to 7*/
   UINT8 cornerDir[8];/*This takes only values 0, 1, 2*/
} rbCubeState;


void GetPermFromIndex( UINT8 * perm, UINT32 index, UINT8 nObjects );
UINT32 GetIndexFromPerm(UINT8 * perm, UINT8 nObjects);
void PrintMove(int id);
typedef struct
{
   UINT32 edgePos;/*The index of the permutation - max 12!*/
   UINT16 cornerPos;/*The index of the permutation - max 8!*/
   UINT32 dir;/*left two bytes for edge direction value index, right two bytes for
   corner direction value index*/
} rbCompState;/*compressed state*/


/*The way user describes a cube - the nine colours on each face*/
typedef UINT8 UserCubeDesc[6][3][3];

Boolean BelongToSameRedGraph(rbCubeState s1, rbCubeState s2);
void ReadUserCubeDesc(UserCubeDesc cube);

typedef unsigned char rIndex[5];
void PrintFull( rbCubeState * s );
void PopulateRedGroups(char fileName[20]);
unsigned char CompareFullStates( rbCubeState * s1, rbCubeState * s2);


#define PRINT_TEXT(s) //printf(s)
#define PRINT_TEXT_1INT(s,i) //printf(s,i)
#define PRINT_TEXT_2INTS(s,i,j) //printf(s,i,j)

#endif//ifndef _RBDEFS_H_
