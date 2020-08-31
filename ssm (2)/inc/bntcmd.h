//=============================
// bntcmd.h
// 
//=============================
#ifndef _BNTCMD_H
#define _BNTCMD_H

#include "hsmcmd.h"
#include "bntfmt.h"
using namespace std;

//THISAREKEYOFBULL
const string Km2bntxKey = "544849534152454B45594F4642554C4C";
const string KDKM = "544849534152454B45594F4642554C4C";
const string LicCtlKey = "544849534152454B45594F4642554C4C";
const string MD_SEC_BNT_HEADER_REQ =  "F1F2080000000000000001";
const string MD_SEC_BNT_CDE = "00";
const string MD_SEC_BNT_TK_KDK = "00";
const string MD_SEC_BNT_TK_KDKM = "90";
const string MD_SEC_BNT_TK_KM2BNTX = "98";

const string MD_SEC_BNT_TK_KT1 = "06";
const string MD_SEC_BNT_TK_KTK1 = "62";



const string MD_SEC_BNT_D3_DEFAULT = "D302FFFF";		//Key Usage
const string MD_SEC_BNT_DF22_DEFAULT = "DF220101";		//Output Key support:01-paper form
const string MD_SEC_BNT_DF24_DEFAULT = "DF240101";		//Input Key support:01-paper form
const string MD_SEC_BNT_DF14_KDK = "33";		//33-KDK File
const string MD_SEC_BNT_DF14_KSK = "34";		//33-KSK File

static int kFlag; //key load flag
static KeyIDFmtDef kid; //key id in SSM storage
static string LKey; //key value in SSM storage

//class HSMCmd defines virtual functions of HSM command processing
class BNTCmd:public HSMCmd{
private:
	//int (*pFunc)( const unsigned char *, const int, unsigned char *, int * );//pointer to member function
	//int GetFuncFP( string tag, string funcname, int (**ppFunc)() );
	int FB_TransPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FC_VerPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FD_VerCVX( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF28_GenPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF29_GenPVV( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF2A_GenCVV( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF64_PrntPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int E9_GenKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF23_KeyInput( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF22_OutPKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int FF21_TransToken( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int EB_ImpKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
	int EA_LoadKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen );
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
	static string EB_ImpKey_cmd( const string encKeyToken,
                              const string impKeyId,
                              const string impKeyKCV,
                              const string impKeyKEV,
                              const string stoKeyId );
	static string FF21_TransToken_cmd( const string keyToken, const string encKey );
	static string FF22_OutPKey_cmd( const string keyToken, const string keySplitMethod );
	static string FF23_KeyInput_cmd( const string keyId,
 																	const string stokeyId,
	                                const string keyPartMode,
	                                const string kevParts );
	static string FF09_GenKey_resp( const string resp );
	static string FF31_TransToken_resp( const string resp );
	static string FF33_InputKey_resp( const string resp );
};
#endif //_BNTCMD_H

