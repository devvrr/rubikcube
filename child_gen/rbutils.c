#include "../rbdefs.h"
#include "../rbmath.h"
#include "edge_pos.h"
#include "edge_dir.h"
#include "corner_pos.h"
#include "corner_dir.h"
char randomMove( void );


#define NULL_STATE {{0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}
#define GOAL_STATE1 {{0,1,2,3,4,5,6,7,8,9,10,11},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,1,2,3,4,5,6,7},{1,0,0,0,0,0,0,0}}
#define GOAL_STATE2 {{0,1,2,3,4,5,6,7,8,9,10,11},{0,0,0,0,0,0,0,0,0,0,0,0},\
   {0,1,2,3,4,5,6,7},{2,0,0,0,0,0,0,0}}


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
   printf("\nedgeDir =  ");
   for(i=0;i<12;i++)
      printf("%3u",s->edgeDir[i]);
   printf("\ncornerPos =");
   for(i=0;i<8;i++)
      printf("%3u",s->cornerPos[i]);
   printf("\ncornerDir =");
   for(i=0;i<8;i++)
      printf("%3u",s->cornerDir[i]);
   printf("\n");
   //getchar();
}
void PrintFullN( rbCubeState * s, char * StateName )
{
   printf("\nstate:%s =>\n",StateName);
   PrintFull(s);
}
/*compute the full state from compressed state*/
void GetRBStateFullFromComp(rbCompState * sComp, rbCubeState * sFull )
{
   UINT8 edgeDir[12],cornerDir[8],i;
   GetPermFromIndex(sFull->edgePos,sComp->edgePos, NEDGECUBELETS);
   GetPermFromIndex(sFull->cornerPos,sComp->cornerPos, NCORNERCUBELETS);
   //printf("**dir = %u \n",sComp->dir );
   //printf("**edge dir = %u \n",GET_EDGE_DIR(sComp->dir) );
   RadixTenToN(edgeDir, GET_EDGE_DIR(sComp->dir), 2,NEDGECUBELETS);
   for(i=0;i<12;i++)
       sFull->edgeDir[i] = edgeDir[sFull->edgePos[i]];
   //printf("**corner dir = %u \n",GET_CORNER_DIR(sComp->dir) );
   RadixTenToN(cornerDir, GET_CORNER_DIR(sComp->dir), 3,NCORNERCUBELETS);
   for(i=0;i<8;i++)
       sFull->cornerDir[i] = cornerDir[sFull->cornerPos[i]];
   //PrintFull( sFull );
}
/*compute the compressed state from full state*/
void GetRBStateCompFromFull( rbCubeState * sFull, rbCompState * sComp )
{
   UINT8 edgeDir[12],cornerDir[8],i;
   /*get the index of the permutation formed by the edge cubelet positions*/
   sComp->edgePos = GetIndexFromPerm( sFull->edgePos, NEDGECUBELETS );
   sComp->cornerPos = (UINT16)GetIndexFromPerm( sFull->cornerPos, NCORNERCUBELETS );
   for(i=0;i<12;i++)
       edgeDir[sFull->edgePos[i]] = sFull->edgeDir[i];
   sComp->dir = (UINT16)RadixNToTen(edgeDir,2,NEDGECUBELETS);
   //printf("%u\n",sComp->dir);
   sComp->dir <<=16;
   //printf("%u\n",sComp->dir);
   for(i=0;i<8;i++)
       cornerDir[sFull->cornerPos[i]] = sFull->cornerDir[i];
   sComp->dir |= (UINT16)RadixNToTen(cornerDir,3, NCORNERCUBELETS);
   //printf("%u\n",sComp->dir);
   //getchar();
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
void PrintCompState( rbCompState * s )
{
   printf("state \n");
   printf("edgePos = %d  ",s->edgePos);
   printf("cornerPos = %d \n",s->cornerPos);
   printf("dir = %d \n",s->dir);
   
}
void GetChildComp( rbCompState * parent, rbCompState * child, UINT8 move )
{
   rbCubeState parentFull=NULL_STATE,childFull=NULL_STATE;

   GetRBStateFullFromComp(parent,&parentFull);
   GetChild(&parentFull,&childFull, move);
   GetRBStateCompFromFull(&childFull,child);
   //PrintCompState(child);
}
void GetChildComp1( rbCubeState * parent, rbCompState * child, UINT8 move )
{
   rbCubeState childFull=NULL_STATE,childFull1=NULL_STATE;

   GetChild(parent,&childFull, move);
   //GetChild(&childFull,&childFull1, move);
   //printf("GetChildComp1()\n");
   //PrintFull(&childFull);
   GetRBStateCompFromFull(&childFull,child);
   //PrintCompState(child);
}
Boolean IsCompStateEqual( rbCompState * s1, rbCompState * s2 )
{
   if( s1->dir != s2->dir )
      return FALSE;
   if( s1->edgePos != s2->edgePos )
      return FALSE;
   if( s1->cornerPos != s2->cornerPos )
      return FALSE;
   return TRUE;
}
//void PrintMove(int id){}
/*
 * "dist": the number of moves to make from the Goal Node to obtain the random state
 d1 : number of double turns in the reduced graph*/
void GetRandRbRedState(rbCompState * state, UINT8 dist,int d1 )
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
    GetRBStateCompFromFull(&child,state);
    printf("random state calculated\n");
}
/*
 * "dist": the number of moves to make from the given Node to obtain the random state*/
