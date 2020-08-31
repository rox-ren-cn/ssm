//=============================
// hsmcmd.cpp
// 
//=============================
#include "hsmcmd.h"
#include "errlog.h"
#include "global.h"
#include <string>
#include <iostream>
using namespace std;
//-----------------------------
/*
HSMCmd::HSMCmd()
{
	for ( int i = 0; i < 64; i++ )
		cmdType[i].pFunc = NULL;
}
*/
//------------------------------------------------------------------
// Function   : Proc
// Description: process command
// Input      : pcReq - request message
//              iReqLen - request message length
// Output     : pcResp - response message
//              piRespLen - pointer to response message length
// Return     : 0 - success
//             -1 - failed
//------------------------------------------------------------------
int HSMCmd::Proc( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
    errLog.ErrLog( LOG_LEVEL_ERROR, "HSMCmd::Proc", 0, 0, 0);
    return -1;
}//=============================

