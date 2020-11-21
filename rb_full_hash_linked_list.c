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
  #define NODE_COUNT 96873856
//#define NODE_COUNT  9839380
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
/*4*/11207,
/*5*/105047,
/*6*/983927,
/*7*/9205559,
/*8*/90055581
};
#define POWER3_8 6561/*3 to the power of 8*/
#define RB_NEW(x) ((x*)malloc(sizeof(x)))
#define RB_NEW_ARRAY(x,n) ((x*)malloc(n*sizeof(x)))

#define MOVE_MASK(x) (0x0001<<x)
#define INVERSE_MOVE(move) (move ^ 0x01)
#define BAD_INDEX (-1)
#define UPDATE_YES_MOVES(g,x,y) g[x].yesMoves|=MOVE_MASK(INVERSE_MOVE(y))

/*values for "goalFlag" the possible problem states */
#define GOAL_NODE 0
#define GOAL_GRAPH 1
#define FINDING_DIFFS 4
#define DEBUG_N 1

/*epg1cnt=3613834
 epg2cnt=3769263
 cpg1cnt=3827147
 edircnt=2472827*/

typedef unsigned char BOOLEAN;


typedef struct{
   rbCubeState state;
   UINT8 parentMove;/*move made at the parent to come to this state(0..11)*/
   UINT8 level;/*distance from Goal node.*/
   INT32 nodeNext;
   INT32 parentIndex;
} FinalNode;

//268435455 = 0x0FFFFFFF
//#define EPosTableSize (79833600*6)
#define EPosTableSize 479001600
#define FAC 1
int EPosTable[EPosTableSize];/*12!/6*/

/*red group */
struct CPos_tag{
   INT32 finalNode; /*index of a node in the Array of nodes of Goal Graph*/
   struct CPos_tag * CPosNext;
   UINT16 CPosId; /*0..40320(8!)*/
   UINT32 hitCount;/*no of nodes stored under this hash value */
};

typedef struct CPos_tag CPos_t;

struct cubesRed_tag{
    int level;
    struct{
        int redGraphId;
        int level;
    } child[6];
} CubesRedGraphNeighb[20736][32];
struct{
    int cube;
    int vertex;
    int levelChild;
    int move;
    rbCubeState stateChild;
    rbCubeState stateParent;
} cubeMove;

rbCubeState redGraphGoal[1297];

/*corner position group1*/
struct CPG1_tag{
   CPos_t * CPosPtr;
   struct CPG1_tag * cpg1Next;
   UINT8 cpg1Id; /*0..69*/
   UINT32 CPosCnt;/*no of nodes stored under this hash value */
   int redGraphId;
};

typedef struct CPG1_tag CPG1_t;

struct EPG2_tag{
   int cpg1Cnt;
   UINT16 epg2Id; /*0..69 ( 8C4 - 1 )*/ /*12C4 now*/
   struct EPG2_tag * epg2Next;
   CPG1_t * cpg1Ptr;
};

typedef struct EPG2_tag EPG2_t;

struct EPG1_tag{
   int epg2Cnt;
   UINT16 epg1Id;/*0..494*/
   struct EPG1_tag * epg1Next;
   EPG2_t * epg2Ptr;
};

typedef struct EPG1_tag EPG1_t;
   
struct ED_tag{
   int epg1Cnt;
   UINT16 edId;/*0..4095*/
   struct ED_tag * edNext;
   EPG1_t * epg1Ptr;
};

typedef struct ED_tag ED_t;

struct CD_tag{
   int edCnt;
   ED_t * edPtr;
};
typedef struct CD_tag CD_t;


static rbCubeState GoalState = GOAL_STATE;
//static rbCubeState GoalState = {244671840,29102,174069472};
//static FinalNode Goal = {{0,0,0},0,0,0,BAD_INDEX,BAD_INDEX};/*actually the parentMove and
//						parentIndex for the Goal node are undefined.
//					       Nobody is going to use them. They are used now*/

