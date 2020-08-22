#include "../rbdefs.h"
#include "../rbmath.h"
#include "edge_pos.h"
#include "edge_dir.h"
#include "corner_pos.h"
#include "corner_dir.h"
#include "../diff/diff.h"
char randomMove( void );


void GetD5Index( rbCubeState * state, UINT8 id[5]);
struct Tripod8_tag
{
    UINT8 corners[8];
    UINT8 edges[12];
    UINT8 edgePosSer[12];
	UINT8 nValidEdge;
	UINT8 nValidCorner;
} Tripod[8] =
{
	{
	   	/*0*/{1,0,0,0,0,1,1,1},
	        /*0 1 2 3 4 5 6 7 8 9 10 11*/
		/*0*/{1,0,1,1,1,1,1,0,1,1, 0, 1},
		/*0*/{0,0,1,2,3,4,5,0,6,7, 0, 8},
		9,4
	},
	{
		/*1*/{0,1,0,0,1,1,1,0},
		/*1*/{1,1,1,0,0,1,1,1,0,1, 1, 1},
		/*1*/{0,1,2,0,0,3,4,5,0,6, 7, 8},
		9,4
	},
	{
		/*2*/{0,0,1,0,1,0,1,1},
		/*2*/{0,1,1,1,1,0,1,1,1,1, 1, 0},
		/*1*/{0,0,1,2,3,0,4,5,6,7, 8, 0},
		9,4
	},
	{
		/*3*/{0,0,0,1,1,1,0,1},
		/*3*/{1,1,0,1,1,1,0,1,1,0, 1, 1},
		/*1*/{0,1,0,2,3,4,0,5,6,0, 7, 8},
		9,4
	},
	{
		/*4*/{0,1,1,1,1,0,0,0},
		/*4*/{1,1,0,1,1,0,1,1,0,1, 1, 1},
		/*1*/{0,1,0,2,3,0,4,5,0,6, 7, 8},
		9,4
	},
	{
		/*5*/{1,1,0,1,0,1,0,0},
		/*5*/{1,1,1,0,1,1,1,0,1,0, 1, 1},
		/*5*/{0,1,2,0,3,4,5,0,6,0, 7, 8},
		9,4
	},
	{
		/*6*/{1,1,1,0,0,0,1,0},
		/*6*/{1,0,1,1,0,1,1,1,1,1, 1, 0},
		/*6*/{0,0,1,2,0,3,4,5,6,7, 8, 0},
		9,4
	},
	{
		/*7*/{1,0,1,1,0,0,0,1},
		/*7*/{0,1,1,1,1,1,0,1,1,1, 0, 1},
		/*7*/{0,0,1,2,3,4,0,5,6,7, 0, 8},
		9,4
	}

};
#define NULL_STATE {{0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}
#define GOAL_STATE1 {{0,1,2,3,4,5,6,7,8,9,10,11},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,1,2,3,4,5,6,7},{1,0,0,0,0,0,0,0}}
#define GOAL_STATE2 {{0,1,2,3,4,5,6,7,8,9,10,11},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,1,2,3,4,5,6,7},{2,0,0,0,0,0,0,0}}
/*
tripod

yellow-green
yellow-pink
yellow-blue
yellow-red
red-green
red-blue
pink-blue

blue-yellow-pink
blue-yellow-red
yellow-red-green
*/

/*
tripod

yellow-green - 5
yellow-pink - 8
yellow-blue - 4
yellow-red - 11
red-green - 0
red-blue - 1
pink-blue - 3
white-red - 10
white-blue - 7

white-red-blue - 4
blue-yellow-pink - 7
blue-yellow-red - 3
yellow-red-green - 5
*/


static void Find_child_edge( rbCubeState * parent, rbCubeState * child, UINT8 move )
{
   //UINT8 i;
   Find_child_edge_pos(parent,child,move);
   //Find_child_edge_dir(parent,child,move);
   
  // printf("move = %d\nedgeDir = ",move);
  // for(i=0;i<12;i++)
  //    printf("%u ",child->edgeDir[i]);
 //  printf("\n");
}
static void Find_child_corner( rbCubeState * parent, rbCubeState * child, UINT8 move )
{
   //UINT8 i;
   Find_child_corner_pos(parent,child,move);
   //Find_child_corner_dir(parent,child,move);
 //  printf("move = %d\ncornerDir = ",move);
  // for(i=0;i<8;i++)
     // printf("%u ",child->cornerDir[i]);
 //  printf("\n");
}
void PrintFull( rbCubeState * s )
{
   UINT8 i;
   printf("state\n");
   printf("edgePos =  ");
   for(i=0;i<12;i++)
      printf("%3u",s->edgePos[i]);
   printf("\ncornerPos =");
   for(i=0;i<8;i++)
      printf("%3u",s->cornerPos[i]);
   printf("\n");
   //getchar();
}
void PrintFullN( rbCubeState * s, char * StateName )
{
   printf("\nstate:%s =>\n",StateName);
   PrintFull(s);
}
void BBSort(UINT8 A[], int from, int to)
{
	int i,j,tmp;
	for(j=1;j<to-from;j++)
		for(i=from;i<to-j;i++)
			if(A[i]>A[i+1])
			{
				tmp = A[i];
				A[i] = A[i+1];
				A[i+1] = tmp;
			}
}
void GetChild( rbCubeState * parent, rbCubeState * child, UINT8 move )
{
/*
 * corner cube directions
 * edge cube directions
 * edge cube positions
 * corner cube positions
 * */
   Find_child_edge(parent,child,move);
   Find_child_corner(parent,child,move);
  // getchar();
}
void PrintMove(int id)
{
	switch(id)
	{
		case  RED_IN:printf("red-in ");break;
		case  RED_OUT:printf("red-out ");break;
		case  PINK_IN:printf("pink-in ");break;
		case  PINK_OUT:printf("pink-out ");break;
		case  GREEN_IN:printf("green-in ");break;
		case  GREEN_OUT:printf("green-out ");break;
		case  BLUE_IN:printf("blue-in ");break;
		case  BLUE_OUT:printf("blue-out ");break;
		case  WHITE_IN:printf("white-in ");break;
		case  WHITE_OUT:printf("white-out ");break;
		case YELLOW_IN:printf("yellow-in ");break;
		case YELLOW_OUT:printf("yellow-out ");break;
		default:printf("invalid move %3d",id);break;
	}
	printf("-");
}
/*
 * "dist": the number of moves to make from the Goal Node to obtain the random state
 d1 : number of double turns in the reduced graph*/
