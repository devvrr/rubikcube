/*Implements the Rubik cube configuration difference function*/
#include "../rbdefs.h"

/*difference of corner cubelet direction values
 * diff_dir = dirMap[c2_dir][c1_dir]*/
int dirMap[3][3] = {{0,1,2},{2,0,1},{1,2,0}};

/*The Rubik Cube configuration difference function:
 * The difference of two configs is such that
 * relative placement of cubelets in "diff" relative to "Goal" is same as the relative
 * placement of cubelets in "c2" compared to "c1"*/
void RbCubeConfigDiff( rbCubeState* config1, rbCubeState* config2, rbCubeState* diff)
{
    int i,j;
    UINT8 edgeDir[12];
    UINT8 cornerDir[8];
    for( i=0; i<12; i++)
    {
        for(j=0;j<12;j++)
        {
            if( config1->edgePos[i] == config2->edgePos[j] )
            {
                diff->edgePos[j] = i;
                if( (config1->edgePos[i]>>4) == (config2->edgePos[j]>>4) )
                    edgeDir[j] = 0;
                else
                    edgeDir[j] = 1;
                break;
            }
        }
    }
    for(i=0;i<12;i++)
        diff->edgePos[i] |= (edgeDir[i]<<4);

    for( i=0; i<8; i++ )
    {
        for( j=0; j<8; j++)
        {
            if( config1->cornerPos[i] == config2->cornerPos[j] )
            {
                diff->cornerPos[j] = i;
                cornerDir[j] = dirMap[config1->cornerPos[i]>>3][config2->cornerPos[j]>>3];
                break;
            }
        }
    }
    for(i=0;i<8;i++)
        diff->cornerPos[i] |= (cornerDir[i]<<3);
}
void PrintEdgeColors( void )
{
   printf("\n              rg  rb  gp  pb  yb  gy  gw  wb  py  pw wr  ry");
}
void PrintCornerColors( void )
{
   printf("\n\n             gpy rgw bpw ryb rwb rgy wgp ybp");
}
#if 0
static void PrintRbCubeConfig( rbCubeState * s )
{
   UINT8 i;
   printf("the config is\n");
   PrintEdgeColors();
   printf("\nedge pos:  ");
   for(i=0;i<12;i++)
      printf("%4d",(s->edgePos[i]));
   printf("\nedge dir:  ");
   for(i=0;i<12;i++)
      printf("%4d",(s->edgeDir[i]));

   PrintCornerColors();
   printf("\ncorner pos:");
   for(i=0;i<8;i++)
      printf("%4d",(s->cornerPos[i]));
   
   printf("\ncorner dir:");
   for(i=0;i<8;i++)
      printf("%4d",(s->cornerDir[i]));
}
static void ReadRbCubeConfig( rbCubeState * c )
{
   UINT8 i;
   int edgePos,edgeDir,cornerPos,cornerDir;
   printf("enter edge pos values\n");
   for(i=0;i<12;i++)
   {
	   scanf("%d",&edgePos);
	   c->edgePos[i] = edgePos;
   }
   printf("enter edge dir values\n");
   for(i=0;i<12;i++)
   {
	   scanf("%d",&edgeDir);
	   c->edgeDir[i]=edgeDir;
   }
   printf("enter corner pos values\n");
   for(i=0;i<8;i++)
   {
	   scanf("%d",&cornerPos);
	   c->cornerPos[i]=cornerPos;
   }
   printf("enter corner dir values\n");
   for(i=0;i<8;i++)
   {
	   scanf("%d",&cornerDir);
	   c->cornerDir[i]=cornerDir;
   }
}

void main( void )
{
   rbCubeState c1, c2, diff;
   printf("config 1\n");
   ReadRbCubeConfig(&c1);
   printf("config 2\n");
   ReadRbCubeConfig(&c2);
   RbCubeConfigDiff(&c1, &c2, &diff);

   printf("\n\nthe difference config is:\n");
   PrintRbCubeConfig( &diff );
}
#endif