static FinalNode * Graph;


static UINT32 curGraphNodePtr=0;
static UINT32 curParentPtr;

static UINT32 curGraph1NodePtr=0;
static UINT32 curParent1Ptr;
static int gFindLevel=0;
static int nLevelsExpl;
static int redGroupCount;
static int cpg1cnt;
static int epg1cnt;
static int epg2cnt;
static int edircnt;

static int InsertGraphCount;
static int FinalNodeCount;

typedef struct {
   UINT16 CD;/*0..3^8 -1*/
   UINT16 ED;/*0..2^12 - 1*/

   /*There are three edge positon groups*/
   UINT16 EPG1;/*0..494(12C4-1)*/
   UINT16 EPG2;/*0..69(8C4-1)*/ /*upto 12C4 now*/

   /*There are two corner position groups*/
   UINT8 CPG1;/*0..69(8C4-1)*/
   UINT16 CPosId;

} rbIndices;

/*Actually, only 3**7 entries of this table will be used, because there are only that many
 * possible values for the corner cubelet direction value index*/
static CD_t CDTable[POWER3_8];


static FinalNode curNode;
static rbIndices curStateHashIndex;

static UINT8 goalFlag=GOAL_NODE;

static rbCubeState problemTreeMatchState;
static rbCubeState goalTreeMatchState;
static rbCubeState searchState;
static int goalTreeMatchStateIndex;
static int goalTreeMatchStateCount=0;
static int redGraphCount;
static int matchCount;
static int redGrChild;
static UINT8 redGraphPath[30],redGraphPathLength=0;

void FindRedGraphPath(void);
void PrintPathToGoal(int id,int Path[],int* ln);
int PathGoal[50],PathRed[30],PathProb[50],plnGoal,plnRed,plnProb;
//unsigned char GetRedGraphGroupId(rIndex id);
//int GetRedGraphPath(rIndex problemId,rIndex goalId,UINT8 * path, UINT8 * len);

