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
		child->edgeDir[i] = parent->edgeDir[i];
	}

	switch( move ){
		case GREEN_IN: /*green in*/
			child->edgePos[green_yellow] =  parent->edgePos[green_red];
			child->edgeDir[green_yellow] = !parent->edgeDir[green_red];
			child->edgePos[green_red] =     parent->edgePos[green_white];
			child->edgeDir[green_red] =    !parent->edgeDir[green_white];
			child->edgePos[green_white] =   parent->edgePos[green_pink];
			child->edgeDir[green_white] =  !parent->edgeDir[green_pink];
			child->edgePos[green_pink] =    parent->edgePos[green_yellow];
			child->edgeDir[green_pink] =   !parent->edgeDir[green_yellow];
			break;
		case GREEN_OUT: /*green out*/
			child->edgePos[green_yellow] =  parent->edgePos[green_pink];
			child->edgeDir[green_yellow] = !parent->edgeDir[green_pink];
			child->edgePos[green_pink] =    parent->edgePos[green_white];
			child->edgeDir[green_pink] =   !parent->edgeDir[green_white];
			child->edgePos[green_white] =   parent->edgePos[green_red];
			child->edgeDir[green_white] =  !parent->edgeDir[green_red];
			child->edgePos[green_red] =     parent->edgePos[green_yellow];
			child->edgeDir[green_red] =    !parent->edgeDir[green_yellow];

			break;
		case BLUE_IN: /*blue in*/
			child->edgePos[blue_yellow] =  parent->edgePos[blue_pink];
			child->edgeDir[blue_yellow] = !parent->edgeDir[blue_pink];
			child->edgePos[blue_pink] =    parent->edgePos[blue_white];
			child->edgeDir[blue_pink] =   !parent->edgeDir[blue_white];
			child->edgePos[blue_white] =   parent->edgePos[blue_red];
			child->edgeDir[blue_white] =  !parent->edgeDir[blue_red];
			child->edgePos[blue_red] =     parent->edgePos[blue_yellow];
			child->edgeDir[blue_red] =    !parent->edgeDir[blue_yellow];
			break;
		case BLUE_OUT: /*blue out*/
			child->edgePos[blue_yellow] =  parent->edgePos[blue_red];
			child->edgeDir[blue_yellow] = !parent->edgeDir[blue_red];
			child->edgePos[blue_red] =     parent->edgePos[blue_white];
			child->edgeDir[blue_red] =    !parent->edgeDir[blue_white];
			child->edgePos[blue_white] =   parent->edgePos[blue_pink];
			child->edgeDir[blue_white] =  !parent->edgeDir[blue_pink];
			child->edgePos[blue_pink] =    parent->edgePos[blue_yellow];
			child->edgeDir[blue_pink] =   !parent->edgeDir[blue_yellow];
			break;
		case RED_IN: /*red in*/
			child->edgePos[red_white] =   parent->edgePos[red_green];
			child->edgeDir[red_white] =  !parent->edgeDir[red_green];
			child->edgePos[red_green] =   parent->edgePos[red_yellow];
			child->edgeDir[red_green] =  !parent->edgeDir[red_yellow];
			child->edgePos[red_yellow] =  parent->edgePos[red_blue];
			child->edgeDir[red_yellow] = !parent->edgeDir[red_blue];
			child->edgePos[red_blue] =    parent->edgePos[red_white];
			child->edgeDir[red_blue] =   !parent->edgeDir[red_white];
			break;
		case RED_OUT: /*red out*/
			child->edgePos[red_white] =   parent->edgePos[red_blue];
			child->edgeDir[red_white] =  !parent->edgeDir[red_blue];
			child->edgePos[red_blue] =    parent->edgePos[red_yellow];
			child->edgeDir[red_blue] =   !parent->edgeDir[red_yellow];
			child->edgePos[red_yellow] =  parent->edgePos[red_green];
			child->edgeDir[red_yellow] = !parent->edgeDir[red_green];
			child->edgePos[red_green] =   parent->edgePos[red_white];
			child->edgeDir[red_green] =  !parent->edgeDir[red_white];
			break;
		case PINK_IN: /*pink in*/
			child->edgePos[pink_white] =   parent->edgePos[pink_blue];
			child->edgeDir[pink_white] =  !parent->edgeDir[pink_blue];
			child->edgePos[pink_blue] =    parent->edgePos[pink_yellow];
			child->edgeDir[pink_blue] =   !parent->edgeDir[pink_yellow];
			child->edgePos[pink_yellow] =  parent->edgePos[pink_green];
			child->edgeDir[pink_yellow] = !parent->edgeDir[pink_green];
			child->edgePos[pink_green] =   parent->edgePos[pink_white];
			child->edgeDir[pink_green] =  !parent->edgeDir[pink_white];
			break;
		case PINK_OUT: /*pink out*/
			child->edgePos[pink_white] =   parent->edgePos[pink_green];
			child->edgeDir[pink_white] =  !parent->edgeDir[pink_green];
			child->edgePos[pink_green] =   parent->edgePos[pink_yellow];
			child->edgeDir[pink_green] =  !parent->edgeDir[pink_yellow];
			child->edgePos[pink_yellow] =  parent->edgePos[pink_blue];
			child->edgeDir[pink_yellow] = !parent->edgeDir[pink_blue];
			child->edgePos[pink_blue] =    parent->edgePos[pink_white];
			child->edgeDir[pink_blue] =   !parent->edgeDir[pink_white];
			break;
		case WHITE_IN: /*white in*/
			child->edgePos[white_blue] =   parent->edgePos[white_pink];
			child->edgeDir[white_blue] =  !parent->edgeDir[white_pink];
			child->edgePos[white_pink] =   parent->edgePos[white_green];
			child->edgeDir[white_pink] =  !parent->edgeDir[white_green];
			child->edgePos[white_green] =  parent->edgePos[white_red];
			child->edgeDir[white_green] = !parent->edgeDir[white_red];
			child->edgePos[white_red] =    parent->edgePos[white_blue];
			child->edgeDir[white_red] =   !parent->edgeDir[white_blue];
			break;
		case WHITE_OUT: /*white out*/
			child->edgePos[white_blue] =   parent->edgePos[white_red];
			child->edgeDir[white_blue] =  !parent->edgeDir[white_red];
			child->edgePos[white_red] =    parent->edgePos[white_green];
			child->edgeDir[white_red] =   !parent->edgeDir[white_green];
			child->edgePos[white_green] =  parent->edgePos[white_pink];
			child->edgeDir[white_green] = !parent->edgeDir[white_pink];
			child->edgePos[white_pink] =   parent->edgePos[white_blue];
			child->edgeDir[white_pink] =  !parent->edgeDir[white_blue];
			break;
		case YELLOW_IN: /*yellow in*/
			child->edgePos[yellow_green] =  parent->edgePos[yellow_pink];
			child->edgeDir[yellow_green] = !parent->edgeDir[yellow_pink];
			child->edgePos[yellow_pink] =   parent->edgePos[yellow_blue];
			child->edgeDir[yellow_pink] =  !parent->edgeDir[yellow_blue];
			child->edgePos[yellow_blue] =   parent->edgePos[yellow_red];
			child->edgeDir[yellow_blue] =  !parent->edgeDir[yellow_red];
			child->edgePos[yellow_red] =    parent->edgePos[yellow_green];
			child->edgeDir[yellow_red] =   !parent->edgeDir[yellow_green];
			break;
		case YELLOW_OUT: /*yellow out*/
			child->edgePos[yellow_green] =  parent->edgePos[yellow_red];
			child->edgeDir[yellow_green] = !parent->edgeDir[yellow_red];
			child->edgePos[yellow_red] =    parent->edgePos[yellow_blue];
			child->edgeDir[yellow_red] =   !parent->edgeDir[yellow_blue];
			child->edgePos[yellow_blue] =   parent->edgePos[yellow_pink];
			child->edgeDir[yellow_blue] =  !parent->edgeDir[yellow_pink];
			child->edgePos[yellow_pink] =   parent->edgePos[yellow_green];
			child->edgeDir[yellow_pink] =  !parent->edgeDir[yellow_green];
			break;
		default:
			printf("error\n");
			break;
	}
}


