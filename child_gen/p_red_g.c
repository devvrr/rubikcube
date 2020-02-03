
/*
12-sep-2019
find child for given seq of moves in red graph
2 1 0 2 0 1

 5,  2,  0,  4, 
 5,  4,  2,  0, 

5,  3,  0,  4, 
 5,  4,  3,  0, 

 3,  2,  1, 
 2,  1,  3, 
*/

/*reduced cube(only double turns) state space graph: efficient method:
 nodes stored at the position of their permutation in the graph */
#include <stdio.h>
#include <stdlib.h>
#include "../rbdefs.h"

#define NGROUPS 5
#define GROUPSIZE 4
#define N 4
#define POWER24_5 7962624
#define BAD_INDEX -1


/*reduced cube(only double turns) state space graph*/




struct config_tag{
	unsigned char valid/*:1*/;/*tells whether this config is valid or not*/
	unsigned char level/*:4*/;
	unsigned char parent_move/*:3*/;/*what move was made at the parent to reach this child*/
	unsigned char redGraphGroup/*:4*/;
	rIndex parent;/*index where the parent config is located in the config array*/
};
typedef struct config_tag config;

config goal = { 1, 0, 0, 0,{0,0,0,0,0} };

unsigned char redGroups[POWER24_5];
#if 1
/*Maximum configs  */
config configs[24][24][24][24][24];
rIndex sequence[24*24*24*24*24];
unsigned int sequence_ptr;/*node being currently explored*/

unsigned char redGraphGroup;


unsigned int config_count = 1;/*also equal to sequence count*/

void PrintrIndex(rIndex cf)
{
    int i;
    for(i=0;i<5;i++)
        printf("%3d",cf[i]);
    printf("\n");
}
void TraceToGoal(rIndex node, FILE * df1,int par_mov[],int *pmcnt)
{
    int i;
    rIndex nd;
    for(i=0;i<5;i++)
       nd[i] = node[i];

    config cf = configs[nd[0]][nd[1]][nd[2]][nd[3]][nd[4]];
    while(cf.level)
    {
        //    fprintf(df1,"parent_move %d %d\n",configs[node[0]][node[1]][node[2]][node[3]][node[4]].parent_move,);
        //fprintf(df1,"config (%2d,%2d,%2d,%2d,%2d) parent_move %d level %d\n",nd[0],nd[1],nd[2],nd[3],nd[4],cf.parent_move,cf.level);
        par_mov[(*pmcnt)++]=cf.parent_move;
        for(i=0;i<5;i++)
            nd[i] = cf.parent[i];
        cf = configs[nd[0]][nd[1]][nd[2]][nd[3]][nd[4]];
    }
    //fprintf(df1,"trace complete\n");
}
void Reverse(int A[],int len)
{
    int i,temp;
    for(i=0;i<len/2;i++)
    {
        temp = A[i];
        A[i] = A[len-i-1];
        A[len-i-1]=temp;
    }
}
void CheckPathstoGoal(rIndex parent, rIndex child_index, FILE * df1,int move)
{
    int move_list1[20],move_list2[20],len1=0,len2=0;
    int need_to_print =0,i;
    TraceToGoal(parent,df1,&move_list1[1],&len1);
    move_list1[0] = move;
    len1++;
    Reverse(move_list1,len1);
    if (len1>=2 && (move_list1[len1-2]==move_list1[len1-1]))
        len1-=2;
    TraceToGoal(child_index,df1,move_list2,&len2);
    Reverse(move_list2,len2);
    /*if ( len1 < len2 )
      {
      move_list1[len1++] = move;
      }
      else if( len2 < len1 )
      {
      move_list2[len2++] = move;
      }
      else
      fprintf(df1,"error both paths are of same length\n");*/
    if (len1!=len2)
        need_to_print = 1;
    else
    {
        for(i=0;i<len1-2;i++)
            if(move_list1[i] != move_list2[i])
            {
                need_to_print = 1;
                break;
            }
        if(need_to_print==0 && len1>2)
        {
            if(move_list1[len1-1]!= move_list2[len1-2] ||
                    move_list1[len1-2]!= move_list2[len1-1] )
                need_to_print =1;

        }
    }
    if(need_to_print==1)
    {
        for(i=0;i<len1;i++)
            fprintf(df1,"%2d, ",move_list1[i]);
        fprintf(df1,"\n");
        for(i=0;i<len2;i++)
            fprintf(df1,"%2d, ",move_list2[i]);
        fprintf(df1,"\n");
    }
}
void red_pos_matrix_from_index( rIndex parent_index, unsigned char parent_pos[NGROUPS][GROUPSIZE] )
{
   unsigned char i;
   for(i=0;i<NGROUPS;i++)
      GetPermFromIndex(parent_pos[i],parent_index[i],N);
}


