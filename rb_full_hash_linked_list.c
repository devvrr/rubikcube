/*Generation of nodes in the full state space graph of the Rubik cube by using
 * a five tier hash table to speed up the look up process while checking if a newly generated
 * node is already present in the graph or not
 * The five tiers of the hash table are the 
 * 1. the corner cube let direction value
 * 2. edge cubelet direction value
 * 3, 4. the first two subgroups of edge cubelets(the twelve edge cubelets are divided into
 * three combinations of four each. The first two subgroups uniquely identify the third.
 * Hence there is no need to use the third group in the hashing )
 * 5. The first subgroup of the corner cubelets(the eight corner cubelets are divided into two
 * subgroups(combinations) of four each. The second subgroup is uniquely determined by the
 *  first )
 *
 * These "sub groups" are divided based on the fact that when only double turns are made
 * in the Rubik cube, the (edge,corner)cubelets move within the subgroups.
 * 
 *  These five parameters actually identify a "Reduced graph"
 *  A "Reduced Graph" is the graph obtained when only doble turns are allowed in the Rubik cube.
 *
 *  The actual nodes of the Graph are sequentially stored in an array, after each node is
 *  checked to see if it is already present in the Graph generated so far.
 *
 * This program limits the generation of nodes by the "PARENT_COUNT" macro defined below.
 * 
 * This program also takes(or randomly generates) a problem node and tries to generate a tree
 * from the "problem node" upto a certain depth(or until a match node is found in the first
 * Graph, which ever is earlier), and thus tries to find an optimal path from the
 * "problem node" to the "Goal node". Here, actually the "match" need not be an exact
 * node match. If two nodes are in the same "Reduced Graph", it is enough to find a path from
 * the "problem node" to the "Goal node".
 *
 * (As a "Reduced Graph" contains only some 6 lakh something nodes, finding a path 
 * through it between two nodes is easy, for any two nodes in the same Reduced Graph )
 * 
 * It is found(using this program) that such Graphs can be generated upto depth
 * 6(due to memory size and algorithm speed constraints). Hence for nodes upto
 * distance 6+6=12 an optimal path to Goal can be found
 * using this method. 
 *
 * With some luck, even for nodes more than distance(from the Goal node) 12, matches should
 * have been found, as there should some chance of finding two nodes, one from each Graph, that
 * belong to the same "Reduced Graph". But such matches were *NOT* found.
 * If such a match(through same reduced graph) is found, there is no guaranty that the path
 * found will be optimal. But chances are high that it is optimal(only a guess!).
 *
 * This expectation was there, since the diameter of the "Reduced Graph" is
 * 15(double turns and hence 30 single turns).(The structure of all "Reduced Graphs"
 * is same.)
 * 
 * A further improvement that can be thought of, over this method is that, with the help
 * of the "difference" function we may be able to find optimal paths for nodes upto
 * to distance 18(from the Goal node)
 *
 * The "difference" of two Rubik cube configurations(nodes in our Graph) s1, s2 can be
 * defined to be the config "diff" which has the same relative placement of
 * cubelets(relative to the Goal node) in it, as s2 is relative to s1.
 *
 * OR
 *
 * The sequence of moves needed to go from s2 to s1 is same as the sequence of moves
 * needed to go from "diff" to Goal node.
 *
 *
 * In this program, the array to store the nodes is different for the Goal graph and the
 * problem graph, but the hash table(5 tier data structure) is common.
 * */
#include <stdio.h>
#include <time.h>
#include "rbdefs.h"
#include "rbmath.h"
#include "child_gen/rbutils.h"
#include "diff/diff.h"
#include "user_desc/convert_user_form.h"
#define NODE_COUNT 26873856
//#define NODE_COUNT 9839380
//#define NODE_COUNT 9839380//1296
//#define NODE_COUNT 17000
#define PARENT_COUNT6 105047//128
#define PARENT_COUNT1 1
#define PARENT_COUNT7 983938
int PARENT_COUNT;
int ParentCount[9]=
{
/*0*/1,
/*1*/13,
/*2*/127,
/*3*/1195,
/*4*/11206,
/*5*/105047,
/*6*/983927,
/*7*/9205558,
/*8*/90055580
};
#define POWER3_8 6561/*3 to the power of 8*/
#define RB_NEW(x) ((x*)malloc(sizeof(x)))

#define MOVE_MASK(x) (0x0001<<x)
#define INVERSE_MOVE(move) (move ^ 0x01)
#define BAD_INDEX (-1)
#define UPDATE_YES_MOVES(g,x,y) g[x].yesMoves|=MOVE_MASK(INVERSE_MOVE(y))

/*values for "goalFlag" the possible problem states */
#define GOAL_NODE 0
#define GOAL_GRAPH 1
#define PROBLEM_NODE 2
#define PROBLEM_GRAPH 3
#define FINDING_DIFFS 4
#define DEBUG_N 1

/*epg1cnt=3613834
 epg2cnt=3769263
 cpg1cnt=3827147
 edircnt=2472827*/

typedef unsigned char BOOLEAN;


typedef struct{
   rbCompState state;
   UINT16 yesMoves; /*12 bits used: iTh(from the right; right most bit is the LSB ) bit
		      is 1, if a neighbour with that move is already
   present in the graph dicovered so far. Otherwise it is zero*/
   UINT8 parentMove;/*move made at the parent to come to this state(0..11)*/
   UINT8 level;
//   UINT8 redGraphGroup;
   INT32 nodeNext;
   INT32 parentIndex;
   UINT8 firstInRedGr;
   UINT8 RedGroup;
   int RedGraph;
} FinalNode;


/*corner position group1*/
struct CPG1_tag{
   INT32 finalNode; /*index of a node in the Array of nodes of Goal Graph*/
   INT32 finalNodePr;/*index of node in problem graph*/
   struct CPG1_tag * cpg1Next;
   UINT8 cpg1Id; /*0..69*/
   UINT32 hitCount;/*no of nodes stored under this hash value */
   UINT8 graph;/*0 - goal graph, 1 problem graph, 2 - both*/
};

typedef struct CPG1_tag CPG1_t;

struct EPG2_tag{
   UINT16 epg2Id; /*0..69 ( 8C4 - 1 )*/ /*12C4 now*/
   struct EPG2_tag * epg2Next;
   CPG1_t * cpg1Ptr;
};

typedef struct EPG2_tag EPG2_t;

struct EPG1_tag{
   UINT16 epg1Id;/*0..494*/
   struct EPG1_tag * epg1Next;
   EPG2_t * epg2Ptr;
};

typedef struct EPG1_tag EPG1_t;
   
struct ED_tag{
   UINT16 edId;/*0..4095*/
   struct ED_tag * edNext;
   EPG1_t * epg1Ptr;
};

typedef struct ED_tag ED_t;


static rbCompState GoalState = {0,0,0};
//static rbCompState GoalState = {244671840,29102,174069472};
//static FinalNode Goal = {{0,0,0},0,0,0,BAD_INDEX,BAD_INDEX};/*actually the parentMove and
//						parentIndex for the Goal node are undefined.
//					       Nobody is going to use them. They are used now*/

//static FinalNode Graph1[NODE_COUNT];
static FinalNode * Graph1;
//static FinalNode Graph[NODE_COUNT];
static FinalNode * Graph;


static UINT32 curGraphNodePtr=0;
static UINT32 curParentPtr;

static UINT32 curGraph1NodePtr=0;
static UINT32 curParent1Ptr;
static int gFindLevel=0;
static int nLevelsExpl;
static int cpg1cnt;
static int epg1cnt;
static int epg2cnt;
static int edircnt;

typedef struct {
   UINT16 CD;/*0..3^8 -1*/
   UINT16 ED;/*0..2^12 - 1*/

   /*There are three edge positon groups*/
   UINT16 EPG1;/*0..494(12C4-1)*/
   UINT16 EPG2;/*0..69(8C4-1)*/ /*upto 12C4 now*/

   /*There are two corner position groups*/
   UINT8 CPG1;/*0..69(8C4-1)*/

} rbIndices;

/*Actually, only 3**7 entries of this table will be used, because there are only that many
 * possible values for the corner cubelet direction value index*/
static ED_t * CDTable[POWER3_8];


static FinalNode curNode;
static rbIndices curStateHashIndex;

static UINT8 goalFlag=GOAL_NODE;

static rbCompState problemTreeMatchState;
static rbCompState goalTreeMatchState;
static rbCompState searchState;
static int goalTreeMatchStateIndex;
static int goalTreeMatchStateCount=0;
static int redGraphCount;
static int matchCount;
static UINT8 redGraphPath[30],redGraphPathLength=0;

void FindRedGraphPath(void);
void CalcPaths(void);
void PrintPathToGoal(int id,int Path[],int* ln);
int PathGoal[50],PathRed[30],PathProb[50],plnGoal,plnRed,plnProb;
void PrintCompState( rbCompState * s );
//unsigned char GetRedGraphGroupId(rIndex id);
//int GetRedGraphPath(rIndex problemId,rIndex goalId,UINT8 * path, UINT8 * len);

