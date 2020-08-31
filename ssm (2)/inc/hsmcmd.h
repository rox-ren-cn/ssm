//=============================
// hsmcmd.h
// 
//=============================
#ifndef _HSMCMD_H
#define _HSMCMD_H
#include <string>

using namespace std;

typedef struct{
	string tag;
	string name;
	int (*pFunc)();
}CmdTypeDef;

//class HSMCmd defines virtual functions of HSM command processing
class HSMCmd{
public:
	virtual int Proc( const unsigned char *, const int, unsigned char *, int * ); //process command
};//===========================

#endif //_HSMCMD_H