void calc_child_index_red_pos(unsigned char child_pos[NGROUPS][GROUPSIZE], rIndex child_index )
{
   unsigned char i;
   for(i=0;i<NGROUPS;i++)
      child_index[i] = GetIndexFromPerm(child_pos[i],N);
}

/*Finds the child configuration in the reduced graph for a
given parent and move number
and puts it into the given config*/

void find_child_red_pos( rIndex parent_index, unsigned char move, rIndex child_index )
{

   unsigned int i,j;
   unsigned char child_pos[NGROUPS][GROUPSIZE],c;
   unsigned char parent_pos[NGROUPS][GROUPSIZE];
   /*five groups each with four positions(cubelets)*/
   
   red_pos_matrix_from_index( parent_index, parent_pos );

   for(i=0;i<NGROUPS;i++)
      for( j=0;j<GROUPSIZE;j++)
      child_pos[i][j] = parent_pos[i][j];

   switch( move ){
      case (GREEN_IN/2): /*green*/
      //case 0: /*green*/
         child_pos[0][2] = parent_pos[0][0];
         child_pos[0][0] = parent_pos[0][2];

         child_pos[1][1] = parent_pos[1][2];
         child_pos[1][2] = parent_pos[1][1];

         child_pos[3][1] = parent_pos[3][0];
         child_pos[3][0] = parent_pos[3][1];

         child_pos[4][2] = parent_pos[4][1];
         child_pos[4][1] = parent_pos[4][2];
         break;
      case (BLUE_IN/2): /*blue*/
         child_pos[0][1] = parent_pos[0][3];
         child_pos[0][3] = parent_pos[0][1];

         child_pos[1][3] = parent_pos[1][0];
         child_pos[1][0] = parent_pos[1][3];

         child_pos[3][3] = parent_pos[3][2];
         child_pos[3][2] = parent_pos[3][3];

         child_pos[4][0] = parent_pos[4][3];
         child_pos[4][3] = parent_pos[4][0];
         break;
      case (RED_IN/2): /*red*/
         child_pos[0][1] = parent_pos[0][0];
         child_pos[0][0] = parent_pos[0][1];

         child_pos[2][3] = parent_pos[2][2];
         child_pos[2][2] = parent_pos[2][3];

         child_pos[3][1] = parent_pos[3][3];
         child_pos[3][3] = parent_pos[3][1];

         child_pos[4][0] = parent_pos[4][1];
         child_pos[4][1] = parent_pos[4][0];
         break;
      case (PINK_IN/2): /*pink*/
         child_pos[0][2] = parent_pos[0][3];
         child_pos[0][3] = parent_pos[0][2];

         child_pos[2][0] = parent_pos[2][1];
         child_pos[2][1] = parent_pos[2][0];

         child_pos[3][2] = parent_pos[3][0];
         child_pos[3][0] = parent_pos[3][2];

         child_pos[4][3] = parent_pos[4][2];
         child_pos[4][2] = parent_pos[4][3];
         break;
      case (WHITE_IN/2): /*white*/
         child_pos[1][3] = parent_pos[1][2];
         child_pos[1][2] = parent_pos[1][3];

         child_pos[2][2] = parent_pos[2][1];
         child_pos[2][1] = parent_pos[2][2];

         child_pos[3][1] = parent_pos[3][2];
         child_pos[3][2] = parent_pos[3][1];

         child_pos[4][0] = parent_pos[4][2];
         child_pos[4][2] = parent_pos[4][0];
         break;
      case (YELLOW_IN/2): /*yellow*/
         child_pos[1][0] = parent_pos[1][1];
         child_pos[1][1] = parent_pos[1][0];

         child_pos[2][0] = parent_pos[2][3];
         child_pos[2][3] = parent_pos[2][0];

         child_pos[3][0] = parent_pos[3][3];
         child_pos[3][3] = parent_pos[3][0];

         child_pos[4][1] = parent_pos[4][3];
         child_pos[4][3] = parent_pos[4][1];
         break;
      default:
         printf("error: invalid move=%d\n",move);
         break;
   }
   calc_child_index_red_pos( child_pos, child_index );
}
void print_graph(void)
   {
   unsigned char i,j,k,l,m;
   int fac1=24,fac2=24*24,fac3=24*24*24,fac4=24*24*24*24;
   FILE * graph = fopen("graph.txt","w");
   FILE * fRedGroup = fopen("redGroup.txt","w");

   for( i=0;i< 24;i++)
      for( j=0;j<24;j++)
         for( k=0;k<24;k++)
            for( l=0;l<24;l++)
               for( m=0;m<24;m++)
		       if(configs[i][j][k][l][m].valid != 0)
      {
         fprintf(graph,"%7d",m+fac1*l+fac2*k+fac3*j+fac4*i);
         fprintf(graph,"%4d",configs[i][j][k][l][m].level);
	     fprintf(graph,"%6d%3d%3d%3d%3d",i,j,k,l,m);
         fprintf(fRedGroup,"%4d\n",configs[i][j][k][l][m].redGraphGroup);
         fprintf(graph,"%4d\n",configs[i][j][k][l][m].redGraphGroup);
      }
      else
	 {
	    //printf("error invalid config\n");
	 }

   fclose(graph);
}

