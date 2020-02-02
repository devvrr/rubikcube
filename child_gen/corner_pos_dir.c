#include "../rbdefs.h"

#if 0
const static UINT8 MoveAffectsCornerPos[NCOLORS][NCORNERCUBELETS]=
{           /*0,1,2,3,4,5,6,7*/
   /*red*/   {0,1,0,1,1,1,0,0},
   /*pink*/  {1,0,1,0,0,0,1,1},
   /*green*/ {1,1,0,0,0,1,1,0},
   /*blue*/  {0,0,1,1,1,0,0,1},
   /*white*/ {0,1,1,0,1,0,1,0},
   /*yellow*/{1,0,0,1,0,1,0,1}
};

const static UINT8 CornerPosMoveMapping[NMOVES][NCORNERCUBELETS]=
{
   /*red-in*/     {0,4,2,5,3,1,6,7},
   /*red-out*/    {0,5,2,4,1,3,6,7},
   /*pink-in*/    {7,1,6,3,4,5,0,2},
   /*pink-out*/   {6,1,7,3,4,5,2,0},

   /*green-in*/   {6,5,2,3,4,0,1,7},
   /*green-out*/  {5,6,2,3,4,1,0,7},
   /*blue-in*/    {0,1,7,4,2,5,6,3},
   /*blue-out*/   {0,1,4,7,3,5,6,2},

   /*white-in*/   {0,6,4,3,1,5,2,7},
   /*white-out*/  {0,4,6,3,2,5,1,7},
   /*yellow-in*/  {5,1,2,7,4,3,6,0},
   /*yellow-out*/ {7,1,2,5,4,0,6,3}
};
#endif

const static unsigned char corner_cube_dir_mapping[3][3] =
{/*[layer_pair_index][previous_direction_value]*/
   { 0, 2, 1}, /*pink - red*/
   { 2, 1, 0}, /*green - blue*/
   { 1, 0, 2}, /*yellow - white*/
};