void GetRandRbRedState(rbCubeState * state, UINT8 dist,int d1 )
{
    int i;
    UINT8 move=14,prevMove;
    rbCubeState parent = GOAL_STATE;
    rbCubeState child;

    if( !state )
    {
        printf("null pointer!!\n");
        exit(0);
    }
    //printf("initial parent\n");
    //PrintFull(&parent);
    for(i=0;i<dist;)
    {
        prevMove = move;
        move = randomMove();
        /*check if we are reversing what is done in the previous move*/ 
        if(move == (prevMove ^0x01))
        {
            move = prevMove;
            continue;
        }
        i++;
        PrintMove(move);
        GetChild(&parent,&child,move);
        parent = child;
    }
    printf("\n");
    for(i=0;i<d1;i++)
    {
        PrintFull(&parent);
        prevMove = move;
        move = randomMove();
        while(move == prevMove || move == (prevMove ^0x01))
        {
            prevMove = move;
            move = randomMove();
        }
        PrintMove(move);
        GetChild(&parent,&child,move);
        parent = child;
        PrintMove(move);
        GetChild(&parent,&child,move);
        parent = child;
        PrintFull(&parent);
    }
    printf("\n");
    for(i=0;i<dist;)
    {
        prevMove = move;
        move = randomMove();
        /*check if we are reversing what is done in the previous move*/ 
        if(move == (prevMove ^0x01))
        {
            move = prevMove;
            continue;
        }
        i++;
        PrintMove(move);
        GetChild(&parent,&child,move);
        parent = child;
    }
    printf("\n");
    printf("getting rb state full\n");
    //if(!(&child))
      //  printf("null pointer child\n");
    if(!state)
        printf("null pointer state\n");
    printf("random state calculated\n");
}
/*
 * "dist": the number of moves to make from the given Node to obtain the random state*/
void GetRandRbPath(rbCubeState * state, UINT8 dist, rbCubeState * Path )
{
   int i;
   UINT8 move=14,prevMove;
   rbCubeState parent = *state;
   rbCubeState child;
   if( !state )
   {
	   printf("null pointer!!\n");
	   exit(0);
   }
  // printf("initial parent\n");
   //PrintFull(&parent);
   Path[0] = parent;
   for(i=1;i<dist;i++)
   {
      prevMove = move;
      move = randomMove();
//printf("i=%d\n",i);
	/*check if we are reversing what is done in the previous move*/ 
      while(move == (prevMove ^0x01))
      {
          prevMove = move;
	      move = randomMove();
	     // continue;
      }
//      else
           // printf("move=%d\n",move);
      //PrintMove(move);
      GetChild(&parent,&child,move);
      parent = child;
      Path[i] = child;
      //PrintFull(&parent);
      //PrintCompState(&Path[i]);
   }
   //printf("\n");
   //printf("getting rb state full\n");
   //if(!(&child))
//	   printf("null pointer child\n");
   //if(!state)
	 //  printf("null pointer state\n");
   //printf("random state calculated\n");
}
#if 1
/*
 * "dist": the number of moves to make from the Goal Node to obtain the random state*/
void GetRandRbStatePath(rbCubeState states[], UINT8 dist, UINT8 path[] )
{
   int i;
   UINT8 move;
   rbCubeState parent = GOAL_STATE;
   rbCubeState child;
   states[0] = parent;
   for(i=0;i<dist;i++)
   {
      path[i] = move = randomMove();
      GetChild(&parent,&child,move);
      states[i+1] = parent = child;
   }
}
void GetRandRbStatePath1(rbCubeState *state, UINT8 dist, UINT8 path[] )
{
    int i;
    UINT8 move=14,prevMove;
    rbCubeState parent = GOAL_STATE;
    rbCubeState child;
    for(i=0;i<dist;)
    {
        prevMove = move;
        path[i] = move = randomMove();
        if(move == (prevMove ^0x01))
        {
            move = prevMove;
            continue;
        }
        i++;
        GetChild(&parent,&child,move);
        //PrintMove(move);
        //PrintFull(&child);
    }
    *state = child;
}
void GetRandRbState(rbCubeState * state, UINT8 dist )
{
   int i;
   UINT8 move=14,prevMove;
   rbCubeState parent = GOAL_STATE;
   rbCubeState child;
   if( !state )
   {
	   printf("null pointer!!\n");
	   exit(0);
   }
   //printf("initial parent\n");
   //PrintFull(&parent);
   for(i=0;i<dist;)
   {
      prevMove = move;
      move = randomMove();
//printf("i=%d\n",i);
	/*check if we are reversing what is done in the previous move*/ 
      if(move == (prevMove ^0x01))
      {
	      move = prevMove;
	      continue;
      }
//      else
           // printf("move=%d\n",move);
      i++;
      //PrintMove(move);
      GetChild(&parent,&child,move);
      parent = child;
     // PrintFull(&parent);
   }
   printf("\n");
   //printf("getting rb state full\n");
   //if(!(&child))
//	   printf("null pointer child\n");
   if(!state)
	   printf("null pointer state\n");
   *state = child;
   //printf("random state calculated\n");
}
#endif
#define NMOVES 12
/*
1 - if states are equal
0 - if states are different
*/
unsigned char CompareFullStates( rbCubeState * s1, rbCubeState * s2)
{
    int i;
    for(i=0;i<NEDGECUBELETS;i++)
        if( s1->edgePos[i] != s2->edgePos[i] )
            return 0;
    for(i=0;i<NCORNERCUBELETS;i++)
        if( s1->cornerPos[i] != s2->cornerPos[i] )
            return 0;
    return 1;
}