void GetRandRbPath(rbCubeState * state, UINT8 dist, rbCompState * Path )
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
   GetRBStateCompFromFull(&parent,&Path[0]);
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
      GetRBStateCompFromFull(&child,&Path[i]);
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
void GetRandRbState(rbCompState * state, UINT8 dist )
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
   GetRBStateCompFromFull(&child,state);
   //printf("random state calculated\n");
}
#endif
#define NMOVES 12
unsigned char CompareFullStates( rbCubeState * s1, rbCubeState * s2)
{
    int i;
    for(i=0;i<NEDGECUBELETS;i++)
        if( s1->edgePos[i] != s2->edgePos[i] )
            return 0;
        else if( s1->edgeDir[i] != s2->edgeDir[i] )
            return 0;
    for(i=0;i<NCORNERCUBELETS;i++)
        if( s1->cornerPos[i] != s2->cornerPos[i] )
            return 0;
        else if( s1->cornerDir[i] != s2->cornerDir[i] )
            return 0;      
    return 1;
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
       Sdest->edgePos[i] = Ssource->edgePos[SfromGoal->edgePos[i]];
	   if (SfromGoal->edgeDir[i]==0)
	      Sdest->edgeDir[i] = Ssource->edgeDir[SfromGoal->edgePos[i]];
	   else
	      Sdest->edgeDir[i] = !Ssource->edgeDir[SfromGoal->edgePos[i]];
    }
	for(i=0;i<NCORNERCUBELETS;i++)
	{
	   Sdest->cornerPos[ i ] = Ssource->cornerPos[ SfromGoal->cornerPos[i] ];
	   Sdest->cornerDir[i] = CornerDirMap[(i/4)!=(SfromGoal->cornerPos[i]/4 )][SfromGoal->cornerDir[i]][Ssource->cornerDir[SfromGoal->cornerPos[i]]];
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
	    if (s->cornerPos[i] == pos)
		    return s->cornerDir[i];
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
	s->cornerDir[pos] = dir;
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
		if(s1.cornerDir[i] != sr3New.cornerDir[i])
		{
			printf("cd mismatch i=%d,pos=%d sdir=%d, sr3Newdir=%d\n",i,s1.cornerPos[i],s1.cornerDir[i],sr3New.cornerDir[i]);
		}
	}


}

/*The list of moves given is the list of color layers. Each move is made twice
as we need to traverse in the reduced Graph*/
void GetnMoveState(int MoveSeq[], int numMoves, rbCubeState * s1)
{
    rbCubeState s=GOAL_STATE;
    int i;
    for(i=0;i<numMoves;i++)
    {
        GetChild(&s,s1,MoveSeq[i]*2);
        s = *s1;
        GetChild(&s,s1,MoveSeq[i]*2);
        PrintFull(s1);
        s = *s1;
    }
    //PrintFull(s1);
    printf("\n");
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
    Check5();
	//Check2();
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
