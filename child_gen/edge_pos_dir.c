#include "../rbdefs.h"


const static UINT8 EdgePosMoveMapping[NMOVES][NEDGECUBELETS] =
{
   /*red-in*/    {10,11,2,3,4,5,6,7,8,9,1,0},/*0,1,10,11*/
   /*red-out*/   {11,10,2,3,4,5,6,7,8,9,0,1},/*0,1,10,11*/
   /*pink-in*/   {0,1,8,9,4,5,6,7,3,2,10,11},/*2,3,8,9*/
   /*pink-out*/  {0,1,9,8,4,5,6,7,2,3,10,11},/*2,3,8,9*/

   /*green-in*/  {5,1,6,3,4,2,0,7,8,9,10,11},/*0,2,5,6*/
   /*green-out*/ {6,1,5,3,4,0,2,7,8,9,10,11},/*0,2,5,6*/
   /*blue-in*/   {0,7,2,4,1,5,6,3,8,9,10,11},/*1,3,4,7*/
   /*blue-out*/  {0,4,2,7,3,5,6,1,8,9,10,11},/*1,3,4,7*/

   /*white-in*/  {0,1,2,3,4,5,9,10,8,7,6,11},/*6,7,9,10*/
   /*white-out*/ {0,1,2,3,4,5,10,9,8,6,7,11},/*6,7,9,10*/
   /*yellow-in*/ {0,1,2,3,8,11,6,7,5,9,10,4},/*4,5,8,11*/
   /*yellow-out*/{0,1,2,3,11,8,6,7,4,9,10,5} /*4,5,8,11*/
};

/*Finds the child configuration in the edge cubelet position value graph for a
given parent and move number
and puts it into the given config*/
void Find_child_edge_pos( rbCubeState * parent, rbCubeState * child, UINT8 move )
{
	unsigned int i;

	for( i=0;i<NEDGECUBELETS;i++){
		child->edgePos[i] = parent->edgePos[i];
	}
#define EDIR 0x10
            /*5th bit is the direction value. so flip it to change it*/
	switch( move ){
		case GREEN_IN: /*green in*/
            //printf("green in\n");
            //printf("parent->edgePos[green_red]=%d\n",parent->edgePos[green_red]);
			child->edgePos[green_yellow] =  parent->edgePos[green_red] ^ EDIR;
			child->edgePos[green_red] =     parent->edgePos[green_white] ^ EDIR;
			child->edgePos[green_white] =   parent->edgePos[green_pink] ^ EDIR;
			child->edgePos[green_pink] =    parent->edgePos[green_yellow] ^ EDIR;
			break;
		case GREEN_OUT: /*green out*/
            //printf("green out\n");
            //printf("parent->edgePos[green_pink]=%d\n",parent->edgePos[green_pink]);
			child->edgePos[green_yellow] =  parent->edgePos[green_pink] ^ EDIR;
			child->edgePos[green_pink] =    parent->edgePos[green_white] ^ EDIR;
			child->edgePos[green_white] =   parent->edgePos[green_red] ^ EDIR;
			child->edgePos[green_red] =     parent->edgePos[green_yellow] ^ EDIR;

			break;
		case BLUE_IN: /*blue in*/
			child->edgePos[blue_yellow] =  parent->edgePos[blue_pink] ^ EDIR;
			child->edgePos[blue_pink] =    parent->edgePos[blue_white] ^ EDIR;
			child->edgePos[blue_white] =   parent->edgePos[blue_red] ^ EDIR;
			child->edgePos[blue_red] =     parent->edgePos[blue_yellow] ^ EDIR;
			break;
		case BLUE_OUT: /*blue out*/
			child->edgePos[blue_yellow] =  parent->edgePos[blue_red] ^ EDIR;
			child->edgePos[blue_red] =     parent->edgePos[blue_white] ^ EDIR;
			child->edgePos[blue_white] =   parent->edgePos[blue_pink] ^ EDIR;
			child->edgePos[blue_pink] =    parent->edgePos[blue_yellow] ^ EDIR;
			break;
		case RED_IN: /*red in*/
			child->edgePos[red_white] =   parent->edgePos[red_green] ^ EDIR;
			child->edgePos[red_green] =   parent->edgePos[red_yellow] ^ EDIR;
			child->edgePos[red_yellow] =  parent->edgePos[red_blue] ^ EDIR;
			child->edgePos[red_blue] =    parent->edgePos[red_white] ^ EDIR;
			break;
		case RED_OUT: /*red out*/
			child->edgePos[red_white] =   parent->edgePos[red_blue] ^ EDIR;
			child->edgePos[red_blue] =    parent->edgePos[red_yellow] ^ EDIR;
			child->edgePos[red_yellow] =  parent->edgePos[red_green] ^ EDIR;
			child->edgePos[red_green] =   parent->edgePos[red_white] ^ EDIR;
			break;
		case PINK_IN: /*pink in*/
			child->edgePos[pink_white] =   parent->edgePos[pink_blue] ^ EDIR;
			child->edgePos[pink_blue] =    parent->edgePos[pink_yellow] ^ EDIR;
			child->edgePos[pink_yellow] =  parent->edgePos[pink_green] ^ EDIR;
			child->edgePos[pink_green] =   parent->edgePos[pink_white] ^ EDIR;
			break;
		case PINK_OUT: /*pink out*/
			child->edgePos[pink_white] =   parent->edgePos[pink_green] ^ EDIR;
			child->edgePos[pink_green] =   parent->edgePos[pink_yellow] ^ EDIR;
			child->edgePos[pink_yellow] =  parent->edgePos[pink_blue] ^ EDIR;
			child->edgePos[pink_blue] =    parent->edgePos[pink_white] ^ EDIR;
			break;
		case WHITE_IN: /*white in*/
			child->edgePos[white_blue] =   parent->edgePos[white_pink] ^ EDIR;
			child->edgePos[white_pink] =   parent->edgePos[white_green] ^ EDIR;
			child->edgePos[white_green] =  parent->edgePos[white_red] ^ EDIR;
			child->edgePos[white_red] =    parent->edgePos[white_blue] ^ EDIR;
			break;
		case WHITE_OUT: /*white out*/
			child->edgePos[white_blue] =   parent->edgePos[white_red] ^ EDIR;
			child->edgePos[white_red] =    parent->edgePos[white_green] ^ EDIR;
			child->edgePos[white_green] =  parent->edgePos[white_pink] ^ EDIR;
			child->edgePos[white_pink] =   parent->edgePos[white_blue] ^ EDIR;
			break;
		case YELLOW_IN: /*yellow in*/
			child->edgePos[yellow_green] =  parent->edgePos[yellow_pink] ^ EDIR;
			child->edgePos[yellow_pink] =   parent->edgePos[yellow_blue] ^ EDIR;
			child->edgePos[yellow_blue] =   parent->edgePos[yellow_red] ^ EDIR;
			child->edgePos[yellow_red] =    parent->edgePos[yellow_green] ^ EDIR;
			break;
		case YELLOW_OUT: /*yellow out*/
			child->edgePos[yellow_green] =  parent->edgePos[yellow_red] ^ EDIR;
			child->edgePos[yellow_red] =    parent->edgePos[yellow_blue] ^ EDIR;
			child->edgePos[yellow_blue] =   parent->edgePos[yellow_pink] ^ EDIR;
			child->edgePos[yellow_pink] =   parent->edgePos[yellow_green] ^ EDIR;
			break;
		default:
			printf("error\n");
			break;
	}
}


