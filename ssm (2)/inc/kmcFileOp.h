//=============================
// kmcFileOp.h
// 
//=============================
#ifndef _KMCFILEOP_H
#define _KMCFILEOP_H
#include <iostream>
#include "flatfile.h"
#include "kmcKeyInfo.h"
using namespace std;

#define	MAXBUFFLEN	10240

class kmcFileOp{
protected:
	string	strFileName;
	string 	strOpMode;
	HexFile	FileHandle;
public:
kmcFileOp( string fileName, string opMode );

int OpenFile(void);
void Close(void);
int ReadKmcKeyDataFile( kmcKeyInfo *pKeyInfo );
/*
int ReadKmcKeyDataFileByLineNo( int iLineNo, kmcKeyInfo *pKeyInfo );
int DeleteKmcKeyDataByLineNo( int iLineNo, kmcKeyInfo *pKeyInfo );
int WriteKmcKeyDataFile( kmcKeyInfo *pKeyInfo );
*/
};

#endif //_KMCFILEOP_H