unsigned char CompareFullStatesT( rbCubeState * s1, rbCubeState * s2)
{
    int i;
    for(i=0;i<9;i++)
        if( s1->edgePos[eposT[i]] != s2->edgePos[eposT[i]] )
            return 0;
    for(i=0;i<4;i++)
        if( s1->cornerPos[cposT[i]] != s2->cornerPos[cposT[i]] )
            return 0;
    return 1;

}
#if 0
unsigned char CompareFullStatesT1( rbCubeState * s1, rbCubeState * s2)
{
    int i;
    //UINT8 edgeDir1[12],cornerDir1[8];
    UINT8 edgePos1[12],cornerPos1[8];
    //UINT8 edgeDir2[12],cornerDir2[8];
    UINT8 edgePos2[12],cornerPos2[8];


    for(i=0;i<12;i++)
    {
        //edgeDir1[s1->edgePos[i]] = s1->edgeDir[i];
        edgePos1[s1->edgePos[i]] = i + 12*s1->edgeDir[i];
        //edgeDir2[s2->edgePos[i]] = s2->edgeDir[i];
        edgePos2[s2->edgePos[i]] = i + 12*s2->edgeDir[i];
    }

    for(i=0;i<8;i++)
    {
        cornerPos1[s1->cornerPos[i]] = i + 8*s1->cornerDir[i];
        //cornerDir1[s1->cornerPos[i]] = s1->cornerDir[i];
        cornerPos2[s2->cornerPos[i]] = i + 8*s2->cornerDir[i];
        //cornerDir2[s2->cornerPos[i]] = s2->cornerDir[i];
    }
    /*
       tripod

       yellow-green - 5
       yellow-pink - 8
       yellow-blue - 4
       yellow-red - 11
       red-green - 0
       red-blue - 1
       pink-blue - 3
       white-red - 10
       white-blue - 7

       white-red-blue - 4
       blue-yellow-pink - 7
       blue-yellow-red - 3
       yellow-red-green - 5
     */
    int eposT[9] = {0,1,3,4,5,7,8,10,11};
    int cposT[4] = {3,4,5,7};

    for(i=0;i<9;i++)
    {
        if(edgePos1[eposT[i]] != edgePos2[eposT[i]])
            return 0;
    }
    for(i=0;i<4;i++)
    {
        if(cornerPos1[cposT[i]] != cornerPos2[cposT[i]])
            return 0;
    }

    return 1;
}
#endif
unsigned char IsGoal( rbCubeState * s1 )
{
    rbCubeState s=GOAL_STATE;
	return CompareFullStates(&s,s1);
}
void CheckInverseMoves(void)
{
   int i,j;
   rbCubeState parent = GOAL_STATE;
   rbCubeState child,tmp=GOAL_STATE;
   printf("checking inverse moves...\n");
   for(i=0;i<NMOVES;i+=2)
   {
      GetChild(&parent,&child,i);
      parent = child;
      GetChild(&parent,&child,i+1);

      if( CompareFullStates(&child,&tmp) )
	 printf(" %d passed\n",i);
      else
	 printf(" %d failed\n",i);
      parent = child;
   }
}
void CheckSameMoves(void)
{
    int i,j;
    rbCubeState parent = GOAL_STATE;
    rbCubeState child,tmp=GOAL_STATE;
    printf("checking same moves...\n");
    for(i=0;i<NMOVES;i++)
    {
        for(j=0;j<4;j++)
        {
            GetChild(&parent,&child,i);
            parent = child;
        }
        if( CompareFullStates(&child,&tmp) )
            printf(" %d passed\n",i);
        else
            printf(" %d failed\n",i);
    }
}
void CheckMoveScheme(void)
{
   CheckSameMoves();
   CheckInverseMoves();
}

const static unsigned char CornerDirMap[2][3][3] =
{/*[layer_pair_index][previous_direction_value]*/
   {
      {0,1,2},
      {1,2,0},
      {2,0,1}
   },
   {
      { 0, 2, 1},
      { 1, 0, 2},
      { 2, 1, 0}
   }
};
/*Given that SfromGoal is obtained by making a set of moves from the Goal node, what is the 
state obtained when same set of moves are made on Ssource*/
void GetChildByConfig( rbCubeState * Ssource, rbCubeState * SfromGoal,  rbCubeState * Sdest )
{
    int i;
    for(i=0;i<NEDGECUBELETS;i++)
    {
        Sdest->edgePos[i] = (SfromGoal->edgePos[i] & 0x10) ^ ( Ssource->edgePos[0x0F & SfromGoal->edgePos[i]]);
    }

#define CPOS(pos) (0x07 & pos)
#define CDIR(pos) (pos >> 3)
    for(i=0;i<NCORNERCUBELETS;i++)
    {
        UINT8 fgpos = CPOS( SfromGoal->cornerPos[i] );
        UINT8 fgdir = CDIR( SfromGoal->cornerPos[i] );

        Sdest->cornerPos[ i ] = CPOS( Ssource->cornerPos[ fgpos ]) | ((CornerDirMap[(i>>2)!=(( fgpos )>>2 )][fgdir][Ssource->cornerPos[fgpos]>>3])<<3);


        //Sdest->cornerPos[ i ] = 0x07 & Ssource->cornerPos[ 0x07 & SfromGoal->cornerPos[i] ];
        //Sdest->cornerPos[ i ] |= ((CornerDirMap[(i>>2)!=(( 0x07 & SfromGoal->cornerPos[i])>>2 )][SfromGoal->cornerPos[i]>>3][Ssource->cornerPos[0x07 & SfromGoal->cornerPos[i]]>>3])<<3);
    }
}
#if 0
int main(void)
{
   rbCompState s;
   int i;
   CheckMoveScheme();
#if 0
   for(i=0;i<10;i++)
   {
      GetRandRbState(&s);
      PrintCompState(&s);
      getchar();
   }
#endif

}
#endif

void Check(void)
{
     rbCubeState s23,s235,s235New,s5,s2,s3,s23New;
     rbCubeState G=GOAL_STATE;
	 GetChild(&G,&s2,2);
	 PrintFullN(&s2,"s2");

	 GetChild(&G,&s3,3);
	 PrintFullN(&s3,"s3");

	 GetChild(&G,&s5,5);
	 PrintFullN(&s5,"s5");

	 GetChild(&s2,&s23,3);
	 PrintFullN(&s23,"s23");

	 GetChildByConfig(&s2,&s3,&s23New);
	 PrintFullN(&s23New,"s23New");

	 GetChild(&s23,&s235,5);
	 PrintFullN(&s235,"s235");

	 GetChildByConfig(&s23,&s5,&s235New);
	 PrintFullN(&s235New,"s235New");
}
void Check1(void)
{
     rbCubeState s27,s275,s275New,s5,s2,s7,s27New;
     rbCubeState G=GOAL_STATE;
	 GetChild(&G,&s2,2);
	 PrintFullN(&s2,"s2");

	 GetChild(&G,&s7,7);
	 PrintFullN(&s7,"s7");

	 GetChild(&G,&s5,5);
	 PrintFullN(&s5,"s5");

	 GetChild(&s2,&s27,7);
	 PrintFullN(&s27,"s27");

	 GetChildByConfig(&s2,&s7,&s27New);
	 PrintFullN(&s27New,"s27New");

	 GetChild(&s27,&s275,5);
	 PrintFullN(&s275,"s275");

	 GetChildByConfig(&s27,&s5,&s275New);
	 PrintFullN(&s275New,"s275New");
}
void Check2(void)
{
     rbCubeState s27,s275,s275New,s5,s2,s7,s27New,s775,s77;
     rbCubeState G=GOAL_STATE;
	 GetChild(&G,&s2,2);
	 PrintFullN(&s2,"s2");

	 GetChild(&G,&s7,7);
	 PrintFullN(&s7,"s7");

	 GetChild(&s7,&s77,7);
	 PrintFullN(&s77,"s77");

	 GetChild(&s77,&s775,5);
	 PrintFullN(&s775,"s775");

	 GetChild(&G,&s5,5);
	 PrintFullN(&s5,"s5");

	 GetChild(&s2,&s27,7);
	 PrintFullN(&s27,"s27");

	 GetChildByConfig(&s2,&s7,&s27New);
	 PrintFullN(&s27New,"s27New");

	 GetChild(&s27,&s275,5);
	 PrintFullN(&s275,"s275");

	 GetChildByConfig(&s27,&s5,&s275New);
	 PrintFullN(&s275New,"s275New");
}