void PrintFinalNode( FinalNode * node )
{
   printf("The final node is...\n");
   //PrintCompState(&(node->state));
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
void PushMove( int move)
{
//printf("pathidx=%d\n",pathidx);
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
	//printf("path to goal node from index:%d is:\n",id);
	while(id != 0)
	{
        //printf("ln=%d\n",*ln);
		//PrintMove(Graph[id].parentMove);
        //printf("ln=%d\n",*ln);
		PushMove(INVERSE_MOVE(Graph[id].parentMove));
        //printf("ln=%d\n",*ln);
		PathArr[(*ln)++] = INVERSE_MOVE(Graph[id].parentMove);
        //printf("ln=%d\n",*ln);
		PushMove(INVERSE_MOVE(Graph[id].parentMove));
		id = Graph[id].parentIndex;
        //printf("id=%d\n",id);
	}
	printf("\n");
	move = PopMove();
	while( move < 12 )
	{
		PushMove1(move);
		move = PopMove();
	}
}


/*No parameters; The global variable 'curNode' contains the new state to be inserted */
void AddFinalNodeToGoalGraph(void)
{
   static UINT8 curLevel=0;
   static INT32 levelCount=0;

   FinalNodeCount++;

   Graph[curGraphNodePtr].state = curNode.state;/*node contents - final state;structure copy*/

   Graph[curGraphNodePtr].nodeNext = BAD_INDEX;


   PRINT_TEXT_1INT("AddFinalNodeToGoalGraph, curGraphNodePtr=%d\n",curGraphNodePtr);
   if( goalFlag == GOAL_GRAPH )
   {
      Graph[curGraphNodePtr].parentMove = curNode.parentMove;
   //   UPDATE_YES_MOVES(Graph,curGraphNodePtr,curNode.parentMove );
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
   //CheckIndices();
   if(curGraphNodePtr > NODE_COUNT )
   {
	   printf("cur graph node ptr too large\n");
	   exit(0);
   }
}

BOOLEAN Find(rbCubeState * state);
void convert_whatisin_to_whereis(rbCubeState *whatisin_state,rbCubeState *whereis_state);
/*return 2 for found in red graph*/
BOOLEAN FindGraph(int cpg1PtrFinalNode )
{
	INT32 index = cpg1PtrFinalNode;
	INT32 indexPrev=BAD_INDEX;
	int x=0;
    UINT8 compare=0;
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
        //UINT8 searchStateRedGroup;
        x++;
        if( x > 64 )
        {
            printf("while loop too long FindGraph \n");
            //getchar();
        }

        //printf("redgroup matched index=%d redgroup=%d\n",index,Graph[index].RedGroup);
        if( CompareFullStates(&(Graph[index].state),&(searchState))/*this condition is redundant*/)
        {
            problemTreeMatchState = searchState;
            goalTreeMatchState = Graph[index].state;
            //printf("Exact Match found in function %s at index=%d!!!\n",__func__,index);
            //PrintPathToGoal(index,gFindLevel?PathRed:PathGoal,gFindLevel?(&plnRed):(&plnGoal));
            PrintPathToGoal(index,PathGoal,&plnGoal);
            return TRUE;
        }
        else
        {
            //rbCubeState s1,s2;
            //convert_whatisin_to_whereis( &(Graph[index].state),&s1);
            //convert_whatisin_to_whereis( &(searchState),&s2);/*this condition is redundant*/
            //if( CompareFullStatesT(&s1,&s2))
            //    printf("tripod match\n");
            //printf("redgroup matched index=%d but state didn't match\n",index);
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

    //if (gFindLevel >= 1)
      //  return FALSE;

    //gFindLevel++;


	PRINT_TEXT_1INT("FindGraph, last index found=%d\n",indexPrev);
	return FALSE;
}
static rbCubeState parent_state,child_state;
static rIndex parent_index;
void InsertGraph(int finalNode )
{
    INT32 index = finalNode;
    INT32 indexPrev=BAD_INDEX;
    int x=0;
    PRINT_TEXT_1INT("InsertGraph(), index=%d\n",index);
    //printf(" - InsertGraph(), index=%d  ",index);
    /*The first index will not be */
    if(index == BAD_INDEX )
        printf("error: first index -1 \n");

    while( index != BAD_INDEX )
    {
        x++;
        if( x > 199 )
        {
            printf("while loop too long InsertGraph x=%d\n",x);
            //return;
            //getchar();
        }
        if(CompareFullStates(&(Graph[index].state),&(curNode.state)) )
        {
            //printf("config exists\n");
            //printf("yes move found\n");
            return;
        }
        else
        {
            indexPrev = index;
            index = Graph[index].nodeNext;
            PRINT_TEXT_1INT("function InsertGraph, new index=%d\n",index);
            //printf("function InsertGraph, new index=%d\n",index);
        }
    }
    if(x==24)
        printf("finalNode=%d x=%d\n",finalNode,x);
    if(indexPrev == BAD_INDEX )
        printf("error: indexPrev cannot be bad\n");

    Graph[indexPrev].nodeNext = curGraphNodePtr;
    InsertGraphCount++;

    PRINT_TEXT_1INT("InsertGraph, last index found=%d\n",indexPrev);
    AddFinalNodeToGoalGraph();
}
void RB_ExitProper(void)
{
   int i;
   printf("kswjehfksdhfksdhfksjdhfkjhdf\nskdjfhskdjhfksjdfh\n");
   for(i=0;i<172361;i++)
      ;
   //exit(0);
}

    int eposT[9] = {0,1,3,4,5,7,8,10,11};
    int cposT[4] = {3,4,5,7};
    /*
int KEY(rbCubeState * state)
{
    int x,y;
    x = state->edgePos[eposT[0]];
    x <<= 5;
    x |= state->edgePos[eposT[1]];
    x <<= 5;
    x |= state->edgePos[eposT[2]];
    x <<= 5;
    x |= state->edgePos[eposT[3]];
    x <<= 5;
    x |= state->edgePos[eposT[4]];
    x <<= 5;
    x |= state->edgePos[eposT[5]];

    y = state->edgePos[eposT[6]];
    y <<= 5;
    y |= state->edgePos[eposT[7]];
    y <<= 5;
    y |= state->edgePos[eposT[8]];
    y <<= 5;
    y |= state->cornerPos[cposT[0]];
    y <<= 5;
    y |= state->cornerPos[cposT[1]];
    y <<= 5;
    y |= state->cornerPos[cposT[2]];
    y ^= state->cornerPos[cposT[3]];
    return (x ^ y)&0x0FFFFFFF;

}*/
int KEY(rbCubeState * state)
{
    int x,y,z;
    x = state->edgePos[0];
    x <<= 5;
    x |= state->edgePos[1];
    x <<= 5;
    x |= state->edgePos[2];
    x <<= 5;
    x |= state->edgePos[3];
    x <<= 5;
    x |= state->edgePos[4];
    x <<= 5;
    x |= state->edgePos[5];

    y = state->edgePos[6];
    y <<= 5;
    y |= state->edgePos[7];
    y <<= 5;
    y |= state->edgePos[8];
    y <<= 5;
    y |= state->edgePos[9];
    y <<= 5;
    y |= state->edgePos[10];
    y <<= 5;
    y |= state->edgePos[11];
    
    z = state->cornerPos[0];
    z <<= 5;
    z |= state->cornerPos[1];
    z <<= 5;
    z |= state->cornerPos[2];
    z <<= 5;
    z |= state->cornerPos[3];
    z <<= 5;
    z |= state->cornerPos[4];
    z <<= 5;
    z |= state->cornerPos[5];
    //return (x ^ z)&0x0FFFFFFF;
    return (x ^ y ^ z)&0x0FFFFFFF;

}
void convert_whatisin_to_whereis(rbCubeState *whatisin_state,rbCubeState *whereis_state)
{
   // *whereis_state = *whatisin_state;
    int i;
    for(i=0;i<12;i++)
        whereis_state->edgePos[0x0F & whatisin_state->edgePos[i]] = i | (whatisin_state->edgePos[i] & 0x10);
    for(i=0;i<8;i++)
        whereis_state->cornerPos[0x07 & whatisin_state->cornerPos[i]] = i | (whatisin_state->cornerPos[i] & 0x18);
        
}
int insgr=0;
int addgr=0;
//#define KEY(x,y,z) (((x>>4)<<4) | (y & 0x000F)  )
/*No parameters; The global variable 'curNode' contains the new state to be inserted*/
void Insert(void)
{
	PRINT_TEXT("Insert()\n");
    rbCubeState whereis_state;
    //convert_whatisin_to_whereis(&curNode.state,&whereis_state);
    int key= KEY(&curNode.state);
    //int key= KEY(&whereis_state);

    //printf("key=%d\n",key);
    //printf("EPosTable[key]=%d\n",EPosTable[key]);

    if (EPosTable[key] == BAD_INDEX)
    {
        EPosTable[key] = curGraphNodePtr;
        //printf("Insert() 1\n");
        AddFinalNodeToGoalGraph();
        addgr++;
    }
    else
    {
        //printf("Insert() 2\n");
        insgr++;
        InsertGraph(EPosTable[key]);
    }


}
BOOLEAN Find(rbCubeState * state)
{
    rbCubeState whereis_state;
    //convert_whatisin_to_whereis(state,&whereis_state);
    int key= KEY(state);
    //int key= KEY(&whereis_state);

    if(EPosTable[key]==BAD_INDEX)
        return 0;
    else
        return FindGraph(EPosTable[key]);
}

void calc_graph(FinalNode * gr, UINT8 flag, rbCubeState * startState)
{
	rbCubeState parent_full;

	goalFlag = flag;
	//   PrintCompState(startState);
	curNode.state = (*startState);
	Insert();
	goalFlag++;
	for(curNode.parentIndex=0;curNode.parentIndex<PARENT_COUNT;
			curNode.parentIndex++)
    {
        parent_full = Graph[curNode.parentIndex].state;
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
                GetChild(&parent_full,&(curNode.state),curNode.parentMove);
                //printf("child node is:\n");
                //PrintFull(&curNode.state);
                //if(curGraphNodePtr < DEBUG_N )
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
void printrbCubeState( rbCubeState * state )
{
    int i;
    printf("The RB state is:\n");
    printf("  edge:");
    for(i=0;i<12;i++)
        printf("%2d ",state->edgePos[i]);
    printf("\n");
    printf("corner:");
    for(i=0;i<8;i++)
        printf("%2d ",state->cornerPos[i]);
    printf("\n");
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
}
void FreeGraphs(void)
{
   free(Graph);
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
BOOLEAN Find16(rbCubeState * state, int *indexMatch);
/*
Write a prune function:
   take a node obtained after making n(>60) moves from Goal
   in the 60 move sequence, check if the shortest distance between any two points
   that are more than 16 apart is less than 16. If so, modify the sequence
   repeat until no more reduction can be made
   */
void PrunePath()
{
    int idmatch;
    UINT8 NNN = 67,len;
    rbCubeState states[100],diff;
    UINT8 dist, path[100],i,j;
    len = dist = NNN;
    GetRandRbStatePath(states,dist,path);
    while(1)
    {
        for(i=0;i<(len-16);i++)
        {
			RbCubeConfigDiff( &states[i], &states[i+16], &diff);
            Find16(&diff,&idmatch);
        }
    }
}
BOOLEAN Find16(rbCubeState * state, int *indexMatch)
{
    int i;
    //rbCubeState searchState;
    for(i=0;i<curGraphNodePtr;i++)
    {
        GetChildByConfig(state,&Graph[i].state,&searchState);
        if( Find(&searchState))
        {
            printf("searched in %d indices out of %d\n",i,curGraphNodePtr);
            *indexMatch = i;
            return TRUE;
        }
    }
    return FALSE;
}
/*
Generate 17 Grid
   1. Generate node at distance 17 from Goal.
   2. confirm it doesn't have path of length 16 or less from Goal, if not go to step 1
   3a. confirm it doesn't have path of length 8 or less from any node in the SAVED_LIST, if not go to step 1
   OR
   3b. confirm it doesn't have path of length 16 or less from any node in the SAVED_LIST, if not go to step 1
   4. save the node and its path to Goal to the SAVED_LIST
   5. go to step 1
   */

void Gernerate17Grid(void)
{
    UINT8 gridVal=17;
    int NNN = 7,idmatch;
    UINT8 Path[gridVal];
    UINT8 GridPath[NNN][gridVal];
    rbCubeState state,diff;
    rbCubeState stateGrid17[NNN];
    int gridLen=0,i;
    while(1)
    {
        //GetRandRbState(&state,gridVal);
        GetRandRbStatePath1(&state,gridVal,Path);
        printf("state generated\n");
        for(i=0;i<gridVal;i++)
            PrintMove(Path[i]);
        if(Find16(&state,&idmatch))
        {
            printf("state is dist less than 17. skipping\n");
            return;
            continue;
        }
        for(i=0;i<gridLen;i++)
        {
            printf("finding diff with grid[%d]\n",i);
            RbCubeConfigDiff( &stateGrid17[i], &state, &diff);
            if(Find16(&diff,&idmatch))
                break;
        }
        if(i<gridLen)
            continue;
        else
        {
            stateGrid17[gridLen++]= state;
            for(i=0;i<gridVal;i++)
            {
                GridPath[gridLen][i]=Path[i];
                PrintMove(Path[i]);
            }
            printf("\n");
            printf("node added to grid17; gridlen=%d\n",gridLen);
            if(gridLen>=NNN)
                return;
        }
    }
}
int main( void )
{
	int i,choice=0,choice1=0,d1,nLevels,found;
	UINT8 dist;
	int distInt;
	int dist11;
	time_t now;
	time_t now1;
	time(&now);
	struct tm * curTime;
	struct tm * curTime1;
	rbCubeState problemStateComp,sComp;
	rbCubeState problemState,sFull,DestFull,parent;
	rbIndices indices;
	printf("Enter the number of levels to explore(min 2 Maximum 8)\n");
	nLevels = 7;
	scanf("%d",&nLevels);
	if(nLevels > 0 && nLevels < 9 )
		PARENT_COUNT = ParentCount[nLevels-1];
	else
		PARENT_COUNT = ParentCount[6];

    printf("parent_count=%d\n",PARENT_COUNT);

	AllocateGraphs();
	//printf("graph size = %d\n",sizeof(Graph));
	srand(time(0));

	//PopulateRedGroups("./child_gen/redGroup.txt");
	//printf("red graphs generated\n");


	//GenerateAllRedGraphs();
	//curNode.state = GoalState;
	//Insert();
    printf("Enter choice\n");
	printf("0 explore reduced graph neighbours\n");
	printf("1 explore full graph\n");
	printf("2 exit\n");
    choice = 1;
    //scanf("%d",&choice);
    switch(choice)
    {
        case 0:
            GenerateCubes();
            int i0,i1,i2,i3,i4;
            unsigned int icube;
            unsigned short int vertex;
            /*for(i0=0;i0<24;i0++)
              for(i1=0;i1<24;i1++)
              for(i2=0;i2<24;i2++)
              for(i3=0;i3<24;i3++)
              for(i4=0;i4<24;i4++)*/
            for(icube=0;icube<20736;icube++)
            {
                for(vertex=0;vertex<32;vertex++)
                {
                    UINT8 path[40],len;
                    int i,redGr;
                    //rIndex parent_index;
                    rIndex child_index;
                    unsigned char red_pos[NGROUPS][GROUPSIZE];
                    unsigned char red_pos_child[NGROUPS][GROUPSIZE];
                    unsigned char red_pos_parent[NGROUPS][GROUPSIZE];
                    rbCubeState tmp_state, tmp_child;
                    rbCubeState child_state_comp;
                    /*parent_index[0] = i0;
                      parent_index[1] = i1;
                      parent_index[2] = i2;
                      parent_index[3] = i3;
                      parent_index[4] = i4;*/
                    printf("cube:%d, vertex:%d========================\n",icube,vertex);
                    GetCubeVertexState(icube,vertex,&parent_state);

                    //redGr = GetRedGroup(parent_index);
                    //if (redGr != 0)
                    //  continue;

                    //red_pos_matrix_from_index( parent_index, red_pos );
                    //GetRbStateFromRedPos( red_pos, &parent_state);

                    //GetRedGraphPath1(parent_index,path,&len);
                    //PrintFull(&state);
                    GetRedPosFromRbState(&parent_state, red_pos_parent);
                    calc_child_index_red_pos(red_pos_parent, parent_index );
                    CubesRedGraphNeighb[icube][vertex].level = GetRedGraphLevel(parent_index);
                    for(i=0;i<6;i++)
                    {
                        GetChild(&parent_state,&child_state,2*i);
                        GetRedPosFromRbState(&child_state, red_pos_child);
                        calc_child_index_red_pos(red_pos_child, child_index );
                        redGrChild = GetRedGroup(child_index);
                        //redGrChild = 0;
                        //PrintFull(&child_state);
                        curNode.state = child_state;
                        printf("cube:%d, vertex:%2d==",icube,vertex);
                        //printf("%2d,%2d,%2d,%2d,%2d",i0,i1,i2,i3,i4);
                        printf("::%2d,%2d,%2d,%2d,%2d",child_index[0],child_index[1],
                                child_index[2],child_index[3],child_index[4]);
                        printf("::move=%d::",i);
                        cubeMove.cube = icube;
                        cubeMove.vertex = vertex;
                        cubeMove.move = i;
                        cubeMove.stateChild = child_state;
                        cubeMove.stateParent = parent_state;
                        Insert();
                    }
                    printf("FinalNodeCount=%8d  ",FinalNodeCount);
                    printf("new red graphs=%6d  ",FinalNodeCount-InsertGraphCount);
                    printf("insertgraphcount=%8d\n",InsertGraphCount);
                    printf("redGroups: %d\n",redGroupCount);
                    //if((FinalNodeCount-InsertGraphCount) == 1297)
                    //  return 0;
                }
            }
            printf("redGroups: %d\n",redGroupCount);
            for(icube=0;icube<20736;icube++)
            {
                for(vertex=0;vertex<32;vertex++)
                {
                    printf("%6d%4d(%3d)",icube,vertex,CubesRedGraphNeighb[icube][vertex].level);
                    for(i=0;i<6;i++)
                    {
                        printf("%5d(%3d)", CubesRedGraphNeighb[icube][vertex].child[i].redGraphId,
                                CubesRedGraphNeighb[icube][vertex].child[i].level);
                    }
                    printf("\n");
                }
            }

            return 0;
        case 1:
            break;
        case 2:
        default:
            return 0;
    }

    printf("initializing epostable=%d\n",EPosTableSize);
    for(i=0;i<EPosTableSize;i++)
        EPosTable[i]=BAD_INDEX;/*12!/6*/
	time(&now);
	curTime = localtime(&now);
	printf("before H:M:S %02d:%02d:%02d\n",curTime->tm_hour,curTime->tm_min,curTime->tm_sec);
    calc_graph(Graph,GOAL_NODE,&GoalState);

    printf("*** addgr=%d***\n",addgr);
	time(&now1);
	curTime1 = localtime(&now1);
	printf("before H:M:S %02d:%02d:%02d\n",curTime->tm_hour,curTime->tm_min,curTime->tm_sec);
	printf(" after H:M:S %02d:%02d:%02d\n",curTime1->tm_hour,curTime1->tm_min,curTime1->tm_sec);
	printf(" epg1cnt=%d\n",epg1cnt);
	printf(" epg2cnt=%d\n",epg2cnt);
	printf(" cpg1cnt=%d\n",cpg1cnt);
	printf(" edircnt=%d\n",edircnt);

	int wLoop=0;
    Gernerate17Grid();
    return 0;
	while(wLoop<400)
    {
        wLoop++;
        unsigned char problem_red_pos[NGROUPS][GROUPSIZE];
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
                break;
            case 1:
                printf("Enter config type choice\n");
                printf("0 normal config\n");
                printf("1 red graph Config\n");
                choice1 = 0;
                scanf("%d",&choice1);
                switch(choice1)
                {
                    case 0:
                        printf("Enter random state distance\n");
                        dist = 2*nLevels+10;
                        scanf("%d",&distInt);
                        dist = distInt;
                        GetRandRbState(&problemState,dist);
                        printf("Got random state at dist:%d\n",dist);
                        break;
                    case 1:
                        printf("Enter random state distance\n");
                        scanf("%d",&dist11);
                        dist=dist11;
                        printf("enter red distance\n");
                        scanf("%d",&d1);
                        GetRedGraphNodeAtLevel( parent_index,d1);
                        //printf("%d,%d,%d,%d,%d\n");
                        red_pos_matrix_from_index( parent_index, problem_red_pos );
                        GetRbStateFromRedPos(problem_red_pos, &problemState);
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
        printf("searching node in the Graph\n");
        /*TODO no need to search for each i.
          Enough to search with i that is root for that red graph
          No. we need to search each node*/
        time(&now);
        curTime = localtime(&now);
        printf("H:M:S %02d:%02d:%02d\n",curTime->tm_hour,curTime->tm_min,curTime->tm_sec);
        if(Find16(&problemState,&i))
        //for(i=0;i<curGraphNodePtr;i++)
        {
            //GetChildByConfig(&problemState,&Graph[i].state,&searchState);
            //curNode.state = searchState;
            //found = Find(&searchState);
            //if ( found == 1 || found ==2)
            int PathFinal[60],lnFin=0;
            rbCubeState s11,s12,child;
            s11 = problemState;
            printf("\n*** Exact Match Found *** i=%d found=%d\n\n",i,found);
            //continue;

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
                //return 0;
            }


        }
        else
        {
            printf("\n*** match node not found ***\n\n");
            //ExploreTripod(problemState);
            //return 0;
        }
        printf("H:M:S %02d:%02d:%02d\n",curTime->tm_hour,curTime->tm_min,curTime->tm_sec);
        time(&now);
        curTime1 = localtime(&now);
        printf("H:M:S %02d:%02d:%02d\n",curTime1->tm_hour,curTime1->tm_min,curTime1->tm_sec);
    }
	FreeGraphs();
	return 0;
}


/*
list all the 8 tripods configs

list all tripods(4!*3^4*3!*2^3) that have path of length less than o equal to 14 to Goal

If problem node is at a distance greater than 14 from Goal
   from problem node, find if there exists a path of length<=14 to any tripod node

--- for a node at distance 14 from Goal, how many paths of length 14 are there to Goal?
--- how many neighbours does a reduced graph have --- 1296?
--- connections between two neighbouring reduced graphs: (663552*6)/1296 = 512*6 = 3072*2(if we treat green-in and green-out different) = 6144
--- number of vertices in a reduced graph: 663552
--- there are 12 out-edges(going to another reduced graph) from each vertex  - but these 12 edges land on 6 reduced graphs

Full = (8! * 3^8 * 12! * 2^12 )/12
Red = 24^4 * 2  = 8^4 * 3^4 /2 = 2^12 * 3^4 /2

(Full / Red) = 8! * 3^4 * 12! / 6 = 4 * 7! * 3^3 * 12! = 260730150912000 = 2.6 * 10 ^ 14

For any two nodes in a reduced Graph, is it possible to have a path(through the full Graph)
between them that is shorter than the shortest path through the reduced graph itself

What is the diameter of the "Graph of reduced graphs" = the Graph in which each reduced graph
is treated as a vertex. The degree of this Graph is 1296. Means each reduced Graph has 1296 neighbours
diameter = shortest distance between farthest nodes

what is the relation among the 6144 nodes of a reduced graph which go to same reduced graph

Graph has degree 1296
vertex transitive
2.6 * 10 ^ 14 vertices = 12! * 7! * 3^3 * 4


what are those 6144 connections between two reduced graphs:


A, B, C, D, E

if D = A+C and E = B+C
then
A - B = D - E

A - B : sequence of moves required to go from A to B

Let's say SeqC is the sequence of moves required to go from Goal to C, then
A + C is obtained by applying the moves in SeqC to A

Reduced Graph has 1296 neighbours.
What are those?
  432 - green, blue
  432 - pink, red
  432 - yellow, black
Why only 1296?
How many neighbour reduced graphs from a "cube"?  48

*******************************************************************************
Write a prune function:
   take a node obtained after making n(>60) moves from Goal
   in the 60 move sequence, check if the shortest distance between any two points
   that are more than 16 apart is less than 16. If so, modify the sequence
   repeat until no more reduction can be made
*******************************************************************************
Generate 17 Grid
   1. Generate node at distance 17 from Goal.
   2. confirm it doesn't have path of length 16 or less from Goal, if not go to step 1
   3a. confirm it doesn't have path of length 8 or less from any node in the SAVED_LIST, if not go to step 1
   OR
   3b. confirm it doesn't have path of length 16 or less from any node in the SAVED_LIST, if not go to step 1
   4. save the node and its path to Goal to the SAVED_LIST
   5. go to step 1
******************************************************************************
*/
