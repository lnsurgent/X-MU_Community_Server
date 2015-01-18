#include "stdafx.h"

///////////////////////////////////////////////////////////////////

void InitCrcTable();
ULONG Reflect(ULONG ref, char ch);
int Get_CRC(unsigned char* buffer, ULONG bufsize);
long FileSize(FILE *input);
int  GetCheckSumFromMain();