UINT8 find_child( rIndex parent, UINT8 move, rIndex child, config * child_config )
{
   unsigned char i;
   find_child_red_pos( parent, move, child );
   if( configs[child[0]][child[1]][child[2]][child[3]][child[4]].valid )
      return 0;
   for(i=0;i<NGROUPS;i++)
      child_config->parent[i] = parent[i];
   child_config->parent_move = move;
   
   child_config->level = configs[parent[0]][parent[1]][parent[2]][parent[3]][parent[4]].level +
	   1;
   child_config->valid = 1;
   child_config->redGraphGroup = redGraphGroup;
   configs[child[0]][child[1]][child[2]][child[3]][child[4]] = *child_config;

   return 1;
}
void calc_configs( void )
{
   rIndex parent,child_index;
   unsigned char move,i;
   config child_config;
   unsigned char curr_level = 0;
   unsigned int level_count = 1;

   FILE * df = fopen("diaRed.txt","w");
   FILE * df1 = fopen("RedGraph3.txt","w");


   for(sequence_ptr=0;sequence_ptr < config_count;sequence_ptr++)
   {
      for(i=0;i<NGROUPS;i++)
         parent[i] = sequence[sequence_ptr][i];
      for( move=0;move<6;move++)
      {
         if(!find_child( parent, move, child_index, &child_config ))
         {
            fprintf(df1,"config exists. level %d parent %d child_index %d move %d\n",curr_level,sequence_ptr,config_count,move);
            CheckPathstoGoal(parent,child_index,df1,move);
            //TraceToGoal(parent,df1);
            //TraceToGoal(child_index,df1);
            continue;
         }
         else
         {
         }

         if( curr_level != child_config.level )
         {
            fprintf(df,"level %d count = %d\n",curr_level,level_count);
            fflush(df);
            curr_level++;
            level_count = 0;
         }
         level_count++;
         for(i=0;i<NGROUPS;i++)
            sequence[config_count][i] = child_index[i];
         config_count++;

      }
   }
   fprintf(df,"level %d count = %d\n",curr_level,level_count);
   fclose(df);
   fclose(df1);
}
int GetPath(rIndex goalId,UINT8 * path, UINT8 * len)
{
   UINT8 i,level,Id[NGROUPS],j;
   rIndex Id1;
   if(configs[goalId[0]][goalId[1]][goalId[2]][goalId[3]][goalId[4]].valid != 1)
   {
      printf("error red graph\n");
      return 0;
   }
   else
   {
      printf("problem node found in red graph\n");
   }
   level = configs[goalId[0]][goalId[1]][goalId[2]][goalId[3]][goalId[4]].level;
   for(i=0;i<NGROUPS;i++)
      Id[i] = goalId[i];

   i=0;
   while(level>0 && i<16)
   {
      path[i] = configs[Id[0]][Id[1]][Id[2]][Id[3]][Id[4]].parent_move;
      printf("path[%d]=%d\n",i,path[i]);
      
      for(j=0;j<NGROUPS;j++)
         Id1[j] = configs[Id[0]][Id[1]][Id[2]][Id[3]][Id[4]].parent[j];
      level = configs[Id[0]][Id[1]][Id[2]][Id[3]][Id[4]].level;
      printf("level = %d\n",level);
      for(j=0;j<NGROUPS;j++)
	      Id[j] = Id1[j];
      i++;
   }
   *len = i;
   return 1;
}
void InitConfigs( void )
   {
   unsigned char i,j,k,l,m;
   unsigned int dia=0;

   for( i=0;i< 24;i++)
      for( j=0;j<24;j++)
         for( k=0;k<24;k++)
            for( l=0;l<24;l++)
               for( m=0;m<24;m++)
                  configs[i][j][k][l][m].valid = 0;


  // print_graph();
}
int GetRedGraphPath(rIndex problemId,rIndex goalId,UINT8 * path, UINT8 * len)
{
   UINT8 i;
   InitConfigs();
   configs[problemId[0]][problemId[1]][problemId[2]][problemId[3]][problemId[4]].valid = 1;
   configs[problemId[0]][problemId[1]][problemId[2]][problemId[3]][problemId[4]].level = 0;
   
   for(i=0;i<NGROUPS;i++)
      sequence[0][i] = problemId[i];
   printf("finding reduced graph\n");
   
   calc_configs();
   printf("config count = %d \n",config_count);

   printf("found reduced graph\n");
   
//   printf("problem node level = %d\n",configs[1][1][12][8][15].level);
   if( GetPath(goalId,path,len) )
	 return 1;
   return 0;
}
void GenerateRedGraph(rIndex problemId)
{
   UINT8 i;

   configs[problemId[0]][problemId[1]][problemId[2]][problemId[3]][problemId[4]].valid = 1;
   configs[problemId[0]][problemId[1]][problemId[2]][problemId[3]][problemId[4]].level = 0;
   configs[problemId[0]][problemId[1]][problemId[2]][problemId[3]][problemId[4]].redGraphGroup
      = redGraphGroup;
   
   for(i=0;i<NGROUPS;i++)
      sequence[0][i] = problemId[i];
   printf("finding reduced graph\n");
   
   calc_configs();
   printf("config count = %d \n",config_count);

   printf("found reduced graph\n");

}
void AssignId1(UINT8 * id)
{
   id[0] = 0;
   id[1] = 0;
   id[2] = 0;
   id[3] = 0;
   id[4] = 0;   
}
void AssignId2(UINT8 * id)
{
   id[0] = 7;
   id[1] = 21;
   id[2] = 9;
   id[3] = 17;
   id[4] = 10;
}
int FindFreeSlot( rIndex rId )
{
   unsigned char i,j,k,l,m;

   for( i=0;i< 24;i++)
      for( j=0;j<24;j++)
         for( k=0;k<24;k++)
            for( l=0;l<24;l++)
               for( m=0;m<24;m++)
		  if( configs[i][j][k][l][m].valid == 0 )
		  {
		     rId[0] = i;
		     rId[1] = j;
		     rId[2] = k;
		     rId[3] = l;
		     rId[4] = m;
		     printf("%d %d %d %d %d\n",i,j,k,l,m);
		     return 1;
		  }
   return BAD_INDEX;
}
void GenerateAllRedGraphs(void)
{
   int i,slot;
   rIndex rId;

   InitConfigs();
   
   for(i=0;i<12;i++)
   {
      if( FindFreeSlot(rId) == BAD_INDEX )
	 break;
      redGraphGroup = i;
      sequence_ptr = 0;
      config_count = 1;
      GenerateRedGraph(rId);
      printf("redGroup(0,6,21,21,5)%d\n",configs[0][6][21][21][5].redGraphGroup);
      printf("valid(0,6,21,21,5)%d\n",configs[0][6][21][21][5].valid);
   }
   printf("%d red graphs generated\n",i);
   print_graph();
}

