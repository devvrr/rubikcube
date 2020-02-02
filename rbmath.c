#include <stdio.h>
#include "rbdefs.h"
const static UINT32 factorial[13] = 
   {/*0!*/1,
    /*1!*/1,
    /*2!*/2,
    /*3!*/6,
    /*4!*/24,
    /*5!*/120,
    /*6!*/720,
    /*7!*/5040,
    /*8!*/40320,
    /*9!*/362880,
    /*10!*/3628800,
    /*11!*/39916800,
    /*12*/479001600
   };
void swap(UINT8 * x, UINT8 * y)
{
   int tmp;
   tmp = *x;
   *x = *y;
   *y = tmp;
}

void sort(UINT8 * A, UINT8 size)
{
   UINT8 j,k,min,minpos;

   for( j=0; j < (size-1); j++ )
   {
      min = A[j];
      minpos = j;
      for( k = j + 1; k < size; k++)
         if( A[k] < min )
	 {
	    minpos = k;
	    min = A[k];
	 }

      if( minpos != j )
         swap(&(A[j]),&(A[minpos]));
   }
}
UINT32 Comb( UINT8 n, UINT8 r )
{
    /*      n!  
    *   -------------
    *   r! * (n - r)! */
   return ( ( factorial[n] ) / ( factorial[r] * factorial[n-r] ) );
}

UINT32 Perm( UINT8 n, UINT8 r )
{
   /*     n!  
    *   ------
    *   (n - r)! */
   return ( ( factorial[n] ) / ( factorial[n-r] ) );
}

UINT32 Factorial( UINT8 n )
{
   if( n > 12 )
      return 0;

   return factorial[n];
}

/*calculates the combination from the index*/
void GetCombFromIndex( UINT32 index, UINT8 *comb, UINT8 n, UINT8 r )
{
   UINT8 m,pos,posMin=0;
   for(pos=0; pos<r; pos++, posMin++)
   {
      //comb[pos] = posMin;
      m = n - 1 - posMin;
      while( index >= 0 )
      {
	 if( index < Comb(m,r - 1 - pos ) )
	 {
	    comb[pos] = posMin;
	    break;
	 }
	 else
	 {
	    index -= Comb(m, r - 1 - pos );
	    posMin++;
	    m--;
	 }
      }
   }
}

/*returns index of the combination*/
UINT32 GetIndexFromComb( UINT8 * comb, UINT8 n, UINT8 r )
{
   UINT32 index=0;
   UINT8 posMin = 0;
   UINT8 nM1 = n - 1;
   UINT8 rM1 = r - 1;
   UINT8 mMax, mMin, pos,s,m;

   sort(comb, r);
   for( pos = 0; pos < r; pos++, posMin++ )
   {
      if( comb[pos] - posMin )
      {
	 s = rM1 - pos;
	 mMax = nM1 - posMin;
	 mMin = nM1 - comb[pos];
	 for( m = mMax; m > mMin; m-- )
	    index += Comb(m,s);
      }
      posMin = comb[pos];
   }
   return index;
}
void print_comb(UINT8* comb,UINT8 r)
{
   UINT8 i;
   for(i=0;i<r;i++)
      printf("%d",comb[i]);
   printf("  ");
}
/*least permutable item remaining*/
unsigned char i_th_least_rem(UINT8 * perm, UINT8 i, UINT8 k, UINT8 nObjects)
{
   UINT8 j, least=0, rem[nObjects],count=0;
   for(j=0;j<nObjects;j++)
      rem[j] = nObjects + 1;

   for(j=0;j<i;j++)
      rem[perm[j]] = 0;

   for(j=0;j<nObjects;j++)
      if(rem[j] == (nObjects + 1) )
      {
         if( count == k )
            return j;
         count++;
      }

	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
	return 0;
}