/*Get the position value of corner cubelet pos*/
int GetCornerPos(rbCubeState * s, int pos)
{
    int i;
	for(i=0;i<8;i++)
	{
	    if (s->cornerPos[i] == pos)
		    return i;
	}
	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
	return 9;
}
/*Get the direction value of corner cubelet pos*/
int GetCornerDir(rbCubeState * s, int pos)
{
    int i;
	for(i=0;i<8;i++)
	{
	    if ((s->cornerPos[i] & 0x07) == pos)
		    return s->cornerPos[i]>>3;
	}
	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
	return 3;
}
int MapDirTuple(int d1,int d2,int d3)
{
    if (d1==0)
	{
	   if(d2==1)
	      return 0;
       else
	      return 1;
	}
	else if (d1==1)
	{
	    if(d2==2)
		    return 2;
			else return 3;
	}
	else
	{
	    if(d2==0)
		   return 4;
		   else return 5;
	}
}
void GetModGoalState(rbCubeState * s, int pos, int dir)
{
    rbCubeState s1 = GOAL_STATE;
    *s = s1;
	s->cornerPos[pos] |= (dir<<3);
}
void Check3()
{
    int i,move,pos,d1,d2,d3,curPos,index,k;
	for(pos=0;pos<8;pos++)
	{
	    int Dir[6][8]={0};
		rbCubeState sA,s1A,s2A;
		rbCubeState s=GOAL_STATE;
		rbCubeState s1;
		GetModGoalState(&s1,pos,1);
		rbCubeState s2;
		GetModGoalState(&s2,pos,2);
		for(i=0;i<400;i++)
		{
			move = randomMove();
			//printf("\n\ni=%d, move=%d\n",i,move);
			GetChild(&s,&sA,move);
			//	    PrintFullN(&sA,"sA");
			curPos = GetCornerPos(&sA,pos);
			//printf("%d: cpos: %d\n",pos,GetCornerPos(&sA,pos));
			d1 = GetCornerDir(&sA,pos);
			//printf("0: cd: %d\n",d1);
			//		PrintCorDir(&sA,0);
			GetChild(&s1,&s1A,move);
			d2 = GetCornerDir(&s1A,pos);
			//printf("1: cd: %d\n",d2);
			//	    PrintFullN(&s1A,"s1A");
			GetChild(&s2,&s2A,move);
			d3 = GetCornerDir(&s2A,pos);
			//printf("2: cd: %d\n",d3);
			index = MapDirTuple(d1,d2,d3);
			Dir[index][curPos]=1;
			//	    PrintFullN(&s2A,"s2A");
			s = sA;
			s1 = s1A;
			s2 = s2A;
		}
		printf("pos=%d\n",pos);
		for(i=0;i<6;i++)
		{ 
		    switch(i)
			{
			case 0: printf("0,1,2::");break;
			case 1: printf("0,2,1::");break;
			case 2: printf("1,2,0::");break;
			case 3: printf("1,0,2::");break;
			case 4: printf("2,0,1::");break;
			case 5: printf("2,1,0::");break;
			}

		    //printf("i=%d::",i);
			for(k=0;k<8;k++)
			    if(Dir[i][k]==1)
				    printf("%d, ",k);
			printf("\n");
		}
	}
}
void Check4(int mc1, int mc2)
{
    rbCubeState s=GOAL_STATE,s1,s2,s3=GOAL_STATE,s4,sr,sr3New,s3rNew;
	int i,move;
    printf("moveA=");
	for(i=0;i<mc1;i++)
	{
		move = randomMove();
		printf("%d,",move);
		GetChild(&s,&s1,move);
		s = s1;
	}
	printf("\n");
	sr = s1;
	//PrintFullN(&s1,"first s1");
	printf("moveB=");
	for(i=0;i<mc2;i++)
	{
		move = randomMove();
		printf("%d,",move);
		GetChild(&s,&s1,move);
		GetChild(&s3,&s4,move);
		s = s1;
		s3 = s4;
	}
	printf("\n");

    GetChildByConfig(&sr,&s3,&sr3New);
    GetChildByConfig(&s3,&sr,&s3rNew);
	PrintFullN(&s1,"s1");
	PrintFullN(&sr3New,"sr3New");
	//PrintFullN(&s3rNew,"s3rNew");
	printf("mc1=%d,mc2=%d\n",mc1,mc2);
	for(i=0;i<8;i++)
	{
		if(s1.cornerPos[i] != sr3New.cornerPos[i])
		{
			printf("cd mismatch i=%d,pos=%d sdir=%d, sr3Newdir=%d\n",i,s1.cornerPos[i],s1.cornerPos[i],sr3New.cornerPos[i]);
		}
	}


}
void check6()
{
	rbCubeState s=GOAL_STATE;
	rbCubeState s1;
	rbCubeState sComp,s1Comp;
	int move;
	while(1)
	{
		printf("enter move(0 to 11). 12 to exit\n");
		scanf("%d",&move);
		if(move>=12)
			return;
		GetChild(&s,&s1,move);
        PrintFull( &s1 );
		sComp = s1Comp;
		s = s1;
	}
}
/*The list of moves given is the list of color layers. Each move is made twice
as we need to traverse in the reduced Graph*/
void GetnMoveStateGen(rbCubeState *sA, int MoveSeq[], int numMoves, rbCubeState * s1)
{
    rbCubeState s= *s1 = *sA;
    int i;
    for(i=0;i<numMoves;i++)
    {
        GetChild(&s,s1,MoveSeq[i]*2);
        s = *s1;
        GetChild(&s,s1,MoveSeq[i]*2);
        //PrintFull(s1);
        s = *s1;
    }
    //PrintFull(s1);
    //printf("\n");
}

/*Gives the state(in s1) obtained by making given sequence of moves from Goal node
Each move here is a double turn*/
void GetnMoveState(int MoveSeq[], int numMoves, rbCubeState * s1)
{
    rbCubeState s = GOAL_STATE;
     *s1 = s;
	GetnMoveStateGen(&s,MoveSeq,numMoves,s1);
}