#endif

int fac1=24;
int fac2=24*24;
int fac3=24*24*24;
int fac4=24*24*24*24;
unsigned char GetRedGraphGroupId(rIndex id)
{
    int k;
    int x = fac4*id[0]+fac3*id[1]+fac2*id[2]+fac1*id[3]+id[4];

    if( x >= POWER24_5 )
    {
        printf("error: x too large\n");
    }
    return redGroups[x];
}
unsigned char GetRedGraphGroupIdNew(rIndex id)
{
    int i;
    for(i=0;i<5;i++)
        if(id[i]>=24)
        {
            printf("*** error *** bad red graph d5 index %d\n",id[i]);
            return 0;
        }
    return configs[id[0]][id[1]][id[2]][id[3]][id[4]].redGraphGroup;
}
int i;
void PopulateRedGroups(char fileName[20])
{
   int group=0;
   FILE * gr = fopen(fileName,"r");
   if(!gr)
      printf("could not open file\n");
   for(i=0;i<POWER24_5;i++)
   {
      fscanf(gr,"%d",&group);
      redGroups[i] = (unsigned char)group;
      //printf("redGroups[%d]=%d\n",i,redGroups[i]);
      //printf("i=%d\n",i);
      //getchar();
   }
   printf("redGroup(0,6,21,21,5)%d\n",redGroups[fac4*0+fac3*6+fac2*21+fac1*21+5]);

   printf("populated\n");
}
#if 0
void PrintArray( UINT8 * a, UINT8 length)
{
   int i;
   printf("Array is:");
   for(i=0;i<length;i++)
      printf("%4d",a[i]);
   printf("\n");
}