void PrintFinalNode( FinalNode * node )
{
   printf("The final node is...\n");
   PrintCompState(&(node->state));
   printf("yesMoves=%d  ",node->yesMoves);
   printf("parentMove=%d  ",node->parentMove);
   printf("level=%d\n",node->level);
   printf("nodeNext=%d  ",node->nodeNext);
   printf("parentIndex=%d\n",node->parentIndex);
}
void PrintArray(UINT8 * a, UINT8 n)
{
   UINT8 i;
   printf("Array is:   ");
   for(i=0;i<n;i++)
      printf("%4d",a[i]);
   printf("\n");
}


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
void GetD5Index( rbCompState * state, UINT8 id[5])
{
   UINT8 edgePos[NEDGECUBELETS],cornerPos[NCORNERCUBELETS];

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
}

/*which group*/
UINT16 GetEdgePositionGroup1(UINT8 * edgePos)
{
   UINT8 comb[4];
   comb[0] = edgePos[green_red];
   comb[1] = edgePos[red_blue];
   comb[2] = edgePos[green_pink];
   comb[3] = edgePos[pink_blue];
   return GetIndexFromComb(comb,12,4);
}
UINT16 GetEdgePositionGroup2(UINT8 * edgePos)
{
   UINT8 comb[4];
   comb[0] = edgePos[blue_yellow];
   comb[1] = edgePos[yellow_green];
   comb[2] = edgePos[green_white];
   comb[3] = edgePos[white_blue];

   /*Actually this can have only 8C4 possibilities. But it is an unnecessary processing
    * overhead to convert the 12Combination(combination of 12 objects) to an 8Combination,
    * before we can find out the index of the 8Combination. Resultantly we have to use
    * a UINT16 to store this index instead of a UINT8 for an 8Combination
    *
    * Let up take up this enhancement in phase 2*/
   return GetIndexFromComb(comb,12,4);/*problem*/
}
UINT8 GetCornerPositionGroup1(UINT16 cornerPosId)
{
	UINT8 cornerPos[NCORNERCUBELETS];
	UINT8 comb[4];

	GetPermFromIndex(cornerPos,cornerPosId,NCORNERCUBELETS);


	if( curGraphNodePtr < DEBUG_N )
	{
		printf("corner pos is: ");
		PrintArray(cornerPos,NCORNERCUBELETS);
	}
	comb[0] = cornerPos[pink_yellow_green];
	comb[1] = cornerPos[green_white_red];
	comb[2] = cornerPos[pink_white_blue];
	comb[3] = cornerPos[blue_red_yellow];
	return GetIndexFromComb(comb,8,4);
}
/*
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
*/
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
int FinPath[30];
int FinPath1[30];
int pathidx=0;
int pathidx1=0;
/* -1 < move < 12 */
void PushMove1( int move)
{
	FinPath1[pathidx1++] = move;
}
int PopMove1()
{
	if(pathidx1>0)
	{
		pathidx1--;
		return FinPath1[pathidx1];
	}
	else
		return 12;
}
void PrintPathToProblemNode(int id)
{
	int move,len=0;
	printf("path to problem node is:\n");
	while(id != 0)
	{
		PrintMove(Graph1[id].parentMove);
		PushMove1(Graph1[id].parentMove);
		id = Graph1[id].parentIndex;
	}
	printf("\n");

	printf("**********************************************************************\n");
	printf("The final solution is:\n");
	move=PopMove1();
	while(move<12)
	{
		PrintMove(move);
		len++;
		move=PopMove1();
	}
	printf("\npath length: %d",len);
	printf("\n**********************************************************************\n");
}
void PushMove( int move)
{
printf("pathidx=%d\n",pathidx);
	FinPath[pathidx++] = move;
}
int PopMove()
{
	if(pathidx>0)
	{
		pathidx--;
		return FinPath[pathidx];
	}
	else
		return 12;
}
void PrintPathToGoal(int id,int PathArr[],int* ln)
{	
	int move;
	printf("path to goal node from index:%d is:\n",id);
	while(id != 0)
	{
        printf("ln=%d\n",*ln);
		PrintMove(Graph[id].parentMove);
        printf("ln=%d\n",*ln);
		PushMove(INVERSE_MOVE(Graph[id].parentMove));
        printf("ln=%d\n",*ln);
		PathArr[(*ln)++] = INVERSE_MOVE(Graph[id].parentMove);
        printf("ln=%d\n",*ln);
		PushMove(INVERSE_MOVE(Graph[id].parentMove));
		id = Graph[id].parentIndex;
        printf("id=%d\n",id);
	}
	printf("\n");
	move = PopMove();
	while( move < 12 )
	{
		PushMove1(move);
		move = PopMove();
	}
}


void PrintSubTableIndices(rbIndices * index5)
{
   printf("sub table indices:\n");
   printf("Corner Direction index:%d\n",index5->CD);
   printf("Edge Direction index:%d\n",index5->ED);
   printf("Edge Position index1:%d\n",index5->EPG1);
   printf("Edge Position index2:%d\n",index5->EPG2);
   printf("Corner Position index:%d\n",index5->CPG1);
}
void CalculateSubTableIndices(rbCompState * state, rbIndices * indices)
{
   UINT8 edgePos[NEDGECUBELETS];
   
   indices->CD = GET_CORNER_DIR(state->dir);
   indices->ED = GET_EDGE_DIR(state->dir);

   GetPermFromIndex(edgePos,state->edgePos,NEDGECUBELETS);
   //if( curGraphNodePtr < DEBUG_N )
    //  PrintArray(edgePos,NEDGECUBELETS);
   indices->EPG1 = GetEdgePositionGroup1(edgePos);
   indices->EPG2 = GetEdgePositionGroup2(edgePos);

   indices->CPG1 = GetCornerPositionGroup1(state->cornerPos);


  // if( curGraphNodePtr < DEBUG_N )
  // {
   //   printf("#######\n");
      //PrintCompState(state);
    //  printf("cd:%4d, ed:%4d, epg1:%4d epg2:%4d, cpg1:%4d\n",indices->CD,indices->ED,
//	    indices->EPG1,indices->EPG2,indices->CPG1);
    //  printf("cur index = %d, red group = %d\n\n",curGraphNodePtr,indices->redGroup);

//      printf("\n\n");
  // }
}

void FindRedGraphCount(void);
void AddFinalNodeToProblemGraph(void)
{
   static UINT8 curLevel=0;
   static INT32 levelCount=0;
   Graph1[curGraph1NodePtr].state = curNode.state;/*node contents - final state;structure copy*/
   Graph1[curGraph1NodePtr].nodeNext = BAD_INDEX;
   Graph1[curGraph1NodePtr].yesMoves = 0;
//printf("AddFinalNodeToProblemGraph\n");
   if(goalFlag == PROBLEM_GRAPH)
   {
      Graph1[curGraph1NodePtr].parentMove = curNode.parentMove;
      UPDATE_YES_MOVES(Graph1,curGraph1NodePtr,curNode.parentMove );
      Graph1[curGraph1NodePtr].parentIndex = curNode.parentIndex;
      Graph1[curGraph1NodePtr].level = Graph1[curNode.parentIndex].level + 1;
   }
   else if(goalFlag == PROBLEM_NODE)
   {
      Graph1[curGraph1NodePtr].level = 0;
   }
   if( Graph1[curGraph1NodePtr].level > curLevel )
   {
      printf("level %d count = %d\n",curLevel,levelCount);
	  //FindRedGraphCount();
      fflush(stdin);
      levelCount=0;
      curLevel++;
      nLevelsExpl = curLevel;
   }
   levelCount++;

   curGraph1NodePtr++;
   if(curGraph1NodePtr > NODE_COUNT )
   {
	   printf("cur graph1 node ptr too large\n");
	   exit(0);
   }
}
void CheckIndices( void )
{
   //printf("check indices\n");
   if( curStateHashIndex.CD == 0/*5856*/ &&
	 curStateHashIndex.ED == 0/*2656*/ &&
	 curStateHashIndex.EPG1 == 0/*208*/ &&
	 curStateHashIndex.EPG2 == 0/*55*/ &&
	 curStateHashIndex.CPG1 == 0/*58*/)
   {
      printf("match node = %d \n",curGraphNodePtr);
   }
}
/*No parameters; The global variable 'curNode' contains the new state to be inserted */
void AddFinalNodeToGoalGraph(void)
{
   static UINT8 curLevel=0;
   static INT32 levelCount=0;
   Graph[curGraphNodePtr].state = curNode.state;/*node contents - final state;structure copy*/

   Graph[curGraphNodePtr].nodeNext = BAD_INDEX;

   UINT8 d5IndexProblemTreeMatchState[5];
   //printf("FindRedGraphPath_func1\n");
   GetD5Index(&curNode.state,d5IndexProblemTreeMatchState);

   Graph[curGraphNodePtr].firstInRedGr = 2;
   Graph[curGraphNodePtr].RedGroup = GetRedGraphGroupId(d5IndexProblemTreeMatchState);
   Graph[curGraphNodePtr].yesMoves = 0;
   PRINT_TEXT_1INT("AddFinalNodeToGoalGraph, curGraphNodePtr=%d\n",curGraphNodePtr);
   if( goalFlag == GOAL_GRAPH )
   {
      Graph[curGraphNodePtr].parentMove = curNode.parentMove;
      UPDATE_YES_MOVES(Graph,curGraphNodePtr,curNode.parentMove );
      Graph[curGraphNodePtr].parentIndex = curNode.parentIndex;
      Graph[curGraphNodePtr].level = Graph[curNode.parentIndex].level + 1;
      PRINT_TEXT_1INT("cur level=%d\n",Graph[curGraphNodePtr].level);
      PRINT_TEXT_1INT("parent level=%d\n",Graph[curNode.parentIndex].level);
   }
   else if( goalFlag == GOAL_NODE )
   {
      Graph[curGraphNodePtr].level = 0;
      PRINT_TEXT("goal node\n");
   }
   if( Graph[curGraphNodePtr].level > curLevel )
   {
      printf("level %d count = %6d  \n",curLevel,levelCount);
	  //FindRedGraphCount();
      levelCount=0;
      curLevel++;
      nLevelsExpl = curLevel;
   }
   levelCount++;
   if( curGraphNodePtr < DEBUG_N )
   {
      PrintPathToGoal(curGraphNodePtr,PathGoal,&plnGoal);
      printf("******* end ****\n\n");
   }
   curGraphNodePtr++;
   CheckIndices();
   if(curGraphNodePtr > NODE_COUNT )
   {
	   printf("cur graph node ptr too large\n");
	   exit(0);
   }
}

