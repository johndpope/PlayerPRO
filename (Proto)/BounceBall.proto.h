
/* BounceBall.c */
pascal long GetVBLRec(void);
void RemoveBounceVBL(VBLTask *);
void InitBounceBall(short, short, short);
void DrawBall(long, long, long *);
void ClearBall(long, long, long);
//void PositionProcess(BounceRec *);
pascal void DoBounceBall(VBLTaskPtr);
void CloseBounceBall(void);