typedef struct{
    int seq1[10];
    int seq2[10];
    int nMoves;
} Moves_t;
void Check(int MoveList[],int nMoves)
{
    rIndex parent={0,0,0,0,0},child;
    int i,k;
    for(i=0;i<nMoves;i++)
    {
        find_child_red_pos( parent, MoveList[i], child );
        PrintrIndex(child);
        for(k=0;k<5;k++)
            parent[k] = child[k]; 
    }

    printf("\n");
}
void CheckMany(void)
{
    int n;
    Moves_t MoveList[]={{{2,1,0},{1,0,2},3}};
    n = sizeof(MoveList)/sizeof(Moves_t);
    for(i=0;i<n;i++)
    {
        Check(MoveList[i].seq1,MoveList[i].nMoves);
        Check(MoveList[i].seq2,MoveList[i].nMoves);
    }
}
int main( void )
{
   
   rIndex id1,id2;
   UINT8 path[15],length=0,i;
   for(i=0;i<5;i++)
      id1[i] = 0;

   AssignId1(id1);
   AssignId2(id2);
//   printf("size of redgroups=%d\n",sizeof(redGroups));
  // printf("size of config=%d\n",sizeof(config));
   GenerateAllRedGraphs();
  //PopulateRedGroups("redGroup.txt");
   //GetRedGraphPath(id1,id2,path,&length);
   PrintArray(path,length);

   CheckMany();
   
   return 1;
}
#endif