void GetPermFromIndex( UINT8 * perm, UINT32 index, UINT8 nObjects )
{
   unsigned char j;
   //unsigned char perm_tmp[N];
   for(j=0;j<nObjects;j++)
   {
      perm[j] = i_th_least_rem(perm,j, (index/factorial[nObjects-1-j]) , nObjects);
      //printf("%d",perm_tmp[j]);
      index = index%factorial[nObjects-1-j];
   }
   //printf("\n");
}


UINT32 diff( UINT8 * perm, UINT32 index, UINT8 nObjects )
{
   UINT32 j,difference=0;
   for(j=index+1;j<nObjects;j++)
      if( perm[j] < perm[index] )
         difference++;
   return difference;
}

UINT32 GetIndexFromPerm(UINT8 * perm, UINT8 nObjects)
{
   UINT32 index=0,i;
   for(i=0;i<nObjects;i++)
   {
      index = index +  (diff(perm,i,nObjects)*factorial[nObjects-i-1]);
   }
   return index;
}
/*map an arbitrary permutation to 0 .. nPerm - 1
 * here arbitrary permutation means a permutation of nPerm numbers which may not be
 * from 0 to nPerm - 1
 * e.g 3456 is mapped to 0123
 * 3647 -> 0213*/
void GetPermNormalForm(UINT8 * perm, UINT8 nPerm)
{
   UINT8 permTmp[nPerm],i,j;

   //PrintArray(perm,nPerm);

   for( i=0; i < nPerm; i++ )
      permTmp[i] = perm[i];

   //PrintArray(permTmp,nPerm);
   
   sort(permTmp,nPerm);

   //PrintArray(permTmp,nPerm);

   for(i=0;i<nPerm;i++)
   {
      for(j=0;j<nPerm;j++)
      {
	 if( perm[i] == permTmp[j] )
	 {
	    perm[i] = j;
	    break;
	 }
      }
   }

   //PrintArray(perm,nPerm);
   //printf("\n");
}
void RadixTenToN( UINT8 * numRadixN, UINT16 numRadixTen, UINT8 radix, UINT8 nObjects )
{
   INT8 i;
   //printf("numRadixTen=%d\n",numRadixTen);
   for(i=nObjects-1;i>=0 && numRadixTen;i--)
   {
      numRadixN[i] = numRadixTen % radix;
      numRadixTen /= radix;
   }
   for(;i>=0;i--)
      numRadixN[i] = 0;
   //for(i=0;i<nObjects;i++)
//	   printf("%d",numRadixN[i]);
  // printf("\n");
}
UINT16 RadixNToTen( UINT8 * numRadixN, UINT8 radix, UINT8 nObjects )
{
   INT8 i;
   UINT16 numRadixTen=0,radixPower=1;
   //for(i=0;i<nObjects;i++)
//	   printf("%d",numRadixN[i]);
   for(i=nObjects-1;i>=0;i--)
   {
      numRadixTen += ( numRadixN[i] * radixPower);
      radixPower *= radix;
   }
  // printf("numRadixTen=%d\n",numRadixTen);
   return numRadixTen;
}
//#if 0
void CombTest( void )
{
   UINT8 i,comb[4];
   for(i=0;i<70;i++)
   {
      printf("%4d:  ",i);
      GetCombFromIndex(i,comb,8,4);
      print_comb(comb,4);
      printf("index=%4d\n", GetIndexFromComb(comb,8,4) );
      
   }
}
void PermTest( void )
{
   UINT8 i,perm[5];
   for(i=0;i<120;i++)
   {
      printf("%4d:  ", i );
      GetPermFromIndex(perm,i,5);
      print_comb(perm,5);
      printf("index=%4d\n", GetIndexFromPerm(perm,5) );
   }
}
void RadixConvTest( void )
{
   UINT8 i,numRadixN[5];
   for(i=0;i<120;i++)
   {
      printf("%4d:  ", i );
      RadixTenToN(numRadixN,i,3,5);
      print_comb(numRadixN,5);
      printf("index=%4d\n", RadixNToTen(numRadixN,3,5) );
   }
}
#if 0
int main( void )
{
 
   RadixConvTest();
   return 0;
}
#endif
