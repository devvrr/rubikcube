void GetChild( rbCubeState * parent, rbCubeState * child, UINT8 move );
unsigned char GetRedGraphGroupId(rIndex id);
unsigned char GetRedGraphGroupIdNew(rIndex id);
int GetRedGraphPath(rIndex problemId,rIndex goalId,UINT8 * path, UINT8 * len);
int GetRedGraphPath1(rIndex problemId,UINT8 * path, UINT8 * len);



void GetRandRbStatePath1(rbCubeState *state, UINT8 dist, UINT8 path[] );
void GetRandRbState(rbCubeState * state, UINT8 dist );
void GetRandRbStatePath(rbCubeState states[], UINT8 dist, UINT8 path[] );
void GetRandRbPath(rbCubeState * state, UINT8 dist, rbCubeState * Path );
void GetChildByConfig( rbCubeState * SfromGoal, rbCubeState * Ssource,  rbCubeState * Sdest );
void GetRandRbRedState(rbCubeState * state, UINT8 dist,int d1 );
int ExploreTripod( rbCubeState problemState );
void GenerateAllRedGraphs(void);
void red_pos_matrix_from_index( rIndex parent_index, unsigned char parent_pos[NGROUPS][GROUPSIZE] );
void GetRbStateFromRedPos(unsigned char red_pos[NGROUPS][GROUPSIZE], rbCubeState* state);
void GetRedPosFromRbState(rbCubeState * state, unsigned char red_pos[NGROUPS][GROUPSIZE]);
void calc_child_index_red_pos(unsigned char child_pos[NGROUPS][GROUPSIZE], rIndex child_index );
int GetRedGroup(rIndex id);
void GenerateCubes();
void GetCubeVertexState(unsigned int cube, unsigned short int vertex,rbCubeState *parent_state);
int GetRedGraphLevel(rIndex problemId);
void GetRedGraphNodeAtLevel(rIndex problemId, int level);
unsigned char CompareFullStatesT( rbCubeState * s1, rbCubeState * s2);
