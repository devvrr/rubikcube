void GetChild( rbCubeState * parent, rbCubeState * child, UINT8 move );
unsigned char GetRedGraphGroupId(rIndex id);
int GetRedGraphPath(rIndex problemId,rIndex goalId,UINT8 * path, UINT8 * len);

void GetRBStateCompFromFull( rbCubeState * sFull, rbCompState * sComp );
void GetRBStateFullFromComp(rbCompState * sComp, rbCubeState * sFull );
Boolean IsCompStateEqual( rbCompState * s1, rbCompState * s2 );
void GetChildComp1( rbCubeState * parent, rbCompState * child, UINT8 move );
void GetRandRbState(rbCompState * state, UINT8 dist );
void GetRandRbPath(rbCubeState * state, UINT8 dist, rbCompState * Path );
void GetChildByConfig( rbCubeState * SfromGoal, rbCubeState * Ssource,  rbCubeState * Sdest );
void GetRandRbRedState(rbCompState * state, UINT8 dist,int d1 );
