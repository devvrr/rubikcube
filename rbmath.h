UINT32 Comb(UINT8 n, UINT8 r );
UINT32 Perm(UINT8 n, UINT8 r );
UINT32 Factorial( UINT8 n );
/*calculates the combination from the index*/
void GetCombFromIndex( UINT32 index, UINT8 * comb, UINT8 n, UINT8 r );

/*returns index of the combination*/
UINT32 GetIndexFromComb( UINT8 * comb, UINT8 n, UINT8 r );

void GetPermFromIndex( UINT8 * perm, UINT32 index, UINT8 nObjects );
UINT32 GetIndexFromPerm(UINT8 * perm, UINT8 nObjects);
void GetPermNormalForm(UINT8 * perm, UINT8 nPerm);
void RadixTenToN( UINT8 * numRadixN, UINT16 numRadixTen, UINT8 radix, UINT8 nObjects );
UINT16 RadixNToTen( UINT8 * numRadixN, UINT8 radix, UINT8 nObjects );