void AddNodeCPG1Middle(CPG1_t * cpg1NodePrevPtr, CPG1_t * cpg1NodeNextPtr )
{
   CPG1_t * tmpCpg1=0;
   tmpCpg1 = RB_NEW(CPG1_t);
   PRINT_TEXT("AddNodeCPG1Middle()\n");
   if( !tmpCpg1 )
   {
      printf("could not allocate memory for cpg1\n");
      exit(0);
   }
   cpg1cnt++;
   cpg1NodePrevPtr->cpg1Next = tmpCpg1;/*parent points to child*/
   tmpCpg1->cpg1Id = curStateHashIndex.CPG1;/*the level index for the node*/
   tmpCpg1->cpg1Next = cpg1NodeNextPtr;/*The next in the same level*/
      
   if(goalFlag == GOAL_GRAPH || goalFlag == GOAL_NODE )
   {
      tmpCpg1->graph = GOAL_GRAPH;
      tmpCpg1->finalNode = curGraphNodePtr;/*parent points to child*/
      AddFinalNodeToGoalGraph();
   }
   else
   {
      tmpCpg1->graph = PROBLEM_GRAPH;
      tmpCpg1->finalNode = curGraph1NodePtr;/*parent points to child*/

      AddFinalNodeToProblemGraph();
   }
}

void AddNodeCPG1Begin(EPG2_t * parentEpg2, CPG1_t * cpg1NodeNextPtr )
{
   CPG1_t * tmpCpg1=0;
   tmpCpg1 = RB_NEW(CPG1_t);
   PRINT_TEXT("AddNodeCPG1Begin()\n");
   if( !tmpCpg1 )
   {
      printf("could not allocate memory for cpg1\n");
      exit(0);
   }
   cpg1cnt++;
   parentEpg2->cpg1Ptr = tmpCpg1;/*parent points to child*/
   tmpCpg1->cpg1Id = curStateHashIndex.CPG1;/*the level index for the node*/
   tmpCpg1->cpg1Next = cpg1NodeNextPtr;/*The next in the same level*/
      
   if(goalFlag == GOAL_GRAPH || goalFlag == GOAL_NODE )
   {
      tmpCpg1->graph = GOAL_GRAPH;
      tmpCpg1->finalNode = curGraphNodePtr;/*parent points to child*/
      AddFinalNodeToGoalGraph();
   }
   else
   {
      tmpCpg1->graph = PROBLEM_GRAPH;
      tmpCpg1->finalNode = curGraph1NodePtr;/*parent points to child*/

      AddFinalNodeToProblemGraph();
   }

}

void AddNodeEPG2Middle(EPG2_t * epg2NodePrevPtr, EPG2_t * epg2NodeNextPtr )
{
   EPG2_t *tmpEpg2=0;
   PRINT_TEXT("AddNodeEPG2Middle()\n");
   tmpEpg2 = RB_NEW(EPG2_t);
   if( !tmpEpg2 )
   {
      printf("could not allocate memory for epg2\n");
      exit(0);
   }
   epg2cnt++;
   epg2NodePrevPtr->epg2Next = tmpEpg2;/*parent points to child*/
   tmpEpg2->epg2Id = curStateHashIndex.EPG2;/*the level index for the node*/
   tmpEpg2->epg2Next = epg2NodeNextPtr;/*The next in the same level*/
   AddNodeCPG1Begin(tmpEpg2,NULL);
}

void AddNodeEPG2Begin(EPG1_t * parentEpg1, EPG2_t * epg2NodeNextPtr )
{
   EPG2_t *tmpEpg2=0;
   PRINT_TEXT("AddNodeEPG2Begin()\n");
   tmpEpg2 = RB_NEW(EPG2_t); 
   if( !tmpEpg2 )
   {
      printf("could not allocate memory for epg2\n");
      exit(0);
   }
   epg2cnt++;
   parentEpg1->epg2Ptr = tmpEpg2;/*parent points to child*/
   tmpEpg2->epg2Id = curStateHashIndex.EPG2;/*the level index for the node*/
   tmpEpg2->epg2Next = epg2NodeNextPtr;/*The next in the same level*/
   AddNodeCPG1Begin(tmpEpg2,NULL);
}

void AddNodeEPG1Middle(EPG1_t * epg1NodePrevPtr, EPG1_t * epg1NodeNextPtr )
{
   EPG1_t *tmpEpg1=0;
   tmpEpg1 = RB_NEW(EPG1_t);
   PRINT_TEXT("AddNodeEPG1Middle()\n");
   if( !tmpEpg1 )
   {
      printf("could not allocate memory for epg1\n");
      exit(0);
   }
   epg1cnt++;
   epg1NodePrevPtr->epg1Next = tmpEpg1;/*parent points to child*/
   tmpEpg1->epg1Id = curStateHashIndex.EPG1;/*the level index for the node*/
   tmpEpg1->epg1Next = epg1NodeNextPtr;/*The next in the same level*/
   AddNodeEPG2Begin(tmpEpg1,NULL);
}

void AddNodeEPG1Begin(ED_t * parent, EPG1_t * epg1NodeNextPtr )
{
   EPG1_t *tmpEpg1=0;
   tmpEpg1 = RB_NEW(EPG1_t);
   PRINT_TEXT("AddNodeEPG1Begin()\n");
   if( !tmpEpg1 )
   {
      printf("could not allocate memory for epg1\n");
      exit(0);
   }
   epg1cnt++;
   parent->epg1Ptr = tmpEpg1;/*parent points to child*/
   tmpEpg1->epg1Id = curStateHashIndex.EPG1;/*the level index for the node*/
   tmpEpg1->epg1Next = epg1NodeNextPtr;/*The next in the same level*/

   AddNodeEPG2Begin(tmpEpg1,NULL);
}

void AddNodeEdgeDirMiddle(ED_t * edgeDirNodePrevPtr, ED_t * edgeDirNodeNextPtr )
{
   ED_t * newEdgeDirNode = RB_NEW(ED_t);
   PRINT_TEXT("AddNodeEdgeDirMiddle()\n");
   if( !newEdgeDirNode )
   {
      printf("could not allocate memory for ed node\n");
      exit(0);
   }
   edircnt++;
   newEdgeDirNode->edId = curStateHashIndex.ED;
   newEdgeDirNode->edNext = edgeDirNodeNextPtr;
   edgeDirNodePrevPtr->edNext = newEdgeDirNode;

   AddNodeEPG1Begin(newEdgeDirNode,NULL);
}

