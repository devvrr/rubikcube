#include "../rbdefs.h"
#include <stdlib.h>
#include <stdio.h>
/*There are 6 faces to the cube. Let's identify each face with the color of the cublet in
 *  the center.
 *  0 - red
 *  1 - pink
 *  2 - green
 *  3 - blue
 *  4 - white
 *  5 - yellow
 *
 *
 * Each of the six diagrams below shows the color of the face and the colors of the faces
 * on the left, right, top and bottom of the face. This will be the pattern in which the 
 * colors of a given cube configuration will be read into the userDesc data structure.
 *
 *
 *        0          1           2
 *0                green
 *                   |   
 *1    yellow  <-   red  ->    white
 *                   |
 *2                blue
 *
 *
 *        0          1           2
 *0                blue
 *                   |   
 *1    yellow  <-  pink  ->    white
 *                   |
 *2                green
 *
 * 
 *        0          1           2
 *0                pink
 *                   |   
 *1    yellow  <-  green ->    white
 *                   |
 *2                 red
 *
 * 
 *         0          1           2
 *0                 red
 *                   |   
 *1    yellow  <-  blue  ->    white
 *                   |
 *2                pink
 *
 *
 * 
 *        0          1           2
 *0                blue
 *                   |   
 *1    pink    <-  white ->     red
 *                   |
 *2                green
 *
 *
 *        0          1           2
 *0                 blue
 *                   |   
 *1    red    <-  yellow ->    pink
 *                   |
 *2                green
 *
 *
 *
 * An example input
 * b p r
 * p g w
 * b w b
 * g r r
 * g b g
 * y p g
 * y g y
 * y r b
 * r b w
 * p w w
 * b p y
 * p y y
 * p p b
 * r w y
 * g r p
 * w r g
 * g y w
 * r b w
 *
 * The representation for the Goal node would be
 * r r r
 * r r r
 * r r r
 * p p p
 * p p p
 * p p p
 * g g g
 * g g g
 * g g g
 * b b b
 * b b b
 * b b b
 * w w w
 * w w w
 * w w w
 * y y y
 * y y y
 * y y y
 * 
 *  */

static CORNERPOS GetCornerPosFromColors( rbCubeColours c1, rbCubeColours c2, rbCubeColours c3 );
static EDGEPOS GetEdgePosFromColors( rbCubeColours c1, rbCubeColours c2 );
/*direction value of an edge position is zero if it has edge cubelet "i" and
 * colour "edgeDirMap[i]" on the color-side mentioned below(in edgeDirXyz comments)  */
static UINT8 edgeDirMap[12] = {red, red, pink, pink, blue, green,
       	green, blue, yellow, white, white, yellow};

/*coordinates for .?. in userDesc[][][]*/
static UINT8 edgeDirXyz[12][3] = {
   {0,0,1},/*0. red-side of red-green*/
   {0,2,1},/*1. red-side of red-blue*/
   {1,2,1},/*2. pink-side of green-pink*/
   {1,0,1},/*3. pink-side of pink-blue*/
   {3,1,0},/*4. blue-side of yellow-blue*/
   {2,1,0},/*5. green-side of green-yellow*/
   {2,1,2},/*6. green-side of green-white*/
   {3,1,2},/*7. blue-side of white-blue*/
   {5,1,2},/*8. yellow-side of pink-yellow*/
   {4,1,0},/*9. white-side of pink-white*/
   {4,1,2},/*10. white-side of white-red*/
   {5,1,0}};/*11. yellow-side of yellow-red*/
static UINT8 cornerDirMap[2][8][3] = 
                      {
                         {
                            {green, pink, yellow},/*0*/
                            {green, red, white},/*1*/
                            {blue, pink, white},/*2*/
                            {blue, red, yellow},/*3*/
                            {white, red, blue},/*4*/
                            {yellow, red, green},/*5*/
                            {white, pink, green},/*6*/
                            {yellow, pink, blue}/*7*/
			 },
                         {
			    {yellow, pink, green},/*0*/
			    {white, red, green},/*1*/
			    {white, pink, blue},/*2*/
			    {yellow, red, blue},/*3*/
			    {blue, red, white},/*4*/
			    {green, red, yellow},/*5*/
			    {green, pink, white},/*6*/
			    {blue, pink, yellow}/*7*/
			 }
                      };