typedef struct moveseq_{
   int seq1[10];
   int seq2[10];
   int nMoves;
} MoveSeq_t;
void Check5()
{
    rbCubeState s1,s2;
    MoveSeq_t MoveSeq[]=
    {
      {{1,3,5,2,0},{0,2,4,3,1},5},
      {{3,1,0},{1,0,3},3},
      {{0,2,0},{2,0,0},3}
      
    };
    int i,j,n;
    n=sizeof(MoveSeq)/sizeof(MoveSeq_t);
    for(i=0;i<1;i++)
    {
       GetnMoveState(MoveSeq[i].seq1,MoveSeq[i].nMoves,&s1);
       GetnMoveState(MoveSeq[i].seq2,MoveSeq[i].nMoves,&s2);
       if(!CompareFullStates(&s1,&s2))
          printf("%d states are not same\n",i);
       else
          printf("%d states are same\n",i);

    }
}
#if 0
int main(void)
{
	int choice;
	while(1)
	{
		printf("enter 1 to check. 0 to quit\n");
		scanf("%d",&choice);
		if(choice==0)
			return 0;
		check6();
		//Check2();
	}
	return 0;
	//Check3();
	//Check4(1,1);
	//Check4(1,2);
	//Check4(2,1);
	/*Check4(2,2);
	  Check4(2,3);
	  Check4(33,42);
	  Check4(53,32);
	  Check4(23,22);*/
}
#endif
void check6a()
{
    int layer,nMoves=0,i;
	int moves[50];
    rbCubeState s=GOAL_STATE,gs=GOAL_STATE;
    rbCubeState s1,s2;
	while(1)
	{
		printf("enter layer(0,1,2,3,4,5) for double turn:\n");
		scanf("%d",&layer);
		moves[nMoves]=layer;
		nMoves++;
		if(layer==6)
			break;
		GetChild(&s,&s1,layer*2);
		s=s1;
		GetChild(&s,&s1,layer*2);
		PrintFull(&s1);
		for(i=0;i<nMoves;i++)
		    printf("%d ",moves[i]);
		printf("\n");
		s=s1;
		if(IsGoal( &s1))
		{
			printf("Reached goal\n");
			break;
		}

	}
}
#if 0
UINT8 GetEdgePosGroup1Index(UINT8 edgePos[NEDGECUBELETS])
{
   UINT8 perm[4];
   perm[0] = edgePos[green_red];
   perm[1] = edgePos[red_blue];
   perm[2] = edgePos[green_pink];
   perm[3] = edgePos[pink_blue];

   GetPermNormalForm(perm,4);

   return (UINT8)GetIndexFromPerm(perm,4);
   
}
UINT8 GetEdgePosGroup2Index(UINT8 edgePos[NEDGECUBELETS])
{
   UINT8 perm[4];
   perm[0] = edgePos[blue_yellow];
   perm[1] = edgePos[yellow_green];
   perm[2] = edgePos[green_white];
   perm[3] = edgePos[white_blue];

   GetPermNormalForm(perm,4);

   return (UINT8)GetIndexFromPerm(perm,4);
}
UINT8 GetEdgePosGroup3Index(UINT8 edgePos[NEDGECUBELETS])
{
   UINT8 perm[4];
   perm[0] = edgePos[yellow_pink];
   perm[1] = edgePos[pink_white];
   perm[2] = edgePos[white_red];
   perm[3] = edgePos[red_yellow];
   
   GetPermNormalForm(perm,4);

   return (UINT8)GetIndexFromPerm(perm,4);
}
UINT8 GetCornerPosGroup1Index(UINT8 cornerPos[NCORNERCUBELETS])
{
   UINT8 perm[4];

   perm[0] = cornerPos[pink_yellow_green];
   perm[1] = cornerPos[green_white_red];
   perm[2] = cornerPos[pink_white_blue];
   perm[3] = cornerPos[blue_red_yellow];
   
   GetPermNormalForm(perm,4);

   return (UINT8)GetIndexFromPerm(perm,4);
}
UINT8 GetCornerPosGroup2Index(UINT8 cornerPos[NCORNERCUBELETS])
{
   UINT8 perm[4];

   perm[0] = cornerPos[red_white_blue];
   perm[1] = cornerPos[red_green_yellow];
   perm[2] = cornerPos[white_green_pink];
   perm[3] = cornerPos[yellow_blue_pink];
      
   GetPermNormalForm(perm,4);

   return (UINT8)GetIndexFromPerm(perm,4);
}
#endif
void GetD5Index( rbCubeState * state, UINT8 id[5])
{
   UINT8 edgePos[NEDGECUBELETS],cornerPos[NCORNERCUBELETS];
/*
   GetPermFromIndex( edgePos, state->edgePos, NEDGECUBELETS );
   GetPermFromIndex( cornerPos, state->cornerPos, NCORNERCUBELETS );
   
   //PrintArray(edgePos,NEDGECUBELETS);
   //printf("\n");
   //PrintArray(cornerPos,NCORNERCUBELETS);
  // printf("\n");
   id[0] = GetEdgePosGroup1Index(edgePos);
   id[1] = GetEdgePosGroup2Index(edgePos);
   id[2] = GetEdgePosGroup3Index(edgePos);
   id[3] = GetCornerPosGroup1Index(cornerPos);
   id[4] = GetCornerPosGroup2Index(cornerPos);
   */
}
typedef struct CubeNodes
{
	int nMoves;
	int Moves[10];
	rbCubeState s;/*The state obtained when the above moves are made*/ 
} CubeNodes_t;
typedef struct Cube
{
    int Moves[3];
    rbCubeState states[24];
}Cube_t;
int main2()
{
    check6a();
	return 0;
}
int main1()
{
	CubeNodes_t Nodes[]
		=
		{
			/*
			A "reduced graph" is a graph obtained when only double turns are made.
			A "cube" is the cube traversed when only double turns of layer pairs are made.
			   An "open vertex" is a vertex from which there are also edges going out of the cube
			   A "closed vertex" is a vertex from which all edges go into the same cube
			   The 12 edges of the cube obtained when a pair of moves from a layer pair are made
			   are called "full-edges"
			   But each "full-edge" is a combination of 4 half edges
			   The sequence of moves 0,1 or 1,0 or 2,3 or 3,2 or 4,5 or 5,4 give a full-edge
			   But a single move(0 or 1 or 2 or 3 or 4 or 5) gives a "half-edge"
			   Half-edge obtained by (double) turning the green layer is "green-half-edge"
			   Half-edge obtained by (double) turning the blue layer is "blue-half-edge"
			   Since blue and green are opposite faces/layers a "blue-half-edge" and
			   "green-half-edge" next to each other form a "blue-green-full-edge".
			   A "blue-green-full-edge" takes us to the same node as a "green-blue-full-edge"
			   Infact they are the same full edge in the cube. A full-edge is one of the 12 edges
			   of the actual cube.
			   When we take a half-edge from a closed vertex, we reach an open vertex.
			   When we take a full-edge from a closed vertex, we reach another closed vertex.
			   When we take a pairing half-edge from an open vertex, we reach a closed vertex of the 
			   same cube.
			   When we take a non-pairing half-edge from an open vertex, we reach a new cube
			 */

			/*The 24 open vertices of a cube */
			/*0*/	{1,{0},{0}},
			/*1*/	{1,{1},{0}},
			/*2*/	{1,{2},{0}},
			/*3*/	{1,{3},{0}},
			/*4*/	{1,{4},{0}},
			/*5*/	{1,{5},{0}},
			/*6*/	{3,{0,1,2},{0}},
			/*7*/	{3,{0,1,3},{0}},
			/*8*/	{3,{0,1,4},{0}},
			/*9*/	{3,{0,1,5},{0}},
			/*10*/	{3,{2,3,0},{0}},
			/*11*/	{3,{2,3,1},{0}},
			/*12*/	{3,{2,3,4},{0}},
			/*13*/	{3,{2,3,5},{0}},
			/*14*/	{3,{4,5,0},{0}},
			/*15*/	{3,{4,5,1},{0}},
			/*16*/	{3,{4,5,2},{0}},
			/*17*/	{3,{4,5,3},{0}},
			/*18*/	{5,{0,1,2,3,4},{0}},
			/*19*/	{5,{0,1,2,3,5},{0}},
			/*20*/	{5,{0,1,4,5,2},{0}},
			/*21*/	{5,{0,1,4,5,3},{0}},
			/*22*/	{5,{2,3,4,5,0},{0}},
			/*23*/	{5,{2,3,4,5,1},{0}}
		};
	CubeNodes_t Nodes1[] =
	{
		{4,{3,4,3,0},{0}},
		{4,{3,4,3,1},{0}},
		{4,{3,4,3,2},{0}},
		{4,{3,4,3,3},{0}},
		{4,{3,4,3,4},{0}},
		{4,{3,4,3,5},{0}},
		{6,{3,4,3,0,1,2},{0}},
		{6,{3,4,3,0,1,3},{0}},
		{6,{3,4,3,0,1,4},{0}},
		{6,{3,4,3,0,1,5},{0}},
		{6,{3,4,3,2,3,0},{0}},
		{6,{3,4,3,2,3,1},{0}},
		{6,{3,4,3,2,3,4},{0}},
		{6,{3,4,3,2,3,5},{0}},
		{6,{3,4,3,4,5,0},{0}},
		{6,{3,4,3,4,5,1},{0}},
		{6,{3,4,3,4,5,2},{0}},
		{6,{3,4,3,4,5,3},{0}},
		{8,{3,4,3,0,1,2,3,4},{0}},
		{8,{3,4,3,0,1,2,3,5},{0}},
		{8,{3,4,3,0,1,4,5,2},{0}},
		{8,{3,4,3,0,1,4,5,3},{0}},
		{8,{3,4,3,2,3,4,5,0},{0}},
		{8,{3,4,3,2,3,4,5,1},{0}}
	};
	Cube_t Cubes24[24];
	int i,j,k,l,i1,i2,count=0,k1;
	rbCubeState diff;
	rbCubeState sA;
	int seq[]={4,3,4};
	for(i=0;i<24;i++)
	{
		GetnMoveState(Nodes[i].Moves, Nodes[i].nMoves, &Nodes[i].s);
	}
	for(i1=0;i1<6;i1++)/*for each layer/color/face*/
	{
		for(i2=0;i2<6;i2++)/*for each layer/color/face*/
		{
            /*skip the same layer and same laye pair moves, as they won't take us into a new cube*/
			if(i1/2==i2/2)
				continue;
			seq[0]=i1;
			seq[1]=i2;
			seq[2]=i1;
			Cubes24[count].Moves[0] = seq[0];
			Cubes24[count].Moves[1] = seq[1];
			Cubes24[count].Moves[2] = seq[2];
			GetnMoveState(seq,3,&sA);
			for(i=0;i<24;i++)
			{
				GetnMoveStateGen(&sA,Nodes[i].Moves, Nodes[i].nMoves, &Nodes1[i].s);
				Cubes24[count].states[i] = Nodes1[i].s;
			}
			for(i=0;i<count;i++)
				for(j=0;j<24;j++)
					for(k1=0;k1<24;k1++)
					{
						RbCubeConfigDiff( &Cubes24[i].states[j], &Nodes1[k1].s, &diff);
						if(IsGoal(&diff))
						{
							printf("diff is goal\n");
							continue;
						}
						/*check if they are 1 double turn apart*/
						for(k=0;k<6;k++)
							if(CompareFullStates(&Nodes[k].s,&diff))
							{
								printf("states match i=%d j=%d, k=%d   moves:",i,j,k);
								for(l=0;l<Nodes[j].nMoves;l++)
									printf("%d ",Nodes[j].Moves[l]);
								printf(" - ");
								for(l=0;l<3;l++)
									printf("%d ",seq[l]);
								printf("\n");
							}
					}
			printf("starting comparison\n");
			for(i=0;i<24;i++)
			{
				for(j=0;j<24;j++)
				{
					RbCubeConfigDiff( &Nodes[i].s, &Nodes1[j].s, &diff);
					/*check if they are 1 double turn apart*/
					for(k=0;k<6;k++)
						if(CompareFullStates(&Nodes[k].s,&diff))
						{
							printf("states match i=%d j=%d, k=%d   moves:",i,j,k);
							for(l=0;l<Nodes[j].nMoves;l++)
								printf("%d ",Nodes[j].Moves[l]);
							printf(" - ");
							printf(" %d  ",k);
							printf(" - ");
							for(l=Nodes[j].nMoves-1;l>=0;l--)
								printf("%d ",Nodes[j].Moves[l]);
							printf(" - ");
							for(l=0;l<3;l++)
								printf("%d ",seq[l]);
							printf("\n");
						}
				}
			}
			count++;
			printf("(%d)",count);
		}
	}
	return 0;
}
/*
A "cube" has 24 open vertices. There are 96 half edges coming out of these vertices
that go out of the cube. Rest of the 48 half edges coming out of these open vertices
land in the same cube.

It is found that these 96 half edges land into 24 new cubes. The starting nodes of those
cubes are given below.

A move is a face/layer/colour
0,1 is a layer pair
2,3 is a layer pair
4,5 is a layer pair

Explore the tree of cubes where each cube has 24 neighbours. To find the pattern in which
the cubes are connected.

The 24 neighboours of a cube are cubes with starting node obtained
by making the below moves from the Goal node. Each move here is a double turn.
The below table lists all the 96 half edges leading to 24 neighburing cubes

0:  0 2 0  (2 3 1 2 1; 4 5 1 3 1; 2 3 4 5 0 3 0)  <- 4 half edges taking us to the same neighbouring cube
    ^^^^^
      |
half-edge1 (first 0 is half-edge in current cube; 2 is the connecting half edge; last 0 is the half edge
in the new cube taking us to a closed vertex in the new cube)

edge: 2 3 4 5 0 3 0
edge: 2 3(reached closed vertex in the same cube)
      4 5(reached another closed vertex in same cube)
        0(reached same cube, open vertex)
        3(reached new cube, open vertex)  --- this is the "connecting edge"
		0(reached closed vertex of new cube) --- this is an edge in the new/neighbour cube

In the below table the columns representing "connecting edge" are labeled "c"
       c             c            c                c
0:  0  2  0  (2 3 1  2  1; 4 5 1  3  1; 2 3 4 5 0  3  0)
1:  0  3  0  (2 3 1  3  1; 4 5 1  2  1; 2 3 4 5 0  2  0)
2:  0  4  0  (2 3 1  5  1; 4 5 1  4  1; 2 3 4 5 0  5  0)
3:  0  5  0  (2 3 1  4  1; 4 5 1  5  1; 2 3 4 5 0  4  0)
4:  1  2  1  (2 3 0  2  0; 4 5 0  3  0; 2 3 4 5 1  3  1)
5:  1  3  1  (2 3 0  3  0; 4 5 0  2  0; 2 3 4 5 1  2  1)
6:  1  4  1  (2 3 0  5  0; 4 5 0  4  0; 2 3 4 5 1  5  1)
7:  1  5  1  (2 3 0  4  0; 4 5 0  5  0; 2 3 4 5 1  4  1)
8:  2  0  2  (0 1 3  0  3; 4 5 3  1  3; 0 1 4 5 2  1  2)
9:  2  1  2  (0 1 3  1  3; 4 5 3  0  3; 0 1 4 5 2  0  2)
10: 2  4  2  (0 1 3  5  3; 4 5 3  4  3; 0 1 4 5 2  5  2)
11: 2  5  2  (0 1 3  4  3; 4 5 3  5  3; 0 1 4 5 2  4  2)
12: 3  0  3  (0 1 2  0  2; 4 5 2  1  2; 0 1 4 5 3  1  3)
13: 3  1  3  (0 1 2  1  2; 4 5 2  0  2; 0 1 4 5 3  0  3)
14: 3  4  3  (0 1 2  5  2; 4 5 2  4  2; 0 1 4 5 3  5  3)
15: 3  5  3  (0 1 2  4  2; 4 5 2  5  2; 0 1 4 5 3  4  3)
16: 4  0  4  (0 1 5  0  5; 2 3 5  1  5; 0 1 2 3 4  1  4)
17: 4  1  4  (0 1 5  1  5; 2 3 5  0  5; 0 1 2 3 4  0  4)
18: 4  2  4  (0 1 5  3  5; 2 3 5  2  5; 0 1 2 3 4  3  4)
19: 4  3  4  (0 1 5  2  5; 2 3 5  3  5; 0 1 2 3 4  2  4)
20: 5  0  5  (0 1 4  0  4; 2 3 4  1  4; 0 1 2 3 5  1  5)
21: 5  1  5  (0 1 4  1  4; 2 3 4  0  4; 0 1 2 3 5  0  5)
22: 5  2  5  (0 1 4  3  4; 2 3 4  2  4; 0 1 2 3 5  3  5)
23: 5  3  5  (0 1 4  2  4; 2 3 4  3  4; 0 1 2 3 5  2  5)
*/