/*add in the beginning of edge direction linked list*/
void AddNodeEdgeDirBegin(ED_t * edgeDirNodeNextPtr )
{
   ED_t * newEdgeDirNode = RB_NEW(ED_t);
   PRINT_TEXT("AddNodeEdgeDirBegin()\n");
   if( !newEdgeDirNode )
   {
      printf("could not allocate memory for ed node\n");
      exit(0);
   }
   edircnt++;
   newEdgeDirNode->edId = curStateHashIndex.ED;
   newEdgeDirNode->edNext = edgeDirNodeNextPtr;
   CDTable[curStateHashIndex.CD] = newEdgeDirNode;
   
   AddNodeEPG1Begin(newEdgeDirNode,NULL);
}
void InsertProblemGraph(CPG1_t * cpg1Ptr )
{
	INT32 index = cpg1Ptr->finalNode;
	INT32 indexPrev=BAD_INDEX;
	int x=0;

	/*The first index will not be */
	if(index == BAD_INDEX )
		printf("error: first index -1 \n");

	while( index != BAD_INDEX )
	{
		x++;
		if( x > 1000 )
		{
			printf("while loop too long InsertProblemGraph\n");
			getchar();
		}
		if(IsCompStateEqual(&(Graph1[index].state),&(curNode.state)))
		{
			UPDATE_YES_MOVES(Graph1,index,curNode.parentMove);
			return;
		}
		else
		{
			indexPrev = index;
			index = Graph1[index].nodeNext;
		}
	}
	if(indexPrev == BAD_INDEX )
		printf("error: indexPrev cannot be bad\n");

	Graph1[indexPrev].nodeNext = curGraph1NodePtr;
	//printf("InsertProblemGraph");
	AddFinalNodeToProblemGraph();
}
BOOLEAN Find(rbCompState * state);
/*return 2 for found in red graph*/
BOOLEAN FindGraph(CPG1_t * cpg1Ptr )
{
    int found=0;
	INT32 index = cpg1Ptr->finalNode;
	INT32 indexPrev=BAD_INDEX;
    rbCubeState stFull,diff,st1Full;
    rbCompState diffComp;
	int x=0;
	PRINT_TEXT_1INT("FindGraph(), index=%d\n",index);
	/*The first index will not be */
	if(index == BAD_INDEX )
		printf("error: first index -1 \n");
    else
    {
        //printf("path to same red graph from index=%d\n",index);
        //PrintPathToGoal(index);
    }

	while( index != BAD_INDEX )
    {
        UINT8 searchStateRedGroup;
        x++;
        if( x > 1000 )
        {
            printf("while loop too long FindGraph\n");
            getchar();
        }
        //PrintPathToGoal(index);
        //PrintCompState(&(Graph[index].state));
        //PrintCompState(&(Graph[index].state));
        UINT8 d5IndexProblemTreeMatchState[5];
        //printf("%s\n",__func__);
        GetD5Index(&searchState,d5IndexProblemTreeMatchState);

        searchStateRedGroup = GetRedGraphGroupId(d5IndexProblemTreeMatchState);
        if (/*(Graph[index].RedGroup == searchStateRedGroup) &&*/
                IsCompStateEqual(&(Graph[index].state),&(searchState))/*this condition is redundant*/
           )
        {
            problemTreeMatchState = searchState;
            goalTreeMatchState = Graph[index].state;
            //FindRedGraphPath();
            GetRBStateFullFromComp( &(Graph[index].state), &stFull);
            //TODO PrintFullN(&stFull,"match state Full");
            GetRBStateFullFromComp( &(searchState), &st1Full);
            //TODO PrintFullN(&st1Full,"search State Full");
            printf("Exact Match found in function %s at index=%d!!!\n",__func__,index);
            //PrintPathToGoal(index,gFindLevel?PathRed:PathGoal,gFindLevel?(&plnRed):(&plnGoal));
            PrintPathToGoal(index,PathGoal,&plnGoal);
            return TRUE;
        }
        else
        {
            indexPrev = index;
            index = Graph[index].nodeNext;
            PRINT_TEXT_1INT("function Find, new index=%d\n",index);
        }
    }
	if(indexPrev == BAD_INDEX )
    {
		printf("error: indexPrev cannot be bad\n");
        return FALSE;
    }

    if (gFindLevel >= 1)
        return FALSE;

    gFindLevel++;
	/*GetRBStateFullFromComp( &(Graph[ cpg1Ptr->finalNode ].state), &stFull);
	//PrintFullN(&stFull,"match state Full");
	RbCubeConfigDiff(&st1Full, &stFull, &diff);
	//PrintFullN(&diff,"diff");
	GetRBStateCompFromFull(&diff, &diffComp);
    printf("calling Find for diff findlevel:%d\n",gFindLevel);
    searchState = diffComp;
	found = Find(&diffComp); 
	if( found )
    {
        printf("Exact Match Found i=%d\n",cpg1Ptr->finalNode);
        PrintPathToGoal(cpg1Ptr->finalNode,PathGoal,&plnGoal);
        //PrintPathToProblemNode(j);
        return found;
    }*/


	PRINT_TEXT_1INT("FindGraph, last index found=%d\n",indexPrev);
	return FALSE;
}
void InsertGraph(CPG1_t * cpg1Ptr )
{
	INT32 index = cpg1Ptr->finalNode;
	INT32 indexPrev=BAD_INDEX;
	int x=0;
	PRINT_TEXT_1INT("InsertGraph(), index=%d\n",index);
	/*The first index will not be */
	if(index == BAD_INDEX )
		printf("error: first index -1 \n");

	while( index != BAD_INDEX )
	{
		x++;
		if( x > 100000 )
		{
			printf("while loop too long InsertGraph\n");
			getchar();
		}
		if(IsCompStateEqual(&(Graph[index].state),&(curNode.state)))
		{
			UPDATE_YES_MOVES(Graph,index,curNode.parentMove);
			//printf("yes move found\n");
			return;
		}
		else
		{
			indexPrev = index;
			index = Graph[index].nodeNext;
			PRINT_TEXT_1INT("function InsertGraph, new index=%d\n",index);
		}
	}
	if(indexPrev == BAD_INDEX )
		printf("error: indexPrev cannot be bad\n");

	Graph[indexPrev].nodeNext = curGraphNodePtr;

	PRINT_TEXT_1INT("InsertGraph, last index found=%d\n",indexPrev);
	AddFinalNodeToGoalGraph();
}
void RB_ExitProper(void)
{
   int i;
   CalcPaths();
   printf("kswjehfksdhfksdhfksjdhfkjhdf\nskdjfhskdjhfksjdfh\n");
   for(i=0;i<172361;i++)
      ;
   //exit(0);
}

