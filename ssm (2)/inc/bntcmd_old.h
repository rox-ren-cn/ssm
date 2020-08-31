//=============================
// bntcmd.h
// 
//=============================
#ifndef _BNTCMD_H
#define _BNTCMD_H

#include "hsmcmd.h"
#include "bntfmt.h"
using namespace std;

//THISISKEYOFBULL
const string Km2bntxKey = "5448495349534B45594F4642554C4C";
const string KDKM = "5448495349534B45594F4642554C4C";
const string MD_SEC_BNT_HEADER_REQ =  "F1F2080000000000000001";
const string MD_SEC_BNT_CDE = "00";
const string SSM_TK_KDKM = "90";
	
//class HSMCmd defines virtual functions of HSM command processing
class BNTCmd:public HSMCmd{
private:
	KeyIDFmtDef kid; //key id in SSM storage
	unsigned char key[17]; //key value in SSM storage
	//int (*pFunc)( const unsigned char *, const int, unsigned char *, int * );//pointer to member function
	//int GetFuncFP( string tag, string funcname, int (**ppFunc)() );
	int FB_TransPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FC_VerPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FD_VerCVX( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF28_GenPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF29_GenPVV( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF64_PrntPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int E9_GenKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF23_InpKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF22_OutPKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int EB_ImpKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int F4_LoadKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int F2_SelfTest( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	//int Init();
public:
	BNTCmd();
	~BNTCmd();
	int Proc( const unsigned char *, const int, unsigned char *, int * ); //process command
};//===========================

class KMCReq{
public:
	static string E9_GenKey_cmd( const string keyId, const string stokeyId );
	static string FF09_GenKey_resp( const string resp );
	
	static string FF21_TransToken_cmd( const string keyToken, const string encKey );
	static string FF31_TransToken_resp( const string resp );
};
#endif //_BNTCMD_H