int Neighbour[24][3]=
{
/*0:*/  {0,  2,  0},  //(2 3 1  2  1; 4 5 1  3  1; 2 3 4 5 0  3  0)
/*1:*/  {0,  3,  0},  //(2 3 1  3  1; 4 5 1  2  1; 2 3 4 5 0  2  0)
/*2:*/  {1,  2,  1},  //(2 3 0  2  0; 4 5 0  3  0; 2 3 4 5 1  3  1)
/*3:*/  {1,  3,  1},  //(2 3 0  3  0; 4 5 0  2  0; 2 3 4 5 1  2  1)
/*8:*/  {2,  0,  2},  //(0 1 3  0  3; 4 5 3  1  3; 0 1 4 5 2  1  2)
/*9:*/  {2,  1,  2},  //(0 1 3  1  3; 4 5 3  0  3; 0 1 4 5 2  0  2)
/*10:*/ {3,  0,  3},  //(0 1 2  0  2; 4 5 2  1  2; 0 1 4 5 3  1  3)
/*11:*/ {3,  1,  3},  //(0 1 2  1  2; 4 5 2  0  2; 0 1 4 5 3  0  3)
/*4:*/  {0,  4,  0},  //(2 3 1  5  1; 4 5 1  4  1; 2 3 4 5 0  5  0)
/*5:*/  {0,  5,  0},  //(2 3 1  4  1; 4 5 1  5  1; 2 3 4 5 0  4  0)
/*6:*/  {1,  4,  1},  //(2 3 0  5  0; 4 5 0  4  0; 2 3 4 5 1  5  1)
/*7:*/  {1,  5,  1},  //(2 3 0  4  0; 4 5 0  5  0; 2 3 4 5 1  4  1)
/*16:*/ {4,  0,  4},  //(0 1 5  0  5; 2 3 5  1  5; 0 1 2 3 4  1  4)
/*17:*/ {4,  1,  4},  //(0 1 5  1  5; 2 3 5  0  5; 0 1 2 3 4  0  4)
/*18:*/ {5,  0,  5},  //(0 1 4  0  4; 2 3 4  1  4; 0 1 2 3 5  1  5)
/*19:*/ {5,  1,  5},  //(0 1 4  1  4; 2 3 4  0  4; 0 1 2 3 5  0  5)
/*12:*/ {2,  4,  2},  //(0 1 3  5  3; 4 5 3  4  3; 0 1 4 5 2  5  2)
/*13:*/ {2,  5,  2},  //(0 1 3  4  3; 4 5 3  5  3; 0 1 4 5 2  4  2)
/*14:*/ {3,  4,  3},  //(0 1 2  5  2; 4 5 2  4  2; 0 1 4 5 3  5  3)
/*15:*/ {3,  5,  3},  //(0 1 2  4  2; 4 5 2  5  2; 0 1 4 5 3  4  3)
/*20:*/ {4,  2,  4},  //(0 1 5  3  5; 2 3 5  2  5; 0 1 2 3 4  3  4)
/*21:*/ {4,  3,  4},  //(0 1 5  2  5; 2 3 5  3  5; 0 1 2 3 4  2  4)
/*22:*/ {5,  2,  5},  //(0 1 4  3  4; 2 3 4  2  4; 0 1 2 3 5  3  5)
/*23:*/ {5,  3,  5}   //(0 1 4  2  4; 2 3 4  3  4; 0 1 2 3 5  2  5)
};
typedef struct CubeNew
{
    //int nMoves;
    //int Moves[30];/*moves made at Goal state to reach a closed vertex in this cube*/
    //rbCubeState open_vertices[24];
    rbCubeState firstVertex;
    //rbCubeState closed_vertices[8];
	int NeighbourIndex[24];/*the indices at which 24 neighbours are present*/
	//int NeighbourMoves[24][3];
//	int first_child_index;
//	int last_child_index;/*index into an array of nodes*/
}CubeNew_t;
struct
{
    int len;
    int path[6];
}ClosedVertices[8]=
{
    /*0*/{0,{0}},
    /*1*/{2,{0,1}},
    /*2*/{2,{2,3}},
    /*3*/{2,{4,5}},
    /*4*/{4,{0,1,2,3}},
    /*5*/{4,{0,1,4,5}},
    /*6*/{4,{2,3,4,5}},
    /*7*/{6,{0,1,2,3,4,5}}
};