void InsertCpg1(EPG2_t * parent )
{
	int x=0;
	UINT16 cpg1Id = curStateHashIndex.CPG1;
	CPG1_t * cpg1NodePtr = parent->cpg1Ptr;
	PRINT_TEXT("InsertCpg1()\n");
	if( cpg1Id < cpg1NodePtr->cpg1Id )
	{
		AddNodeCPG1Begin( parent, cpg1NodePtr );
		return;
	}
	else if( cpg1Id == cpg1NodePtr->cpg1Id )
	{
		if( goalFlag == GOAL_GRAPH || goalFlag == GOAL_NODE)
		{
			InsertGraph(cpg1NodePtr);
			return;
		}
		else if(goalFlag == PROBLEM_GRAPH || goalFlag == PROBLEM_NODE)
		{
			if(cpg1NodePtr->graph == PROBLEM_GRAPH)
			{
				InsertProblemGraph(cpg1NodePtr);
				return;
			}
			else	 /*NodePtr->graph == GOAL_GRAPH*/
			{
				rbIndices index5;
				goalTreeMatchStateIndex = cpg1NodePtr->finalNode;
#if 1
				printf("match found in else\n");
				printf("goal tree match index=%d\n",cpg1NodePtr->finalNode);
				PrintPathToGoal(cpg1NodePtr->finalNode,PathProb,&plnProb);
				printf("problem tree match index=%d\n",curGraph1NodePtr);
				CalculateSubTableIndices(&(Graph[cpg1NodePtr->finalNode].state),&index5);
				AddFinalNodeToProblemGraph();
				PrintPathToProblemNode(curGraph1NodePtr-1);
				PrintCompState(&(Graph[cpg1NodePtr->finalNode].state));
				printf("match node\n");
				PrintSubTableIndices(&index5);
				printf("problem node graph\n");
				PrintSubTableIndices(&curStateHashIndex);
				printf("match node level = %d\n",Graph[cpg1NodePtr->finalNode].level);

				problemTreeMatchState = curNode.state;
				goalTreeMatchState = Graph[cpg1NodePtr->finalNode].state;

				printf("Goal tree match state:\n");
				PrintCompState(&(goalTreeMatchState));
				printf("Problem tree match state:\n");
				PrintCompState(&(goalTreeMatchState));
				printf("**********************************************\n");
				/*begin - for doing match count*/
				matchCount++;
				//  curGraph1NodePtr++;
				return;
				/*end - for doing match count*/
				//RB_ExitProper();
#endif
			}
		}
		InsertGraph(cpg1NodePtr);//continue search in the next dimension
		return;
	}
	else
	{
		CPG1_t * prevCpg1NodePtr = cpg1NodePtr;
		cpg1NodePtr = cpg1NodePtr->cpg1Next;
		while(cpg1NodePtr)
		{
			x++;
			if( x > 100 )
			{
				printf("while loop too long InsertCpg1\n");
				getchar();
			}
			if( cpg1Id < cpg1NodePtr->cpg1Id )
			{
				AddNodeCPG1Middle( prevCpg1NodePtr, cpg1NodePtr);
				return;
			}
			else if( cpg1Id == cpg1NodePtr->cpg1Id )
			{
				if( goalFlag == GOAL_GRAPH || goalFlag == GOAL_NODE)
				{
					InsertGraph(cpg1NodePtr);
					return;
				}
				else if(goalFlag == PROBLEM_GRAPH || goalFlag == PROBLEM_NODE)
				{
					if(cpg1NodePtr->graph == PROBLEM_GRAPH)
					{
						InsertProblemGraph(cpg1NodePtr);
						return;
					}
					else	 /*NodePtr->graph == GOAL_GRAPH*/
					{
						rbIndices index5;
						goalTreeMatchStateIndex = cpg1NodePtr->finalNode;

						printf("match found in while loop x=%d\n",x);
						printf("goal tree match index=%d\n",cpg1NodePtr->finalNode);
						PrintPathToGoal(cpg1NodePtr->finalNode,PathGoal,&plnGoal);
						printf("problem tree match index=%d\n",curGraph1NodePtr);
						PrintPathToProblemNode(curGraph1NodePtr);
						PrintCompState(&(Graph[cpg1NodePtr->finalNode].state));
						CalculateSubTableIndices(&(Graph[cpg1NodePtr->finalNode].state),&index5);
						printf("match node\n");
						PrintSubTableIndices(&index5);
						printf("problem node graph\n");
						PrintSubTableIndices(&curStateHashIndex);
						printf("match node level = %d\n",Graph[cpg1NodePtr->finalNode].level);

						problemTreeMatchState = curNode.state;
						goalTreeMatchState = Graph[cpg1NodePtr->finalNode].state;
						printf("Goal tree match state:\n");
						PrintCompState(&(goalTreeMatchState));
						printf("Problem tree match state:\n");
						PrintCompState(&(goalTreeMatchState));
						printf("**********************************************\n");
						/*begin - for doing match count*/
						matchCount++;
						curGraph1NodePtr++;
						return;
						/*end - for doing match count*/
						//RB_ExitProper();
					}
				}
				return;
			}
			else
			{
				prevCpg1NodePtr = cpg1NodePtr;
				cpg1NodePtr = cpg1NodePtr->cpg1Next;
			}
		}
		AddNodeCPG1Middle( prevCpg1NodePtr, NULL);
	}
}
void InsertEpg2(EPG1_t * parent )
{
	UINT16 epg2Id = curStateHashIndex.EPG2;
	EPG2_t * epg2NodePtr = parent->epg2Ptr;
	PRINT_TEXT("InsertEpg2()\n");
	if( epg2Id < epg2NodePtr->epg2Id )
	{
		AddNodeEPG2Begin( parent, epg2NodePtr );
		return;
	}
	else if( epg2Id == epg2NodePtr->epg2Id )
	{
		InsertCpg1(epg2NodePtr);//continue search in the next dimension
		return;
	}
	else
	{
		EPG2_t * prevEpg2NodePtr = epg2NodePtr;
		epg2NodePtr = epg2NodePtr->epg2Next;
		int x=0;
		while(epg2NodePtr)
		{
			x++;
			if( x > 500 )
			{
				printf("while loop too long InsertEpg2\n");
				return;
			}

			if( epg2Id < epg2NodePtr->epg2Id )
			{
				AddNodeEPG2Middle( prevEpg2NodePtr, epg2NodePtr);
				return;
			}
			else if( epg2Id == epg2NodePtr->epg2Id )
			{
				InsertCpg1(epg2NodePtr);//continue search in the next dimension
				return;
			}
			else
			{
				prevEpg2NodePtr = epg2NodePtr;
				epg2NodePtr = epg2NodePtr->epg2Next;
			}
		}
		AddNodeEPG2Middle( prevEpg2NodePtr, NULL);
	}
}
void InsertEpg1(ED_t * parent )
{
	UINT16 epg1Id = curStateHashIndex.EPG1;
	EPG1_t * epg1NodePtr = parent->epg1Ptr;
	PRINT_TEXT("InsertEpg1()\n");
	if( epg1Id < epg1NodePtr->epg1Id )
	{
		AddNodeEPG1Begin( parent, epg1NodePtr );
		return;
	}
	else if( epg1Id == epg1NodePtr->epg1Id )
	{
		InsertEpg2(epg1NodePtr);//continue search in the next dimension
		return;
	}
	else
	{
		EPG1_t * prevEpg1NodePtr = epg1NodePtr;
		epg1NodePtr = epg1NodePtr->epg1Next;
		int x=0;
		while(epg1NodePtr)
		{
			if( x > 1000 )
			{
				printf("while loop too long InsertEpg1\n");
				return;

			}
			x++;   
			if( epg1Id < epg1NodePtr->epg1Id )
			{
				AddNodeEPG1Middle( prevEpg1NodePtr, epg1NodePtr);
				return;
			}
			else if( epg1Id == epg1NodePtr->epg1Id )
			{
				InsertEpg2(epg1NodePtr);//continue search in the next dimension
				return;
			}
			else
			{
				prevEpg1NodePtr = epg1NodePtr;
				epg1NodePtr = epg1NodePtr->epg1Next;
			}
		}
		AddNodeEPG1Middle( prevEpg1NodePtr, NULL);
	}
}
void InsertEdgeDir(ED_t * edNodePtr )
{
	UINT16 edId = curStateHashIndex.ED;
	/*new edge dir id could be less, equal or more*/

	/*1. Less than the first node edge dir index*/
	PRINT_TEXT("InsertEdgeDir()\n");
	if( edId < edNodePtr->edId )
	{
		AddNodeEdgeDirBegin( edNodePtr);
		return;
	}
	else if( edId == edNodePtr->edId )
	{
		InsertEpg1(edNodePtr);//continue search in the next dimension
		return;
	}
	else
	{
		ED_t * prevEdNodePtr = edNodePtr;
		edNodePtr = edNodePtr->edNext;
		int x=0;
		while(edNodePtr)
		{
			if(x> 5000)
			{
				printf("while loop too long InsertEdgeDir\n");
				return;
			}
			x++;
			if( edId < edNodePtr->edId )
			{
				AddNodeEdgeDirMiddle( prevEdNodePtr, edNodePtr);
				return;
			}
			else if( edId == edNodePtr->edId )
			{
				InsertEpg1(edNodePtr);//continue search in the next dimension
				return;
			}
			else
			{
				prevEdNodePtr = edNodePtr;
				edNodePtr = edNodePtr->edNext;
			}
		}

		AddNodeEdgeDirMiddle( prevEdNodePtr, NULL);
	}
}

/*No parameters; The global variable 'curNode' contains the new state to be inserted*/
void Insert(void)
{
	PRINT_TEXT("Insert()\n");
	CalculateSubTableIndices( &(curNode.state),&curStateHashIndex);

	//PrintSubTableIndices(&curStateHashIndex);
	CheckIndices();
	if(goalFlag == PROBLEM_NODE)
	{
		printf("problem node indices\n");
		PrintSubTableIndices(&curStateHashIndex);
	}
	if( CDTable[curStateHashIndex.CD] == NULL )
	{
		AddNodeEdgeDirBegin( NULL );
	}
	else
	{
		if(goalFlag == PROBLEM_NODE)
		{
			printf("problem node ***insert\n");
			printf("goalFlag=%d\n",goalFlag);
			printf("CD=%d\n",curStateHashIndex.CD);
		}

		InsertEdgeDir(CDTable[curStateHashIndex.CD]);
	}

}
BOOLEAN FindCpg1(EPG2_t * parent)
{
	UINT16 cpg1Id = curStateHashIndex.CPG1;
	CPG1_t * cpg1NodePtr = parent->cpg1Ptr;

	if( cpg1Id < cpg1NodePtr->cpg1Id )
	{
		return 0;
	}
	else if( cpg1Id == cpg1NodePtr->cpg1Id )
    {
        //printf("same red grpah node match found2!\n");
        //RB_ExitProper();
        //return 2;
        return FindGraph( cpg1NodePtr);
    }
	else
	{
		CPG1_t * prevCpg1NodePtr = cpg1NodePtr;
		cpg1NodePtr = cpg1NodePtr->cpg1Next;
		int x=0;
		while(cpg1NodePtr)
		{
			if( x++ > 1000 )
			{
				printf("while loop too long FindCpg1\n");
				return 0;
			}

			if( cpg1Id < cpg1NodePtr->cpg1Id )
			{
				return 0;
			}
			else if( cpg1Id == cpg1NodePtr->cpg1Id )
			{
				printf("same red grpah node match found1!\n");
                //return 2;
				return FindGraph( cpg1NodePtr);
			}
			else
			{
				prevCpg1NodePtr = cpg1NodePtr;
				cpg1NodePtr = cpg1NodePtr->cpg1Next;
			}
		}
		return 0;
	}
}
BOOLEAN FindEpg2(EPG1_t * parent )
{
	UINT16 epg2Id = curStateHashIndex.EPG2;
	EPG2_t * epg2NodePtr = parent->epg2Ptr;

	if( epg2Id < epg2NodePtr->epg2Id )
	{
		return 0;
	}
	else if( epg2Id == epg2NodePtr->epg2Id )
	{
			//printf("iFind %s =\n",__func__);
		return FindCpg1(epg2NodePtr);//continue search in the next dimension
	}
	else
	{
		EPG2_t * prevEpg2NodePtr = epg2NodePtr;
		epg2NodePtr = epg2NodePtr->epg2Next;
		while(epg2NodePtr)
		{
			if( epg2Id < epg2NodePtr->epg2Id )
			{
				return 0;
			}
			else if( epg2Id == epg2NodePtr->epg2Id )
			{
			//printf("iFind %s =\n",__func__);
				return FindCpg1(epg2NodePtr);//continue search in the next dimension
			}
			else
			{
				prevEpg2NodePtr = epg2NodePtr;
				epg2NodePtr = epg2NodePtr->epg2Next;
			}
		}
		return 0;
	}
}
BOOLEAN FindEpg1(ED_t * parent )
{
	UINT16 epg1Id = curStateHashIndex.EPG1;
	EPG1_t * epg1NodePtr = parent->epg1Ptr;

	if( epg1Id < epg1NodePtr->epg1Id )
	{
		return 0;
	}
	else if( epg1Id == epg1NodePtr->epg1Id )
	{
			//printf("iFind %s =\n",__func__);
		return FindEpg2(epg1NodePtr);//continue search in the next dimension
	}
	else
	{
		EPG1_t * prevEpg1NodePtr = epg1NodePtr;
		epg1NodePtr = epg1NodePtr->epg1Next;
		while(epg1NodePtr)
		{
			if( epg1Id < epg1NodePtr->epg1Id )
			{
				return 0;
			}
			else if( epg1Id == epg1NodePtr->epg1Id )
			{
			//printf("iFind %s =\n",__func__);
				return FindEpg2(epg1NodePtr);//continue search in the next dimension
			}
			else
			{
				prevEpg1NodePtr = epg1NodePtr;
				epg1NodePtr = epg1NodePtr->epg1Next;
			}
		}
		return 0;
	}

}
BOOLEAN FindEdgeDir(ED_t * edNodePtr)
{
	UINT16 edId = curStateHashIndex.ED;
	/*new edge dir id could be less, equal or more*/

	/*1. Less than the first node edge dir index*/

	if( edId < edNodePtr->edId )
	{
		return 0;
	}
	else if( edId == edNodePtr->edId )
	{
			//printf("iFind %s =\n",__func__);
		return FindEpg1(edNodePtr);//continue search in the next dimension
	}
	else
	{
		ED_t * prevEdNodePtr = edNodePtr;
		edNodePtr = edNodePtr->edNext;
		while(edNodePtr)
		{
			if( edId < edNodePtr->edId )
			{
				return 0;
			}
			else if( edId == edNodePtr->edId )
			{
			//printf("iFind %s =\n",__func__);
				return FindEpg1(edNodePtr);//continue search in the next dimension
			}
			else
			{
				prevEdNodePtr = edNodePtr;
				edNodePtr = edNodePtr->edNext;
			}
		}

		return 0;
	}  
}
BOOLEAN Find(rbCompState * state)
{
	CalculateSubTableIndices( state, &curStateHashIndex);

	//PrintSubTableIndices(&curStateHashIndex);

	if( CDTable[curStateHashIndex.CD] == NULL )
	{
		return 0;
	}
	else
	{
//			printf("iFind %s =\n",__func__);
		return FindEdgeDir(CDTable[curStateHashIndex.CD]);
	}
}