static UINT8 cornerDirXyz[8][3] = {
   {2,0,0},/*green-pink-yellow green-side*/
   {2,2,2},/*red-green-white green-side*/
   {3,2,2},/*blue-pink-white blue-side*/
   {3,0,0},/*red-yellow-blue blue-side*/
   {3,0,2},/*red-white-blue blue-side*/
   {2,2,0},/*red-green-yellow green-side*/
   {2,0,2},/*white-green-pink green-side*/
   {3,2,0}};/*yellow-blue-pink blue-side*/

static void GetConfigPositions( UserCubeDesc userDesc, rbCubeState * state )
{
   state->edgePos[red_green] = GetEdgePosFromColors( userDesc[0][0][1], userDesc[2][2][1] );

   printf("red-blue %d %d \n",userDesc[0][2][1],userDesc[3][0][1]);
   state->edgePos[red_blue] = GetEdgePosFromColors( userDesc[0][2][1], userDesc[3][0][1] );
   state->edgePos[green_pink] = GetEdgePosFromColors( userDesc[2][0][1], userDesc[1][2][1] );
   state->edgePos[pink_blue] = GetEdgePosFromColors( userDesc[3][2][1], userDesc[1][0][1] );
   
   state->edgePos[yellow_blue] = GetEdgePosFromColors( userDesc[5][0][1], userDesc[3][1][0] );
   state->edgePos[green_yellow] = GetEdgePosFromColors( userDesc[2][1][0], userDesc[5][2][1] );
   state->edgePos[green_white] = GetEdgePosFromColors( userDesc[2][1][2], userDesc[4][2][1] );
   state->edgePos[white_blue] = GetEdgePosFromColors( userDesc[4][0][1], userDesc[3][1][2] );

   state->edgePos[pink_yellow] = GetEdgePosFromColors( userDesc[1][1][0], userDesc[5][1][2] );
   state->edgePos[pink_white] = GetEdgePosFromColors( userDesc[1][1][2], userDesc[4][1][0] );
   state->edgePos[white_red] = GetEdgePosFromColors( userDesc[4][1][2], userDesc[0][1][2] );
   state->edgePos[red_yellow] = GetEdgePosFromColors( userDesc[0][1][0], userDesc[5][1][0] );

   state->cornerPos[green_pink_yellow] = GetCornerPosFromColors( 
	 /*green-side*/ userDesc[2][0][0],
	 /*pink-side*/  userDesc[1][2][0],
	 /*yellow-side*/userDesc[5][2][2]);
   state->cornerPos[red_green_white] = GetCornerPosFromColors( 
	 /*red-side*/  userDesc[0][0][2],
	 /*green-side*/userDesc[2][2][2],
	 /*white-side*/userDesc[4][2][2]);
   state->cornerPos[blue_pink_white] = GetCornerPosFromColors(
	 /*blue-side*/ userDesc[3][2][2],
         /*pink-side*/ userDesc[1][0][2],
	 /*white-side*/userDesc[4][0][0] );
   state->cornerPos[red_yellow_blue] = GetCornerPosFromColors(
	/*red-side*/   userDesc[0][2][0],
        /*yellow-side*/userDesc[5][0][0],
        /*blue-side*/  userDesc[3][0][0]);

   state->cornerPos[red_white_blue] = GetCornerPosFromColors(
	/*red-side*/  userDesc[0][2][2],
        /*white-side*/userDesc[4][0][2],
        /*blue-side*/ userDesc[3][0][2]);
   state->cornerPos[red_green_yellow] = GetCornerPosFromColors(
	/*red-side*/   userDesc[0][0][0],
        /*green-side*/ userDesc[2][2][0],
        /*yellow-side*/userDesc[5][2][0]);
   state->cornerPos[white_green_pink] = GetCornerPosFromColors(
	/*white-side*/ userDesc[4][2][0],
        /*green-side*/ userDesc[2][0][2],
        /*pink-side*/  userDesc[1][2][2]);
   state->cornerPos[yellow_blue_pink] = GetCornerPosFromColors(
	/*yellow-side*/userDesc[5][0][2],
        /*blue-side*/  userDesc[3][2][0],
        /*pink-side*/  userDesc[1][0][0]);

}
static void GetEdgeDir(UserCubeDesc userDesc, rbCubeState * state)
{
   int i;
   for( i=0; i<12; i++ )
   {
      if( edgeDirMap[state->edgePos[i]] ==
	   userDesc[edgeDirXyz[i][0]][edgeDirXyz[i][1]][edgeDirXyz[i][2]] )
	 state->edgeDir[i] = 0;
      else
	 state->edgeDir[i] = 1;

   }
}
static void GetCornerDir(UserCubeDesc userDesc, rbCubeState * state)
{
   int i,j;
   for( i=0; i<8; i++ )
   {
      for(j=0;j<3;j++)
      {
         if( cornerDirMap[i/4][state->cornerPos[i]][j] ==
	    userDesc[cornerDirXyz[i][0]][cornerDirXyz[i][1]][cornerDirXyz[i][2]] )
	    state->cornerDir[i] = j;
      }

   }
}
static Boolean IsColorInThree( rbCubeColours c1, rbCubeColours c2, rbCubeColours c3, rbCubeColours ref )
{
   if( ref == c1 || ref == c2 || ref == c3 )
      return TRUE;
   return FALSE;
}
static Boolean ColorsThreeInThree( rbCubeColours c1, rbCubeColours c2, rbCubeColours c3,
       rbCubeColours ref1, rbCubeColours ref2, rbCubeColours ref3 )
{
   if( IsColorInThree( ref1, ref2, ref3, c1) &&
       IsColorInThree( ref1, ref2, ref3, c2) &&
       IsColorInThree( ref1, ref2, ref3, c3) )
      return TRUE;
   return FALSE;
}
static CORNERPOS GetCornerPosFromColors( rbCubeColours c1, rbCubeColours c2, rbCubeColours c3 )
{
   if( ColorsThreeInThree( green, pink, yellow, c1, c2, c3) )
      return green_pink_yellow;

   if( ColorsThreeInThree( red, green, white, c1, c2, c3) )
      return red_green_white;

   if( ColorsThreeInThree( blue, pink, white, c1, c2, c3) )
      return blue_pink_white;

   if( ColorsThreeInThree( red, yellow, blue, c1, c2, c3) )
      return red_yellow_blue;

   if( ColorsThreeInThree( red, white, blue, c1, c2, c3) )
      return red_white_blue;
   
   if( ColorsThreeInThree( red, green, yellow, c1, c2, c3) )
      return red_green_yellow;

   if( ColorsThreeInThree( white, green, pink, c1, c2, c3) )
      return white_green_pink;

   if( ColorsThreeInThree( yellow, blue, pink, c1, c2, c3) )
      return yellow_blue_pink;

   printf("error in corner cubelet colours; cubelet with this combination of colors doesn't exist.\n");
   return yellow_blue_pink;/*The value returned here is arbitrary*/
}
static EDGEPOS GreenEdgePosGet( rbCubeColours c2 )
{
   switch( c2 )
   {
      case red:
	 return green_red;
      case pink:
	 return green_pink;
      case yellow:
	 return green_yellow;
      case white:
	 return green_white;
      default:
	 printf("bad edge cube colours\n");
	 return green_white;//arbitrary
	 break;
   }
}
static EDGEPOS BlueEdgePosGet( rbCubeColours c2 )
{
   switch( c2 )
   {
      case red:
	 return blue_red;
      case pink:
	 return blue_pink;
      case yellow:
	 return blue_yellow;
      case white:
	 return blue_white;
      default:
	 printf("bad edge cube colours\n");
	 return blue_white;//arbitrary
	 break;
   }
}
static EDGEPOS GetEdgePosFromColors( rbCubeColours c1, rbCubeColours c2 )
{
   if( c1 == green )
      return GreenEdgePosGet( c2 );
   if( c2 == green )
      return GreenEdgePosGet( c1 );

   if( c1 == blue )
      return BlueEdgePosGet( c2 );
   if( c2 == blue )
      return BlueEdgePosGet( c1 );

   if( c1 == pink )
   {
      if( c2 == yellow )
	 return pink_yellow;
      if( c2 == white )
	 return pink_white;
      printf("wrong edge cube pos colours\n");
      return invalid_edgepos;
   }
   if( c2 == pink )
   {
      if( c1 == yellow )
	 return pink_yellow;
      if( c1 == white )
	 return pink_white;
      printf("wrong edge cube pos colours\n");
      return invalid_edgepos;
   }
   if( c1 == red )
   {
      if( c2 == yellow )
	 return red_yellow;
      if( c2 == white )
	 return red_white;
      printf("wrong edge cube pos colours\n");
      return invalid_edgepos;
   }
   if( c2 == red )
   {
      if( c1 == yellow )
	 return red_yellow;
      if( c1 == white )
	 return red_white;
      printf("wrong edge cube pos colours\n");
      return invalid_edgepos;
   }
	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
	return invalid_edgepos;
}