struct 
{
    int len;
    int path[6];
}CubeVertices[32]=
    /*The 32 vertices of a cube */
{
    /*0*/{0,{0}},
    /*1*/{2,{0,1}},
    /*2*/{2,{2,3}},
    /*3*/{2,{4,5}},
    /*4*/{4,{0,1,2,3}},
    /*5*/{4,{0,1,4,5}},
    /*6*/{4,{2,3,4,5}},
    /*7*/{6,{0,1,2,3,4,5}},
    /*The 24 open vertices of a cube */
    /*8  */	{1,{0}},
    /*9*/	{1,{1}},
    /*10*/	{1,{2}},
    /*11*/	{1,{3}},
    /*12*/	{1,{4}},
    /*13*/	{1,{5}},
    /*14*/	{3,{0,1,2}},
    /*15*/	{3,{0,1,3}},
    /*16*/	{3,{0,1,4}},
    /*17*/	{3,{0,1,5}},
    /*18*/	{3,{2,3,0}},
    /*19*/	{3,{2,3,1}},
    /*20*/	{3,{2,3,4}},
    /*21*/	{3,{2,3,5}},
    /*22*/	{3,{4,5,0}},
    /*23*/	{3,{4,5,1}},
    /*24*/	{3,{4,5,2}},
    /*25*/	{3,{4,5,3}},
    /*26*/	{5,{0,1,2,3,4}},
    /*27*/	{5,{0,1,2,3,5}},
    /*28*/	{5,{0,1,4,5,2}},
    /*29*/	{5,{0,1,4,5,3}},
    /*30*/	{5,{2,3,4,5,0}},
    /*31*/	{5,{2,3,4,5,1}}
};
int curNode,TotalCubes;
CubeNew_t redGraphCubes[20736];
#define POWER24_5 7962624
int redGrD5[POWER24_5];