void calc_graph(FinalNode * gr, UINT8 flag, rbCompState * startState)
{
	rbCubeState parent_full,child_full;

	goalFlag = flag;
	//   PrintCompState(startState);
	curNode.state = (*startState);
	PrintCompState(&(curNode.state));
	Insert();
	goalFlag++;
	for(curNode.parentIndex=0;curNode.parentIndex<PARENT_COUNT;
			curNode.parentIndex++)
    {
        GetRBStateFullFromComp( &(gr[curNode.parentIndex].state), &parent_full);
        //printf("---------------------------------\n");
        //printf("parent node is:\n");
        //PrintFull(&parent_full);
        for(curNode.parentMove=0;curNode.parentMove<12;(curNode.parentMove)++)
        {
            //if(curGraphNodePtr < DEBUG_N )
            // printf("move number is %d\n",curNode.parentMove);
            /*compute a neighbour on this move only if the move th neighbour in yesMoves is 0*/
            PRINT_TEXT_2INTS("calc_graph() move num=%d, parent index=%d\n",curNode.parentMove,curNode.parentIndex);
            PRINT_TEXT_1INT("parent level=%d\n",gr[curNode.parentIndex].level);
            if( 1/*(gr[curNode.parentIndex].yesMoves & MOVE_MASK(curNode.parentMove) ) == 0*/)
            {
                GetChildComp1(&parent_full,&(curNode.state),curNode.parentMove);
                //GetRBStateFullFromComp( &(curNode.state), &child_full);
                //printf("child node is:\n");
                //PrintFull(&child_full);
                //if(curGraphNodePtr < DEBUG_N )
                //PrintCompState(&(curNode.state));
                Insert();

                PRINT_TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                if( matchCount > 1 )
                {
                    printf("Match found in calc_graph. Exiting from inner for loop\n");
                    break;
                }
                if( nLevelsExpl > 10 )
                {
                    printf("explored 10 levels. Exiting from inner for loop\n");
                    break;
                }
            }
        }
        if( matchCount > 1 )
        {
            printf("Match found in calc_graph. Exiting from outer for loop\n");
            break;
        }
        if( nLevelsExpl > 10 )
        {
            printf("explored 10 levels. Exiting from outer for loop\n");
            break;
        }
    }
	printf("Final Parent count = %d\n",curNode.parentIndex);
	printf("Graph node count = %d\n",curGraphNodePtr);
	printf("Graph1 node count = %d\n",curGraph1NodePtr);
}
/*InitCDTable: Initialize corner direction values table
 * This is the fist tier of the hash table*/
static void InitCDTable( void )
{
   int i;
   for(i=0;i<POWER3_8;i++)
      CDTable[i] = NULL;
}
void printRbCompState( rbCompState * state )
{
   printf("The RB Compressed state is:%d,%d,%d\n",state->edgePos,state->cornerPos,state->dir);
}

/*Allocate memory for the arrays where the actual nodes are stored*/
void AllocateGraphs(void)
{
   Graph = (FinalNode*)malloc(sizeof(FinalNode)*NODE_COUNT);
   if( !Graph )
   {
      printf("could not allocate memory to Graph\n");
      exit(0);
   }
   Graph1 = NULL;
//   Graph1 = (FinalNode*)malloc(sizeof(FinalNode)*NODE_COUNT);
 //  if( !Graph1 )
  // {
  //    printf("could not allocate memory to Graph1\n");
  //    exit(0);
  // }
}
void FreeGraphs(void)
{
   free(Graph);
   //free(Graph1);
   printf("memory for graphs freed\n");
}
void FindProblemTreePath(void)
{
}
void FindGoalTreePath(void)
{
}
void printRedGraphPath(UINT8 * path, UINT8 n)
{
   UINT8 i;
   printf("red graph path length = %d\n",n);
   printf("red graph path:\n");
   for(i=0;i<n;i++)
      printf("%4d",path[i]);
   printf("\n");
}
void PrintD5Index(UINT8 *id)
{
   UINT8 i;
   printf("d5 index = ");
   for(i=0;i<5;i++)
      printf("%4d",id[i]);
   printf("\n");
}
void FindSameRedGraphStateCountForIndex(int id);
#if 1
void FindRedGraphPath(void)
{
	UINT8 redGraphPathLengthLoc;
	UINT8 redGraphPathLoc[30];
	printf("FindRedGraphPath_func\n");
	int id=goalTreeMatchStateIndex,goalCount=0;
	UINT8 redGroupGoal,redGroupProblem;
	UINT8 d5IndexProblemTreeMatchState[5],d5IndexGoalTreeMatchState[5];
	printf("FindRedGraphPath_func1\n");
	GetD5Index(&problemTreeMatchState,d5IndexProblemTreeMatchState);
	GetD5Index(&goalTreeMatchState,d5IndexGoalTreeMatchState);
	//   FindSameRedGraphStateCount(goalTreeMatchStateIndex);
	//FindSameRedGraphStateCount(0);
	redGroupProblem = GetRedGraphGroupId(d5IndexProblemTreeMatchState);
	printf("redGroupProblem=%d\n",redGroupProblem);
#if 1
	//while(id != BAD_INDEX )
	//{
	// goalCount++;
	// printf("goalcount=%d\n",goalCount);
	//  GetD5Index(&(Graph[id].state),d5IndexGoalTreeMatchState);
	redGroupGoal = GetRedGraphGroupId(d5IndexGoalTreeMatchState);
	printf("redGroupGoal=%d\n",redGroupGoal);

	//id = Graph[id].nodeNext;
	//printf("id=%d\n",id);
	//}
#endif
	// PrintD5Index(d5IndexProblemTreeMatchState);
	//   PrintD5Index(d5IndexGoalTreeMatchState);
	//FreeGraphs();
	//printf("goalCount=%d\n",goalCount);

	if( !GetRedGraphPath(d5IndexProblemTreeMatchState,
				d5IndexGoalTreeMatchState,
				redGraphPathLoc,
				&redGraphPathLengthLoc/*,
				1*/) )
		printf("path could not be found!!!\n");
	printRedGraphPath(redGraphPathLoc,redGraphPathLengthLoc);
	int i;
	for(i=0;i<redGraphPathLengthLoc;i++)
	{
		printf("i=%d len=%d\n",i,redGraphPathLengthLoc);
		printf("move=%d \n",2*redGraphPathLoc[i]);
		PathRed[2*i] = 2*redGraphPathLoc[i];
		PathRed[2*i+1] = 2*redGraphPathLoc[i];
	}
	plnRed = redGraphPathLengthLoc*2-2;

}
#endif
void CalcPaths(void)
{
      FindProblemTreePath();
      FindGoalTreePath();

      FreeGraphs();

      FindRedGraphPath();
}
void FindSameRedGraphStateCountForIndex(int id)
{
   int idTmp,count=0;
   UINT8 index5[5];
   rbCubeState state_full;
   printf("ser.No   id   level\n");
   while(id != BAD_INDEX )
   {
      count++;
      printf("%5d %8d  %d\n",count,id,Graph[id].level);
      PrintCompState(&(Graph[id].state));
      GetD5Index(&(Graph[id].state),index5);
      PrintArray(index5,5);
      PrintPathToGoal(id,PathGoal,&plnGoal);
      GetRBStateFullFromComp( &(Graph[id].state), &state_full);
      PrintFull(&state_full);
      id = Graph[id].nodeNext;
      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   }
   printf("count=%d\n",count);
}
#if 0
   rbCompState state;
   UINT16 yesMoves; /*12 bits used: iTh(from the right; right most bit is the LSB ) bit
		      is 1, if a neighbour with that move is already
   present in the graph dicovered so far. Otherwise it is zero*/
   UINT8 parentMove;/*move made at the parent to come to this state(0..11)*/
   UINT8 level;