static UINT8 colourToInt( char colour )
{
  // printf("colour = %d, %c\n", colour, colour);
   switch(colour)
   {
      case 'r': return 0;
      case 'p': return 1;
      case 'g': return 2;
      case 'b': return 3;
      case 'w': return 4;
      case 'y': return 5;
      default: 
		printf("wrong colour =%d\n",colour);
		return 6;/*arbitrary*/
		
   }
} 
static char colourStr[6][7] = {"red","pink","green","blue","white","yellow"};
void ReadUserCubeDesc(UserCubeDesc cube)
{
   UINT8 i,j,k;
   char colour;

   for(i=0;i<6;i++)
   {
      printf("please enter colours(first letter) for face %s \n",colourStr[i]);
      for(j=0;j<3;j++)
	 for(k=0;k<3;k++)
     {
         scanf("%c",&colour);
         if ( colour <'a' || colour>'z' )
             scanf("%c",&colour);
         printf("%c\n",colour);
         cube[i][j][k] = colourToInt(colour);
         printf("colourToInt=%d\n",cube[i][j][k]);
         scanf("%c",&colour);//read and throw away the space character
     }
   }
}
void PrintConfig(rbCubeState * state)
{
   UINT8 i,j,k;
   printf("\n  edge pos:");
   for(i=0;i<12;i++)
   {
      printf("%3d",state->edgePos[i]);
   }
   printf("\n  edge dir:");
   for(i=0;i<12;i++)
   {
      printf("%3d",state->edgeDir[i]);
   }
   printf("\ncorner pos:");
   for(i=0;i<8;i++)
   {
      printf("%3d",state->cornerPos[i]);
   }
   printf("\ncorner dir:");
   for(i=0;i<8;i++)
   {
      printf("%3d",state->cornerDir[i]);
   }
   printf("\n");
      
}
#if 1 
void GetUserInputConfig(rbCubeState * state)
{
   UserCubeDesc d;

   ReadUserCubeDesc(d);
   GetConfigPositions( d, state );
   GetEdgeDir( d, state );
   GetCornerDir( d, state );
   PrintConfig(state);
}
#endif
#if 0 
int main( void )
{
   UserCubeDesc d;
   rbCubeState state;

   ReadUserCubeDesc(d);
   GetConfigPositions( d, &state );
   GetEdgeDir( d, &state );
   GetCornerDir( d, &state );
   PrintConfig(&state);
}
#endif