/*Generates the 8 closed vertices of the "cube"
Parameters:
    rbCubeState s: the first closed vertex
    int index: the index in to the array of "cubes"*/

/*
Get the state of the vertex number "vertex" in cube number "cube" in the Goal reduced Graph
*/
void GetCubeVertexState(unsigned int cube, unsigned short int vertex,rbCubeState *parent_state)
{
    if ( vertex < 32 )
        GetnMoveStateGen(&redGraphCubes[cube].firstVertex,CubeVertices[vertex].path, CubeVertices[vertex].len, parent_state);
    else
        printf("error %s:%d\n",__FILE__,__LINE__);
}
void SaveNodeD5(rbCubeState *s, int index)
{
	rbCubeState s1;
	rbCompState sComp;
	UINT8 id5[5];
	int j;
	for(j=0;j<8;j++)
	{
	    //printf("safe101 %d\n",index);
		GetnMoveStateGen(s,ClosedVertices[j].path, ClosedVertices[j].len, &s1);
	    //printf("safe102 %d\n",index);
		GetD5Index( &s1, id5);
	    //printf("safe104 %d\n",index);
		redGrD5[id5[0]+id5[1]*24+id5[2]*24*24+id5[3]*24*24*24+id5[4]*24*24*24*24] = index;
	    //printf("safe11 %d\n",index);
	}
}
void GenerateNeighbour(int gr_index,int neighb_ind)
{
    rbCubeState s;
    rbCubeState sComp;
    UINT8 id5[5];
    int gr_index1;
    //printf("18th neighbour of 81 is: %d\n",redGraphCubes[81].NeighbourIndex[18]);
    //printf("safe4 %d\n",neighb_ind);
    GetnMoveStateGen(&redGraphCubes[gr_index].firstVertex,Neighbour[neighb_ind], 3, &s);
    //printf("safe5 %d\n",neighb_ind);
    //printf("safe6 %d\n",neighb_ind);
    GetD5Index( &s, id5);
    //printf("safe7 %d\n",neighb_ind);
    if(redGrD5[id5[0]+id5[1]*24+id5[2]*24*24+id5[3]*24*24*24+id5[4]*24*24*24*24] == -1)
        /*not present*/
    {
        //printf("safe3 %d\n",neighb_ind);
        redGraphCubes[TotalCubes].firstVertex = s;
        SaveNodeD5(&s,TotalCubes);
        redGraphCubes[TotalCubes].NeighbourIndex[neighb_ind] = gr_index;
        redGraphCubes[gr_index].NeighbourIndex[neighb_ind] = TotalCubes;
        //printf("new cube found parent=%d new neighbour=%d neighb=%d\n",gr_index,TotalCubes,neighb_ind);
        TotalCubes++;
    }
    else/*present*/
    {
        gr_index1 = redGrD5[id5[0]+id5[1]*24+id5[2]*24*24+id5[3]*24*24*24+id5[4]*24*24*24*24];
        //printf("safe31 %d\n",neighb_ind);
        //redGraphCubes[gr_index1].NeighbourIndex[neighb_ind] = gr_index;
        //printf("safe32 %d\n",neighb_ind);
        redGraphCubes[gr_index].NeighbourIndex[neighb_ind] = gr_index1;
        //printf("safe33 %d\n",neighb_ind);
        //printf("cube present in graph parent=%d existing neighbour=%d neighb=%d\n",gr_index,gr_index1,neighb_ind);
    }
    if(redGraphCubes[81].NeighbourIndex[18]==431)
    {
        printf("culprit here\n");
    }
}
void GenerateNeighbours(int gr_index)
{
    int neighb_ind;
	printf("generating neighbours for %d\n",gr_index);
    for(neighb_ind=0;neighb_ind<24;neighb_ind++)
	{
	    //printf("safe3 %d\n",neighb_ind);
	    GenerateNeighbour(gr_index,neighb_ind);
	}
	printf("TotalCubes=%d\n",TotalCubes);
}
void PrintCubes()
{
	int i,j;
	for(i=0;i<TotalCubes;i++)
	{
		//PrintNeighbours(i);
		printf("%5d Neb:",i);
		for(j=0;j<24;j++)
		{
			printf("%6d",redGraphCubes[i].NeighbourIndex[j]);
		}
		printf("\n");
	}
}
void GenerateCubes()
{
	int i,j;
	rbCubeState s = GOAL_STATE;
	for(i=0;i<POWER24_5;i++)
		redGrD5[i] = -1;
	SaveNodeD5(&s,0);
	redGraphCubes[0].firstVertex = s;
	TotalCubes = 1;
	for(i=0;i<TotalCubes;i++)
	{
		GenerateNeighbours(i);
	}
}
int main22()
{
    GenerateCubes();
    PrintCubes();
	return 0;
}