//   UINT8 redGraphGroup;
   INT32 nodeNext;
   INT32 parentIndex;
   UINT8 firstInRedGr;
   UINT8 RedGroup;
#endif
void FindRedGraphCount(void)
{
	int i,count=0,redgrcnt=0;
	for(i=0;i<curGraphNodePtr;i++)
	{
		printf("Graph[%3d].nodeNext=%3d pmove:%2d pidx:%3d, frgr:%d, rgrp:%2d, redgrcnt=%3d  ",
				i,Graph[i].nodeNext,
				Graph[i].parentMove,
				Graph[i].parentIndex,
				Graph[i].firstInRedGr,
				Graph[i].RedGroup,
				count);
		if(Graph[i].firstInRedGr)
		{
			redgrcnt++;
			Graph[i].RedGraph = redgrcnt;
		}
		PrintMove(Graph[i].parentMove);
		PrintMove(Graph[Graph[i].parentIndex].parentMove);
		//printf("\n");
		if ( Graph[i].nodeNext == BAD_INDEX )
			count++;
		else
			Graph[Graph[i].nodeNext].RedGraph = Graph[i].RedGraph;

		printf("redgr:%d\n",Graph[i].RedGraph);
	}
	printf("Graph[%d].nodeNext=%d\n",i,Graph[i].nodeNext);
	printf("%5d red graph count calculated=%d\n",curGraphNodePtr,count);
}
void FindSameRedGraphStateCount(void)
{
   int index,tmp=BAD_INDEX;

   for(index=0;index<1;index++)
   {
      printf("index=%d    ",index);
      FindSameRedGraphStateCountForIndex(index);
      printf("\n\n\n");
   }
}
void PrintNodes( FinalNode * Gr )
{
	int i;
	for(i=0;i<100;i++)
	{
		printf("index=%d\n",i);
		PrintFinalNode(&(Gr[i]));
		getchar();
	}
}
void ReadPath(int path[], int * len)
{
   int i;
   printf("enter path length\n");
   scanf("%d",len);
   printf("enter path\n");
   for(i=0;i<(*len);i++)
   {
      scanf("%d",&path[i]);
   }
}
void GetTestState( rbCompState * state )
{
   int path[35];
   int i,length;

   rbCubeState parent = GOAL_STATE;
   rbCubeState child;
   ReadPath(path,&length);
   for(i=0;i<length;i++)
   {
      GetChild(&parent, &child, path[i]);
      parent = child;
   }

   GetRBStateCompFromFull(&child,state);
}
BOOLEAN CompareCompState(rbCompState * s1, rbCompState * s2)
{
   return ( s1->edgePos == s2->edgePos &&
	 s1->cornerPos == s2->cornerPos &&
	 s1->dir == s2->dir );
      
}
void FindInGraph(rbCompState * state)
{
   int i;
   for(i=0;i<curGraphNodePtr;i++)
      if( CompareCompState(state, &(Graph[i].state) ) )
      {
	 printf("state found at %d\n",i);
	 return;
      }
   printf("state not found\n");
}
BOOLEAN CompareHashIndexState(void)
{
   return (curStateHashIndex.CD == 5856 &&
	 curStateHashIndex.ED == 2656 &&
	 curStateHashIndex.EPG1 == 208 &&
	 curStateHashIndex.EPG2 == 55 &&
	 curStateHashIndex.CPG1 == 58);
}
void FindInGraph1(void)
{
   int i;
   for(i=0;i<curGraphNodePtr;i++)
   {
      CalculateSubTableIndices(&(Graph[i].state), &curStateHashIndex);
      if( CompareHashIndexState( ) )
      {
	 printf("nodeNext[%d]= %d\n",i,Graph[i].nodeNext);
      }
   }

}
void CheckGraph(void)
{
   int i;
   rbCompState state;
   for(i=0;i<20;i++)
   {
      GetTestState(&state);
      CalculateSubTableIndices(&(state), &curStateHashIndex);
      PrintSubTableIndices(&curStateHashIndex);
      FindInGraph(&state);
   }
}
/*find diffs between nodes of 'Graph' and 'Graph1'; if the difference lies in 'Graph'
 *then we know the path between that pair of vertices hence we can find the path
 *from problem node to the Goal node*/