/*Finds the child configuration in the corner cubelet position value graph for a
given parent and move number
and puts it into the given config*/
void Find_child_corner_pos( rbCubeState * parent, rbCubeState * child, UINT8 move )
{
	UINT8 i;

	for( i=0;i<8;i++ ){
		child->cornerPos[i] = parent->cornerPos[i];
		child->cornerDir[i] = parent->cornerDir[i];
	}

	switch( move )
	{
		case GREEN_IN: /*green in*/
			child->cornerPos[green_white_red] = parent->cornerPos[green_white_pink];
			child->cornerDir[green_white_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_white_pink]];
			child->cornerPos[green_white_pink] = parent->cornerPos[green_pink_yellow];
			child->cornerDir[green_white_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_pink_yellow]];
			child->cornerPos[green_pink_yellow] = parent->cornerPos[green_yellow_red];
			child->cornerDir[green_pink_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_yellow_red]];
			child->cornerPos[green_yellow_red] = parent->cornerPos[green_white_red];
			child->cornerDir[green_yellow_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_white_red]];
			break;
		case GREEN_OUT: /*green out*/
			child->cornerPos[green_white_red] = parent->cornerPos[green_yellow_red];
			child->cornerDir[green_white_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_yellow_red]];
			child->cornerPos[green_yellow_red] = parent->cornerPos[green_pink_yellow];
			child->cornerDir[green_yellow_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_pink_yellow]];
			child->cornerPos[green_pink_yellow] = parent->cornerPos[green_white_pink];
			child->cornerDir[green_pink_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_white_pink]];
			child->cornerPos[green_white_pink] = parent->cornerPos[green_white_red];
			child->cornerDir[green_white_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[green_white_red]];
			break;
		case BLUE_IN: /*blue in*/
			child->cornerPos[blue_red_yellow] = parent->cornerPos[blue_yellow_pink];
			child->cornerDir[blue_red_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_yellow_pink]];
			child->cornerPos[blue_yellow_pink] = parent->cornerPos[blue_pink_white];
			child->cornerDir[blue_yellow_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_pink_white]];
			child->cornerPos[blue_pink_white] = parent->cornerPos[blue_white_red];
			child->cornerDir[blue_pink_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_white_red]];
			child->cornerPos[blue_white_red] = parent->cornerPos[blue_red_yellow];
			child->cornerDir[blue_white_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_red_yellow]];
			break;
		case BLUE_OUT: /*blue out*/
			child->cornerPos[blue_red_yellow] = parent->cornerPos[blue_white_red];
			child->cornerDir[blue_red_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_white_red]];
			child->cornerPos[blue_white_red] = parent->cornerPos[blue_pink_white];
			child->cornerDir[blue_white_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_pink_white]];
			child->cornerPos[blue_pink_white] = parent->cornerPos[blue_yellow_pink];
			child->cornerDir[blue_pink_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_yellow_pink]];
			child->cornerPos[blue_yellow_pink] = parent->cornerPos[blue_red_yellow];
			child->cornerDir[blue_yellow_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[blue_red_yellow]];
			break;
		case RED_IN: /*red in*/
			child->cornerPos[red_green_white] = parent->cornerPos[red_green_yellow];
			child->cornerDir[red_green_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_green_yellow]];
			child->cornerPos[red_green_yellow] = parent->cornerPos[red_yellow_blue];
			child->cornerDir[red_green_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_yellow_blue]];
			child->cornerPos[red_yellow_blue] = parent->cornerPos[red_blue_white];
			child->cornerDir[red_yellow_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_blue_white]];
			child->cornerPos[red_blue_white] = parent->cornerPos[red_green_white];
			child->cornerDir[red_blue_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_green_white]];
			break;
		case RED_OUT: /*red out*/
			child->cornerPos[red_green_white] = parent->cornerPos[red_blue_white];
			child->cornerDir[red_green_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_blue_white]];
			child->cornerPos[red_blue_white] = parent->cornerPos[red_yellow_blue];
			child->cornerDir[red_blue_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_yellow_blue]];
			child->cornerPos[red_yellow_blue] = parent->cornerPos[red_green_yellow];
			child->cornerDir[red_yellow_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_green_yellow]];
			child->cornerPos[red_green_yellow] = parent->cornerPos[red_green_white];
			child->cornerDir[red_green_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[red_green_white]];
			break;
		case PINK_IN: /*pink in*/
			child->cornerPos[pink_green_white] = parent->cornerPos[pink_white_blue];
			child->cornerDir[pink_green_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_white_blue]];
			child->cornerPos[pink_white_blue] = parent->cornerPos[pink_blue_yellow];
			child->cornerDir[pink_white_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_blue_yellow]];
			child->cornerPos[pink_blue_yellow] = parent->cornerPos[pink_yellow_green];
			child->cornerDir[pink_blue_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_yellow_green]];
			child->cornerPos[pink_yellow_green] = parent->cornerPos[pink_green_white];
			child->cornerDir[pink_yellow_green] =corner_cube_dir_mapping[move/4][parent->cornerDir[pink_green_white]];
			break;
		case PINK_OUT: /*pink out*/
			child->cornerPos[pink_green_white] = parent->cornerPos[pink_yellow_green];
			child->cornerDir[pink_green_white] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_yellow_green]];
			child->cornerPos[pink_yellow_green] = parent->cornerPos[pink_blue_yellow];
			child->cornerDir[pink_yellow_green] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_blue_yellow]];
			child->cornerPos[pink_blue_yellow] = parent->cornerPos[pink_white_blue];
			child->cornerDir[pink_blue_yellow] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_white_blue]];
			child->cornerPos[pink_white_blue] = parent->cornerPos[pink_green_white];
			child->cornerDir[pink_white_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[pink_green_white]];
			break;
		case WHITE_IN: /*white in*/
			child->cornerPos[white_blue_red] = parent->cornerPos[white_blue_pink];
			child->cornerDir[white_blue_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_blue_pink]];
			child->cornerPos[white_blue_pink] = parent->cornerPos[white_pink_green];
			child->cornerDir[white_blue_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_pink_green]];
			child->cornerPos[white_pink_green] = parent->cornerPos[white_green_red];
			child->cornerDir[white_pink_green] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_green_red]];
			child->cornerPos[white_green_red] = parent->cornerPos[white_blue_red];
			child->cornerDir[white_green_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_blue_red]];
			break;
		case WHITE_OUT: /*white out*/
			child->cornerPos[white_blue_red] = parent->cornerPos[white_green_red];
			child->cornerDir[white_blue_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_green_red]];
			child->cornerPos[white_green_red] = parent->cornerPos[white_pink_green];
			child->cornerDir[white_green_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_pink_green]];
			child->cornerPos[white_pink_green] = parent->cornerPos[white_blue_pink];
			child->cornerDir[white_pink_green] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_blue_pink]];
			child->cornerPos[white_blue_pink] = parent->cornerPos[white_blue_red];
			child->cornerDir[white_blue_pink] = corner_cube_dir_mapping[move/4][parent->cornerDir[white_blue_red]];
			break;
		case YELLOW_IN: /*yellow in*/
			child->cornerPos[yellow_pink_green] = parent->cornerPos[yellow_pink_blue];
			child->cornerDir[yellow_pink_green] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_pink_blue]];
			child->cornerPos[yellow_pink_blue] = parent->cornerPos[yellow_blue_red];
			child->cornerDir[yellow_pink_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_blue_red]];
			child->cornerPos[yellow_blue_red] = parent->cornerPos[yellow_green_red];
			child->cornerDir[yellow_blue_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_green_red]];
			child->cornerPos[yellow_green_red] = parent->cornerPos[yellow_pink_green];
			child->cornerDir[yellow_green_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_pink_green]];
			break;
		case YELLOW_OUT: /*yellow out*/
			child->cornerPos[yellow_pink_green] = parent->cornerPos[yellow_green_red];
			child->cornerDir[yellow_pink_green] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_green_red]];
			child->cornerPos[yellow_green_red] = parent->cornerPos[yellow_blue_red];
			child->cornerDir[yellow_green_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_blue_red]];
			child->cornerPos[yellow_blue_red] = parent->cornerPos[yellow_pink_blue];
			child->cornerDir[yellow_blue_red] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_pink_blue]];
			child->cornerPos[yellow_pink_blue] = parent->cornerPos[yellow_pink_green];
			child->cornerDir[yellow_pink_blue] = corner_cube_dir_mapping[move/4][parent->cornerDir[yellow_pink_green]];
			break;
		default:
			printf("error\n");
			break;
	}

	//PrintArray(child->cornerPos,8);
	//printf("cornerPos!! = %d\n",GetIndexFromPerm(child->cornerPos,8));
}