void ExploreDiffs(void)
{
#if 1
	PRINT_TEXT("Explore diffs \n");
	printf("***********Explore Diffs***********\n");
	int i,j;
	rbCubeState stateGraphFull, stateGraph1Full, diff;
	rbCompState diffComp;//compressed difference
	goalFlag = FINDING_DIFFS;
	//for(i=curGraphNodePtr;i>105047;i--)
	for(i=105047;i<curGraphNodePtr;i++)
	{
		//PrintCompState(&(Graph[i].state));
		GetRBStateFullFromComp( &(Graph[i].state), &stateGraphFull);
		printf("i=%d\n",i);
		//PrintFull(&stateGraphFull);
		//for(j=curGraph1NodePtr;j>105047;j--)
		for(j=105047;j<curGraph1NodePtr;j++)
		{
			//PrintCompState(&(Graph1[i].state));
			GetRBStateFullFromComp( &(Graph1[j].state), &stateGraph1Full);
			//PrintFull(&stateGraph1Full);
			RbCubeConfigDiff(&stateGraphFull, &stateGraph1Full, &diff);
			GetRBStateCompFromFull(&diff, &diffComp);
            searchState = curNode.state;
			if( Find(&diffComp) )
			{
				printf("###############diff found in Graph!!!#########\n");
				printf("i=%d, j=%d\n", i,j);
				PrintPathToGoal(i,PathGoal,&plnGoal);
				PrintPathToProblemNode(j);
				return;
			}
		}
	}
#endif
}
void MarkRedGraphs(void)
{
    int i,ni,cnt=0,cnt1=0,cnt2=0,cntrg=0;
    for(i=0;i<curGraphNodePtr;i++)
        if (Graph[i].firstInRedGr!=2)
            printf("*** error %d\n",i);
    for(i=0;i<curGraphNodePtr;i++)
    {
        if (Graph[i].firstInRedGr==2)
        {
            Graph[i].firstInRedGr = 1;
            cnt2++;
            ni = Graph[i].nodeNext;
            cntrg=0;
            while ( ni!=BAD_INDEX )
            {
                Graph[ni].firstInRedGr = 0;
                cnt++;
                cntrg++;
                ni = Graph[ni].nodeNext;
            }
            //printf("i=%d, cntrg=%d\n",i,cntrg);
        }
        else
        {
            //printf("i=%d firstInRedGr=%d\n",i,Graph[i].firstInRedGr);
            cnt1++;
        }
    }
            printf("cnt=%d cnt1=%d firstinRedGr=%d\n",cnt,cnt1,cnt2);
}
void RandomSearchPool(/*rbCubeState * problemState*/)
{
#define RandPoolSize 8000
    UINT8 dist = 4;
	int i,curPoolSize=0,wcounter=0,j;
	rbCubeState randNodePool[RandPoolSize];
	rbCompState problemStateComp,sComp;
	rbCubeState statePoolFull, problemStateFull, diff;
	char poolFile[20];
	snprintf(poolFile,20,"StatePool%d",dist);
	FILE * fp = fopen(poolFile,"a+");
	int result,pos;
	while(1)
	{
		for(i=0;i<12;i++)
		{
			result = fscanf(fp,"%d",&pos);
			if (result == EOF)
				break;
			randNodePool[curPoolSize].edgePos[i]=pos;
		}
		for(i=0;i<12;i++)
		{
			result = fscanf(fp,"%d",&pos);
			if (result == EOF)
				break;
			randNodePool[curPoolSize].edgeDir[i]=pos;
		}
		for(i=0;i<8;i++)
		{
			result = fscanf(fp,"%d",&pos);
			if (result == EOF)
				break;
			randNodePool[curPoolSize].cornerPos[i]=pos;
		}
		for(i=0;i<8;i++)
		{
			result = fscanf(fp,"%d",&pos);
			if (result == EOF)
				break;
			randNodePool[curPoolSize].cornerDir[i]=pos;
		}
		if (result == EOF)
			break;
		curPoolSize++;
	}
    while(wcounter<RandPoolSize)
	{
		UINT8 diffFoundinGraph = 0;
        GetRandRbState(&problemStateComp,dist);
		//printf("check 1\n");
        GetRBStateFullFromComp(&problemStateComp, &problemStateFull);
		//printf("check 2\n");
        /* Check if it is at distance 7 or more from existing nodes in the pool*/
        for(i=0;i<curPoolSize;i++)
		{
			RbCubeConfigDiff(&randNodePool[i], &problemStateFull, &diff);
            GetRBStateCompFromFull(&diff, &sComp);
		    //printf("check 3\n");
            curNode.state = sComp;
            searchState = sComp;
            diffFoundinGraph = Find(&sComp);
			if (diffFoundinGraph)
			{
			    printf("Diff found in graph i=%d Node won't be added to pool\n",i);
			    break;
			}
		}
		if (!diffFoundinGraph)
		{
		    randNodePool[curPoolSize++] = problemStateFull;
			for(j=0;j<12;j++)
			    fprintf(fp," %d",problemStateFull.edgePos[j]);
			fprintf(fp,"\n");
			for(j=0;j<12;j++)
			    fprintf(fp," %d",problemStateFull.edgeDir[j]);
			fprintf(fp,"\n");
			for(j=0;j<8;j++)
			    fprintf(fp," %d",problemStateFull.cornerPos[j]);
			fprintf(fp,"\n");
			for(j=0;j<8;j++)
			    fprintf(fp," %d",problemStateFull.cornerDir[j]);
			fprintf(fp,"\n");
			printf("node added to pool. curPoolSize=%d\n",curPoolSize);
		}
		//printf("check 4\n");
		wcounter++;
		printf("wcounter=%d, curPoolSize=%d\n",wcounter,curPoolSize);

	}
	fclose(fp);
}
void RandomSearch(rbCubeState * problemState)
{
    int found=0,iter=0,i;
    UINT8 len=10;
    rbCompState randPath[100];
    printf("problemstate is\n");
    PrintFull(problemState);
    
    while(!found)
    {
        GetRandRbPath(problemState,len,randPath);
        for(i=0; i<len && !found; i++)
		{
            //printf("i=%d comp state is:\n",i);
            //PrintCompState( &(randPath[i]) );
            curNode.state = randPath[i];
            searchState = randPath[i];
			found = Find(&(randPath[i]));
		}
        iter++;
        if(iter%9000==0)
            printf("iter=%d found=%d\n",iter,found);
        //break;
    }
    if(found)
        printf("***found***\n");
    else
        printf("not found\n");

}
int main( void )
{
    int i,choice=0,choice1=0,d1,nLevels,found;
    UINT8 dist;
    int distInt;
    int dist11;
    rbCompState problemStateComp,sComp;
    rbCubeState problemState,sFull,DestFull,parent;
    rbIndices indices;
    printf("Enter the number of levels to explore(min 2 Maximum 7)\n");
    scanf("%d",&nLevels);
    if(nLevels > 0 && nLevels < 9 )
        PARENT_COUNT = ParentCount[nLevels-1];
    else
        PARENT_COUNT = ParentCount[6];

    AllocateGraphs();
    //printf("graph size = %d\n",sizeof(Graph));
    InitCDTable();
    srand(time(0));

    //PopulateRedGroups("./child_gen/redGroup.txt");
    //printf("red graphs generated\n");
    calc_graph(Graph,GOAL_NODE,&GoalState);
	printf(" epg1cnt=%d\n",epg1cnt);
	printf(" epg2cnt=%d\n",epg2cnt);
	printf(" cpg1cnt=%d\n",cpg1cnt);
	printf(" edircnt=%d\n",edircnt);
    MarkRedGraphs();
    printf("redGraphCount=%d\n",redGraphCount);
    //GenerateAllRedGraphs();
    //   printf("press any key to continue\n");
    //getchar();
    //CheckGraph();
    //   FindSameRedGraphStateCount();
    //   PrintNodes();
     //FindRedGraphCount();
    printf("redGraphCount=%d\n",redGraphCount);
    int wLoop=0;
    while(wLoop<400)
    {
        wLoop++;
        int iSearchCount=0; 
        plnProb=plnGoal=plnRed=0;
        pathidx=0;
        pathidx1=0;
        printf("================================================================================\n");
        printf("Enter choice\n");
        printf("0 user config\n");
        printf("1 Generated Config(choose this if not sure)\n");
        printf("2 exit\n");
        choice = 1;
        scanf("%d",&choice);
        switch(choice)
        {
            case 0:
                GetUserInputConfig(&problemState);
                GetRBStateCompFromFull(&problemState, &problemStateComp);
                break;
            case 1:
                //printf("Enter config type choice\n");
                //printf("0 normal config\n");
                //printf("1 red graph Config\n");
                choice1 = 0;
                //scanf("%d",&choice1);
                switch(choice1)
                {
                    case 0:
                        printf("Enter random state distance\n");
                        //dist = 2*nLevels+2;
                        scanf("%d",&distInt);
						dist = distInt;
                        GetRandRbState(&problemStateComp,dist);
                        GetRBStateFullFromComp(&problemStateComp, &problemState);
                        printf("Got random state at dist:%d\n",dist);
                        break;
                    case 1:
                        printf("Enter random state distance\n");
                        scanf("%d",&dist11);
                        dist=dist11;
                        printf("enter red distance\n");
                        scanf("%d",&d1);
                        //GetRandRbState(&problemStateComp,dist);
                        //GetRBStateFullFromComp(&problemStateComp, &problemState);
                        //CalculateSubTableIndices( &problemStateComp,&indices);
                        //PrintFull(&problemState);
                        //PrintSubTableIndices(&indices);
                        //parent = problemState;
                        //GetChild(&parent,&problemState,5);
                        //parent = problemState;
                        //GetChild(&parent,&problemState,5);
                        //GetRBStateCompFromFull(&problemState, &problemStateComp);
                        //CalculateSubTableIndices( &problemStateComp,&indices);
                        //PrintSubTableIndices(&indices);
                        //PrintFull(&problemState);
                        //GetRandRbState(&problemStateComp,dist);
                        GetRandRbRedState(&problemStateComp,dist,d1);
                        GetRBStateFullFromComp(&problemStateComp, &problemState);
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                return 0;
            default:
                printf("bad choice %d\n",choice);
                break;
        }
        printRbCompState(&problemStateComp);
        //printf("Enter explore method choice\n");
        //printf("0 config function \n");
        //printf("1 calc_graph\n");
        //printf("2 random search\n");
        //printf("3 random search pool\n");
        //printf("4 exit\n");
        //scanf("%d",&choice);
        choice = 0;
        switch(choice)
        {
            case 0:
                printf("searching node in the Graph\n");
                iSearchCount=0; 
                /*TODO no need to search for each i.
                  Enough to search with i that is root for that red graph
                  No. we need to search each node*/
                for(i=0;i<curGraphNodePtr;i++)
                {
                    gFindLevel=0;
                    iSearchCount++; 
                    //printf("i=%d\n",i);
                    GetRBStateFullFromComp(&Graph[i].state, &sFull );
                    //PrintFull(&sFull,"state i");
                    //printf("i1=%d\n",i);
                    GetChildByConfig(&problemState,&sFull,&DestFull);
                    //PrintFullN(&problemState,"prob");
                    //PrintFullN(&DestFull,"dest");
                    //printf("i2=%d\n",i);
                    GetRBStateCompFromFull(&DestFull,&sComp);
                    //printf("i3=%d\n",i);
                    //printRbCompState(&sComp);
                    curNode.state = sComp;
                    searchState = sComp;
                    found = Find(&sComp);
//                    if ( i >0 && i%100)
  //                     printf("searched %d\n",i);
                    if ( found == 1 || found ==2)
                    {
                        int PathFinal[60],lnFin=0;
                        rbCubeState s11,s12,child;
                        s11 = problemState;
                        printf("\n*** Exact Match Found *** i=%d found=%d\n\n",i,found);

                        printf("destfull\n");
                        PrintFull(&DestFull);
                        PrintPathToGoal(i,PathProb,&plnProb);

                        printf("*****full path\n");
                        int iii,mi;
                        for(iii=plnProb-1;iii>=0;iii--)
                        {
                            PrintMove(PathProb[iii]^0x01);
                            PathFinal[lnFin++]= PathProb[iii]^0x01;
                        }
                        printf(" --- ");
                        //for(iii=plnRed-1;iii>=0;iii--)
                        for(iii=0;iii<plnRed;iii++)
                        {
                            PrintMove(PathRed[iii]);
                            PathFinal[lnFin++]= PathRed[iii];
                        }
                        printf(" --- ");
                        for(iii=0;iii<plnGoal;iii++)
                        {
                            PrintMove(PathGoal[iii]);
                            PathFinal[lnFin++]= PathGoal[iii];
                        }
                        printf("\nPath Length:%d\n",lnFin);
                        //PrintPathToProblemNode(i);

                        parent = problemState;
                        for(mi=0;mi<lnFin;mi++)
                        {
                            GetChild(&parent,&child,PathFinal[mi]);
                            parent = child;
                        }
                        PrintFull(&child);
                        rbCubeState goalSt = GOAL_STATE;
                        if (CompareFullStates(&child,&goalSt))
                            printf("\n***********success. Path is correct\n\n");
                        else
                        {
                            printf("\n***********Fail. Path is not correct\n\n");
                            return 0;
                        }


                        break;
                    }
                    else if( found == 2 )
                        printf("found2 at i=%d\n",i);
                    //else
                    //  printf("*** exact match not found with i=%d ***\n",i);
                    //printf("i4=%d\n",i);
                }
                printf("searched in %d indices out of %d\n",iSearchCount,i);
                if (i==curGraphNodePtr)
                {
                    printf("\n*** match node not found ***\n\n");
                    return 0;
                }
                break;
            case 1:
                printf("Exploring graph of problem state\n");


                calc_graph(Graph1,PROBLEM_NODE,&problemStateComp);
                PrintNodes(Graph1);

                if( matchCount > 0 )
                    printf("matches found!!! match count = %d\n", matchCount);
                else{      
                    printf("Match not found\n");
                    ExploreDiffs();
                }
                break;
            case 2:
                RandomSearch(&problemState);
                break;
		    case 3:
			    RandomSearchPool();
				break;
            case 4:
                return 0;
        }
    }
    FreeGraphs();
    return 0;
}
