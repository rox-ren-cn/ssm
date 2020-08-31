//=============================
// bntcmd.cpp
// 
//=============================
#include "bntfmt.h"
#include "bntcmd.h"
#include "hsmcmd.h" 
#include "sec.h" 
#include "errlog.h"
#include "global.h"
#include "commSerialPort.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
//-----------------------------

BNTCmd::BNTCmd()
{
	kFlag = 0;
}//-----------------------------

BNTCmd::~BNTCmd()
{
}//-----------------------------
/*
int BNTCmd::Init()
{
	CmdTypeDef bntcmdType[64];
	bntcmdType[0].tag = "FB";
	bntcmdType[0].name = "FB_TransPIN";
	bntcmdType[0].pFunc = FB_TransPIN;
	

	CmdTypeDef bntcmdType[] = {
			{"FB", "FB_TransPIN", FB_TransPIN},
			{"FC", "FC_VerPIN", FC_VerPIN},
			{"FD", "FD_VerCVX", FD_VerCVX},
			{"FF28", "FF28_GenPIN", FF28_GenPIN},
			{"FF29", "FF29_GenPVV", FF29_GenPVV},
			{"FF64", "FF64_PrntPIN", FF64_PrntPIN},
			{"E9", "E9_GenKey", E9_GenKey},
			{"FF23", "FF23_InpKey", FF23_InpKey},
			{"FF22", "FF22_OutPKey", FF22_OutPKey},
			{"EB", "EB_ImpKey", EB_ImpKey},
			{"", "", NULL}
		};


	for ( int i = 0; bntcmdType[i].tag.length() > 0; i++ )
	{
		cmdType[i] = bntcmdType[i];
	}

	return 0;
}//-----------------------------
*/

//------------------------------------------------------------------
// Function   : GetFuncFP
// Description: get function for the tag
// Input      : tag - request tag
// Output     : funcname - function name
//              ppFunc - pointer to function
// Return     : 0 - success
//             -1 - failed 
//------------------------------------------------------------------
/*
int BNTCmd::GetFuncFP( string tag, string funcname, int (**ppFunc)() )
{
	int (BNTCmd::*pFunc)( const unsigned char *, const int, unsigned char *, int * );
	
	if ( tag == "FB" )
	{
		funcname = "FB_TransPIN";
		pFunc = &BNTCmd::FB_TransPIN;
	}
	*ppFunc = pFunc;
	
	for ( int i = 0; cmdType[i].tag.length() > 0; i++ )
	{
		if ( cmdType[i].tag == tag )
		{
			funcname = cmdType[i].name;
			*ppFunc = cmdType[i].pFunc;
			return 0;
		}
	}

	return -1;
}
*/
/*
------------------------------------------------------------------
 Function   : Proc
 Description: process command
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::Proc( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::Proc Request message", 0, pcReq, iReqLen );
	
	//get cmd tag from request
	char caTag[5];
	memset( caTag, 0, sizeof(caTag) );
	DataFmtConv::hex_asc( caTag, pcReq + 14, 4 );

	string sTag = caTag;
	if ( sTag.substr( 0, 2 ) != "FF" )//1 byte tag
	{
		memset( caTag + 2, 0, sizeof(caTag) - 1 );//get 1 byte tag
		sTag = caTag;
	}
		

	//get function for the request
	/*
	char caFuncName[128];
	int (*pFunc)() = NULL;
	memset( caFuncName, 0, sizeof(caFuncName) );
	if ( GetFuncFP( caTag, caFuncName, &pFunc ) != 0 )
		return -1;

	sprintf( caMsg, "info:func %s", caFuncName );
	errLog.ErrLog(1000, caMsg, 0, 0, 0 );
	*/

	//implement the function
	/*
	if ( (*pFunc)(pcReq, iReqLen, *pcResp, piRespLen) != 0 )
		return -1;
	*/

	int ilen = iReqLen - 14;
	int olen = 0;
	unsigned char ucReq[1024], ucResp[1024];
	memset( ucReq, 0, sizeof(ucReq) );
	memset( ucResp, 0, sizeof(ucResp) );
	memcpy( ucReq, pcReq + 14, iReqLen - 14 );
	
	if ( sTag == "F2" )
		F2_SelfTest( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "E9" )
		E9_GenKey( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "EA" )
		EA_LoadKey( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FB" )
		FB_TransPIN( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FC" )
		FC_VerPIN( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF29" )
		FF29_GenPVV( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF28" )
		FF28_GenPIN( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FD" )
		FD_VerCVX( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF2A" )
		FF2A_GenCVV( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF22" )
		FF22_OutPKey( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF21" )
		FF21_TransToken( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF64" )
		FF64_PrntPIN( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "FF23" )
		FF23_KeyInput( ucReq, ilen, ucResp, &olen);
	else if ( sTag == "EB" )
		EB_ImpKey( ucReq, ilen, ucResp, &olen);
	else 
	{
		sprintf( caMsg, "not supported cmd %s", caTag );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}

   DataFmtConv::asc_hex( pcResp + 2, 11, MD_SEC_BNT_HEADER_REQ.c_str(), 22 );
   DataFmtConv::asc_hex( pcResp + 2 + 11, 1, MD_SEC_BNT_CDE.c_str(), 2 );
	memcpy( pcResp + 2 + 12, ucResp, olen );
	unsigned long l = 12 + olen;
	DataFmtConv::long_tab( pcResp, 2, &l );
	*piRespLen = 2 + 12 + olen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::Proc Response message", 0, pcResp, *piRespLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : F2_SelfTest
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::F2_SelfTest( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::F2_SelfTest request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 2 + 1 + 2;
	iCmdLen += 2 + 1 + 1;
	iCmdLen += 2 + 1 + 1;
	iCmdLen += 2 + 1 + 8;
	iCmdLen += 2 + 1 + 10;
	iCmdLen += 2 + 1 + 6;

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x12", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	memcpy( pcResp + iLen, "\xDF\x17\x02\x00\x00", 5 );
	iLen += 5;
	memcpy( pcResp + iLen, "\xDF\x1C\x01\x00", 4 );
	iLen += 4;
	memcpy( pcResp + iLen, "\xDF\x16\x01\x01", 4 );
	iLen += 4;
	memcpy( pcResp + iLen, "\xDF\x1D\x08", 3 );
	iLen += 3;
	memcpy( pcResp + iLen, "BULL-SSM", 8 );
	iLen += 8;
	memcpy( pcResp + iLen, "\xDF\x1E\x0A", 3 );
	iLen += 3;
	memcpy( pcResp + iLen, "DN0001.0.0", 10 );
	iLen += 10;
	memcpy( pcResp + iLen, "\xDF\x1F\x06", 3 );
	iLen += 3;
	memcpy( pcResp + iLen, "\x00\x00\x00\x00\x00\x00", 6 );
	iLen += 6;
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::F2_SelfTest response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : E9_GenKey
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::E9_GenKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	int i;
	int j;
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::E9_GenKey request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//generate clear key
	unsigned long lRand = 0;
	unsigned char ucRand[5];
	char caClear[33];
	char caClear_8[17];
		
	srand( time(NULL) );
	memset( caClear, 0, sizeof(caClear) );
	memset( caClear_8, 0, sizeof(caClear_8) );
	
	for (i = 0; i < 4; i++ )
	{
		lRand = rand();
		memset( ucRand, 0, sizeof(ucRand) );
	
		DataFmtConv::long_tab( ucRand, 4, &lRand );	
		
	   /* add by guojy in 2011/02/17 for odd parity */
	   /*** begin ***/

	    errLog.ErrLog( LOG_LEVEL_INFO, "no odd parity rand", 0, ucRand, 4 );
	   
	 	for (j = 0; j<4; j++)
    	{
    		SecBasic::Odd_parity(ucRand[j],ucRand + j);
		}

		errLog.ErrLog( LOG_LEVEL_INFO, "odd parity rand", 0, ucRand, 4 );

	    /*** end ***/
	
		DataFmtConv::hex_asc( caClear + i * 8, ucRand, 8 );
	} 
		

	
#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "generated clear key %s", caClear );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
	
	//get kid
	int pos1 = reqStr.find( "D2" );
	string kid = reqStr.substr( pos1 + 2 + 2, 38 );
	int pos2 = reqStr.find( "D2", pos1 + 2 ); 
	string stokid = reqStr.substr( pos2 + 2 + 2, 38 );

	/*add by guojy in 2011/01/26 for generating single DES key */
	if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
	{
		memcpy(caClear_8, caClear, 16);

#ifdef CLEAR_PIN_TEST_FLAG
		sprintf( caMsg, "generated clear DES key %s", caClear_8 );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	}
	else
	{
#ifdef CLEAR_PIN_TEST_FLAG
		sprintf( caMsg, "generated clear 3DES key %s", caClear );
		errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	}

	//check storage key
	string kev;
	if ( stokid.substr( 0, 2 ) == MD_SEC_BNT_TK_KDKM )
	{
		/*add by guojy in 2011/01/26 for generating single DES key */
		if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
		{
			kev = SecBasic::EncECB( KDKM, caClear_8 );
		}
		else
		{
			kev = SecBasic::EncECB( KDKM, caClear );
		}
	}
	else if ( stokid.substr( 0, 2 ) == MD_SEC_BNT_TK_KDK )
	{
		/*add by guojy in 2011/01/26 for generating single DES key */
		if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
		{
			kev = SecBasic::EncECB( LKey, caClear_8 );
		}
		else
		{
			kev = SecBasic::EncECB( LKey, caClear );
		}
	}
		
	
	//get key encryption key
	KeyToken ekToken;
	string ek, ekclear;
	pos1 = reqStr.find( "E1" );
	if ( pos1 > 0 )
	{
		ekToken.CompTokenFromStr( reqStr.substr( pos1, 158 ) );
		ek = ekToken.GetKEV();
		ekclear = SecBasic::DecECB( LKey, ek );

#ifdef CLEAR_PIN_TEST_FLAG
		sprintf( caMsg, "LKey %s", LKey.c_str() );
		errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

		sprintf( caMsg, "ek %s", ek.c_str() );
		errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

		sprintf( caMsg, "ekclear %s", ekclear.c_str() );
		errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	}

	//get key token
	KeyToken InToken;
	if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
	{
		InToken.CompToken( kid.substr( 0, 2 ), kid.substr( 36, 2 ), stokid.substr( 0, 2 ), stokid.substr( 36, 2 ), kev, SecBasic::Enc( caClear_8, "0000000000000000" ).substr( 0, 6 ) );
	}
	else
	{
		InToken.CompToken( kid.substr( 0, 2 ), kid.substr( 36, 2 ), stokid.substr( 0, 2 ), stokid.substr( 36, 2 ), kev, SecBasic::Enc( caClear, "0000000000000000" ).substr( 0, 6 ) );
	}
	
	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 1 + 1 + 79;//additional information
	if ( ek.length() > 0 )
	{
		iCmdLen += 1 + 1 + 16; //D5
	}

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x09", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;

	/*add by guojy in 2011/01/26 for generating single DES key */
	if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
	{
		DataFmtConv::asc_hex( pcResp + iLen, 71, InToken.GetTokenStr().c_str(), 142 );
		iLen += 71;
	}
	else
	{
		DataFmtConv::asc_hex( pcResp + iLen, 79, InToken.GetTokenStr().c_str(), 158 );
		iLen += 79;
	}

	if ( ek.length() > 0 )
	{
		string kev = SecBasic::EncECB( ekclear, caClear );
		memcpy( pcResp + iLen, "\xD5\x10", 2 );
		iLen += 2;
		DataFmtConv::asc_hex( pcResp + iLen, 16, kev.c_str(), 32 );
		iLen += 16;
	}
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::E9_GenKey response", 0, pcResp, iLen );
	return 0;
}

/*
------------------------------------------------------------------
 Function   : EA_LoadKey
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::EA_LoadKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::EA_LoadKey request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//check key storage key

	//get key token
	KeyToken InToken;
	InToken.CompTokenFromStr( reqStr.substr( reqStr.find( "E1" ), 158 ) );

	//get clear key
	kid.kt = InToken.GetKT();
	kid.idp = InToken.GetIdP();
	kid.idc = InToken.GetIdC();
	kid.kn = InToken.GetKN();
	LKey = SecBasic::DecECB( Km2bntxKey, InToken.GetKEV() );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "key loaded kt %s idP %s idC %s kn %s value %s", 
		kid.kt.c_str(), kid.idp.c_str(), kid.idc.c_str(), kid.kn.c_str(), LKey.c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

	sprintf( caMsg, "clear key %s KCV %s", LKey.c_str(), InToken.GetKCV().c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x0A", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::EA_LoadKey response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FB_TransPIN
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FB_TransPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FB_TransPIN request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;
        string pin;
	string ipk;
	string opk;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get key
	KeyToken InToken, OutToken;
	int idx = reqStr.find( "E1" );
	InToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	idx = reqStr.find( "E1", idx + 158 );
	OutToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get encrypted pin block
	idx = reqStr.find( "DF05", idx + 158 );
	string pinblk = reqStr.substr( idx + 6, 16 );

	//get pan
	idx = reqStr.find( "DF0713", idx + 6 + 16 );
	Track2Fmt track2fmt;
	track2fmt.CompTrack2FromStr( reqStr.substr( idx + 6, 38 ));
	string pan = track2fmt.GetPAN();
        /* add by scl for checking pan 2011.12.1 */
        if(pan.length() < 13)
        {
	    sprintf( caMsg, "pan[%s] is error", pan.c_str() );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
            goto err;
        }
        /* add end 2011.12.1 */

	ipk = SecBasic::DecECB( LKey, InToken.GetKEV() );
	opk = SecBasic::DecECB( LKey, OutToken.GetKEV() );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "inkey %s outkey %s pan %s pinblk %s", ipk.c_str(), opk.c_str(), pan.c_str(), pinblk.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
	pin = SecISO::DecPINBlk00( ipk, pan, pinblk );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "pin %s", pin.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );	
#endif
	
	if ( pin.length() < 4 )
		goto err;

	pinblk = SecISO::EncPINBlk00( opk, pan, pin );

	goto end;
	
err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 2 + 1 + 8;

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x1B", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
	{
		memcpy( pcResp + iLen, "\xDF\x05\x08", 3 );
		iLen += 3;
		DataFmtConv::asc_hex( pcResp + iLen, 8, pinblk.c_str(), 16 );
		iLen += 8;
	}
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FB_TransPIN response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FC_VerPIN
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FC_VerPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FC_VerPIN request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;
	string pk;
	string pvk;
	string pvvidx;
	string pvv;
	string pinblk;
	KeyToken pkToken, pvkToken;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get pan, pvv
	int idx = reqStr.find( "DF07" );
	unsigned char ucTrack2Len[2];
	memset( ucTrack2Len, 0, sizeof(ucTrack2Len) );
	DataFmtConv::asc_hex( ucTrack2Len, 1, reqStr.substr( idx + 4, 2 ).c_str(), 2 );
	unsigned long track2len = DataFmtConv::tab_long( ucTrack2Len, 2 );
	
	Track2Fmt track2fmt;
	track2fmt.CompTrack2FromStr( reqStr.substr( idx + 6, track2len * 2 ));
	string pan = track2fmt.GetPAN();
        /* add by scl for checking pan 2011.12.1 */
        if(pan.length() < 13)
        {
	    sprintf( caMsg, "pan[%s] is error", pan.c_str() );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
            goto err;
        }
        /* add end 2011.12.1 */
	pvvidx = track2fmt.GetPVVIdx();
	pvv = track2fmt.GetPVV();

	//get pk
	idx = reqStr.find( "E1" );
	pkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get encrypted pin block
	idx = reqStr.find( "DF05", idx + 158 );
	pinblk = reqStr.substr( idx + 6, 16 );

	//get pvk
	idx = reqStr.find( "E1", idx + 22 );
	pvkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get clear key
	pk = SecBasic::DecECB( LKey, pkToken.GetKEV());
	pvk = SecBasic::DecECB( LKey, pvkToken.GetKEV());

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear PIN key %s KCV %s", pk.c_str(), pkToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

	sprintf( caMsg, "clear PVK key %s KCV %s", pvk.c_str(), pvkToken.GetKCV().c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif


	if ( pvv != SecVisaMag::GenPVV( pk, pvk, pvvidx, pan, pinblk ))
	{
		memcpy( ucErrCode, "\x30\x03", 2  );
		goto err;
	}

	goto end;
	
err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
		return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x1C", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FC_VerPIN response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF29_GenPVV
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF29_GenPVV( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF29_GenPVV request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;
	string pvv;
	string pvvidx;
	string pinblk;
	string pk;
	string pvk;
	KeyToken pkToken, pvkToken;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get pan, pvv
	int idx = reqStr.find( "DF0713" );
	
	Track2Fmt track2fmt;
	track2fmt.CompTrack2FromStr( reqStr.substr( idx + 6, 38 ));
	string pan = track2fmt.GetPAN();
   /* add by scl for checking pan 2011.12.1 */
   if(pan.length() < 13)
   {
	    sprintf( caMsg, "pan[%s] is error", pan.c_str() );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
       goto err;
   }
   /* add end 2011.12.1 */
	pvvidx = track2fmt.GetPVVIdx();

	//get pk
	idx = reqStr.find( "E1" );
	pkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get encrypted pin block
	idx = reqStr.find( "DF05", idx + 158 );
	pinblk = reqStr.substr( idx + 6, 16 );

	//get pvk
	idx = reqStr.find( "E1", idx + 22 );
	pvkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get clear key
	pk = SecBasic::DecECB( LKey, pkToken.GetKEV());
	pvk = SecBasic::DecECB( LKey, pvkToken.GetKEV());

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "LKey %s", LKey.c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	sprintf( caMsg, "clear PIN key %s KCV %s", pk.c_str(), pkToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

	sprintf( caMsg, "clear PVK key %s KCV %s", pvk.c_str(), pvkToken.GetKCV().c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
	pvv = SecVisaMag::GenPVV( pk, pvk, pvvidx, pan, pinblk );
	if ( pvv.length() != 4 )
		goto err;
	else
		goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 2 + 1 + 19;

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
		return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x39", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
	{
		memcpy( pcResp + iLen, "\xDF\x07\x13", 3 );
		iLen += 3;
		track2fmt.PutPVV( pvv );
		DataFmtConv::asc_hex( pcResp + iLen, 19, track2fmt.GetTrack2Str().c_str(), 38 );
		iLen += 19;
	}
	
	*piRespLen = iLen;
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF29_GenPVV response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF28_GenPIN
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF28_GenPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF28_GenPIN request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get key token
	KeyToken pkToken;
	int idx = reqStr.find( "E1" );
	pkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get div(pan)
	idx = reqStr.find( "DF3D", idx + 158 );
	string div = reqStr.substr( idx + 4 + 2, 12 );

	//get clear key
	string pk = SecBasic::DecECB( LKey, pkToken.GetKEV());

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear PIN key %s KCV %s", pk.c_str(), pkToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//get random PIN
	srand( time(NULL) );
	unsigned long lRand = rand();
	char caRand[5];

	memset( caRand, 0, sizeof(caRand) );
	DataFmtConv::long_asc( caRand, 4, &lRand );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear PIN %s", caRand );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//compose PIN block
	string xor1 = "04";
	xor1 += caRand;
	xor1 += "FFFFFFFFFF";

	string xor2 = "0000";
	xor2 += div;

	string plk = SecBasic::Xor( xor1, xor2 );
	string enc = SecBasic::Enc( pk, plk );

	goto end;
	
err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 2 + 1 + 8;

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
		return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x38", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	memcpy( pcResp + iLen, "\xDF\x05\x08", 3 );
	iLen += 3;
	DataFmtConv::asc_hex( pcResp + iLen, 8, enc.c_str(), 16 );
	iLen += 8;
	*piRespLen = iLen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF28_GenPIN response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FD_VerCVX
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FD_VerCVX( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FD_VerCVX request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;
	string cvk;
	string exp;
	string svc;
	string cvv;
	KeyToken cvkToken;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get pan, exp, svc
	int idx = reqStr.find( "DF0713" );
	
	Track2Fmt track2fmt;
	track2fmt.CompTrack2FromStr( reqStr.substr( idx + 6, 38 ));
	
	string pan = track2fmt.GetPAN();
        /* add by scl for checking pan 2011.12.1 */
        if(pan.length() < 13)
        {
	    sprintf( caMsg, "pan[%s] is error", pan.c_str() );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
            goto err;
        }
        /* add end 2011.12.1 */
	exp = track2fmt.GetExpDate();
	svc = track2fmt.GetSvcCode();
	cvv = track2fmt.GetCVV();

	//get key token
	idx = reqStr.find( "E1" );
	cvkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get clear key
	cvk = SecBasic::DecECB( LKey, cvkToken.GetKEV());

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear CVK %s KCV %s", cvk.c_str(), cvkToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	if ( cvv != SecVisaMag::GenCVV( cvk, pan, exp, svc ))
	{
		memcpy( ucErrCode, "\x30\x09", 2  );
		goto err;
	}

	goto end;
	
err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
		return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x1D", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FD_VerCVX response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF2A_GenCVV
 Description: 
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF2A_GenCVV( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF2A_GenCVV request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;
	string exp;
	string svc;
	string cvv;
	string cvk;
	string pinblk;
	KeyToken cvkToken;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get pan, exp, svc
	int idx = reqStr.find( "DF0713" );
	Track2Fmt track2fmt;
	track2fmt.CompTrack2FromStr( reqStr.substr( idx + 6, 38 ));
	string pan = track2fmt.GetPAN();
        /* add by scl for checking pan 2011.12.1 */
        if(pan.length() < 13)
        {
	    sprintf( caMsg, "pan[%s] is error", pan.c_str() );
	    errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
            goto err;
        }
        /* add end 2011.12.1 */
	exp = track2fmt.GetExpDate();
	svc = track2fmt.GetSvcCode();
	cvv = track2fmt.GetCVV();

	//get key token
	idx = reqStr.find( "E1" );
	cvkToken.CompTokenFromStr( reqStr.substr( idx, 158 ) );

	//get clear key
	cvk = SecBasic::DecECB( LKey, cvkToken.GetKEV());

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear CVK %s KCV %s", cvk.c_str(), cvkToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//get encrypted pin block
	idx = reqStr.find( "DF05", idx + 6 );
	pinblk = reqStr.substr( idx + 6, 16 );

	cvv = SecVisaMag::GenCVV( cvk, pan, exp, svc );
	if ( cvv.length() != 3 )
	{
		goto err;
	}

	goto end;
	
err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 2;//return code
	iCmdLen += 2 + 1 + 19;

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
		return -1;
	}
	
	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x3A", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	memcpy( pcResp + iLen, "\xDF\x07\x13", 3 );
	iLen += 3;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	track2fmt.PutCVV( cvv );
	DataFmtConv::asc_hex( pcResp + iLen, 19, track2fmt.GetTrack2Str().c_str(), 38 );
	iLen += 19;
	*piRespLen = iLen;

	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF2A_GenCVV response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF22_OutPKey
 Description: generate output key string
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF22_OutPKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF22_OutPKey request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get kev
	int pos1 = reqStr.find( "E1" );
	if( pos1 < 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Tag E1 not exist", 0, 0, 0 );
		return -1;
	}


	//get kid
	
	string kid = reqStr.substr( pos1 + 58, 38 );
			
	KeyToken keToken;

	/*add by guojy in 2011/01/26 for generating single DES key */
	if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
	{
		keToken.CompTokenFromStr1( reqStr.substr( pos1, 71*2 ) );
	}
	else
	{
		keToken.CompTokenFromStr( reqStr.substr( pos1, 79*2 ) );
	}
		
	//get keySplitMethod
	int pos2 = reqStr.find( "DF23", pos1 + 2 ); 
	string keySplitMethod = reqStr.substr( pos2 + 4 + 2, 2 );
	
	if(( keToken.GetKEV().length() != 32 )&&(keToken.GetKEV().length() != 16))
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "Kev length is wrong", 0, (unsigned char *)keToken.GetKEV().c_str(), keToken.GetKEV().length() );
		//goto err;
	}
	//get clear Kev
	string clearKev;
	string clearKevChkValue;
	clearKev = SecBasic::Dec( KDKM, keToken.GetKEV().substr( 0, 16 ) );

	if ((kid.substr( 0, 2 ) != MD_SEC_BNT_TK_KTK1) && (kid.substr( 0, 2 ) != MD_SEC_BNT_TK_KT1))
	{
		clearKev.append( SecBasic::Dec( KDKM, keToken.GetKEV().substr( 16, 16 ) ) );
	}
	
	clearKevChkValue = SecBasic::Enc( clearKev, "0000000000000000" ).substr( 0, 6 );

#ifdef CLEAR_PIN_TEST_FLAG
	errLog.ErrLog( LOG_LEVEL_DEBUG, "Clear Key", 0, (unsigned char *)clearKev.c_str(), clearKev.length() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, "Clear Key ChkValue", 0, (unsigned char *)clearKevChkValue.c_str(), clearKevChkValue.length() );
#endif
	
	unsigned long lRand = 0;
	unsigned char ucRand[5];
	char caClear[33];
	char caClear_8[17];
	string outPutKey[3];
	string strC1Value;
	
	srand( time(NULL) );
	
	//get output key string accord with keySplitMethod 
	if( keySplitMethod == "01" ){		//2 XOR components
		memset( caClear, 0, sizeof(caClear) );	
		memset( caClear_8, 0, sizeof(caClear_8) );
		for ( int i = 0; i < 4; i++ )
		{
			lRand = rand();
			memset( ucRand, 0, sizeof(ucRand) );
			DataFmtConv::long_tab( ucRand, 4, &lRand );

			DataFmtConv::hex_asc( caClear + i * 8, ucRand, 8 );
		}
		if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
		{
			memcpy(caClear_8, caClear, 16);
			outPutKey[0] = (string)caClear_8;
		}
		else
		{
			outPutKey[0] = (string)caClear;
		}
				
		outPutKey[1] = SecBasic::Xor( clearKev, outPutKey[0] );
		
		outPutKey[0].append( SecBasic::Enc( outPutKey[0], "0000000000000000" ).substr( 0, 6 ) );
		outPutKey[1].append( SecBasic::Enc( outPutKey[1], "0000000000000000" ).substr( 0, 6 ) );
		
		strC1Value = keySplitMethod + outPutKey[0] + outPutKey[1];
	}
	else if( keySplitMethod == "02" ){	//3 XOR components
		memset( caClear, 0, sizeof(caClear) );
		memset( caClear_8, 0, sizeof(caClear_8) );
		for ( int i = 0; i < 4; i++ )
		{
			lRand = rand();
			memset( ucRand, 0, sizeof(ucRand) );
			DataFmtConv::long_tab( ucRand, 4, &lRand );

			DataFmtConv::hex_asc( caClear + i * 8, ucRand, 8 );
		}

		if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
		{
			memcpy(caClear_8, caClear, 16);
			outPutKey[0] = (string)caClear_8;
		}
		else
		{
			outPutKey[0] = (string)caClear;
		}
					
		srand( time(NULL)+1 );
		memset( caClear, 0, sizeof(caClear) );
		memset( caClear_8, 0, sizeof(caClear_8) );
		for ( int i = 0; i < 4; i++ )
		{
			lRand = rand();
			memset( ucRand, 0, sizeof(ucRand) );
			DataFmtConv::long_tab( ucRand, 4, &lRand );

			DataFmtConv::hex_asc( caClear + i * 8, ucRand, 8 );
		}
		if ((kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KTK1) || (kid.substr( 0, 2 ) == MD_SEC_BNT_TK_KT1))
		{
			memcpy(caClear_8, caClear, 16);
			outPutKey[1] = (string)caClear_8;
		}
		else
		{
			outPutKey[1] = (string)caClear;
		}
				
		outPutKey[2] = SecBasic::Xor( clearKev, outPutKey[0] );
		outPutKey[2] = SecBasic::Xor( outPutKey[2], outPutKey[1] );
		
		outPutKey[0].append( SecBasic::Enc( outPutKey[0], "0000000000000000" ).substr( 0, 6 ) );
		outPutKey[1].append( SecBasic::Enc( outPutKey[1], "0000000000000000" ).substr( 0, 6 ) );
		outPutKey[2].append( SecBasic::Enc( outPutKey[2], "0000000000000000" ).substr( 0, 6 ) );
		
		strC1Value = keySplitMethod + outPutKey[0] + outPutKey[1] + outPutKey[2];
	}
	else if( keySplitMethod == "03" ){		//3-Clear component
		outPutKey[0] = (string)clearKev;
	
		outPutKey[0].append( SecBasic::Enc( outPutKey[0], "0000000000000000" ).substr( 0, 6 ) );

		
		strC1Value = keySplitMethod + outPutKey[0];
	}
	else{
		errLog.ErrLog( LOG_LEVEL_ERROR, "Key Parts Mode", 0, (unsigned char *)keySplitMethod.c_str(), keySplitMethod.length() );
		goto err;
	}
	
	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/*
	 * Response fmt:FF32 C0 C1 D4 
	 * C1= keySplitMethod+"|"
	 *    +outPutKey[0]+outPutKey_CheckValue[0]
	 *    +outPutKey[1]+outPutKey_CheckValue[1]
	 *    +outPutKey[2]+outPutKey_CheckValue[2]
	*/
	/* get command length */
	
	int iCmdLen = 0;
	int iC1Len=0;
	
	//FF32 NN
	//iCmdLen += 2 + 1;			
	
	//C0 02 00 00
	iCmdLen += 1 + 1 + 2;
	
	//C1 NN CC CC ...
	iC1Len = strC1Value.length()/2+strC1Value.length()%2;
	iCmdLen += 1 + 1 + iC1Len;
	
	//D4 03 12 34 56
	iCmdLen += 1 + 1 + 3;
	
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	memcpy( pcResp + iLen, "\xFF\x32", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	memcpy( pcResp + iLen, "\xC0\x02", 2 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	memcpy( pcResp + iLen, "\xC1", 1 );
	iLen += 1;
	
	pcResp[iLen] = iC1Len;
	iLen += 1;
	DataFmtConv::asc_hex( pcResp + iLen, iC1Len, strC1Value.c_str(), iC1Len*2 );
	iLen += iC1Len;
	
	memcpy( pcResp + iLen, "\xD4\x03", 2 );
	iLen += 2;
	DataFmtConv::asc_hex( pcResp + iLen, clearKevChkValue.length()/2, clearKevChkValue.c_str(), clearKevChkValue.length() );
	iLen += clearKevChkValue.length()/2;
	
	*piRespLen = iLen;
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF22_OutPKey response", 0, pcResp, iLen );
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF21_TransToken
 Description: Translate KeyToKen encryption From one KT to another KT
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF21_TransToken( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	int iE1_len = 0;
	char caTemp[10];
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF21_TransToken request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get toKen
	int pos1 = reqStr.find( "E1" );
	iE1_len = DataFmtConv::HexStr2Int( reqStr.substr(pos1 + 2,2).c_str() );
	
	KeyToken keToken;
	keToken.CompTokenFromStr( reqStr.substr( pos1, 79*2 ) );
	
	//get clear Kev for keToken
	string clearKev;
	clearKev = SecBasic::Dec( KDKM, keToken.GetKEV().substr( 0, 16 ) );
	clearKev.append( SecBasic::Dec( KDKM, keToken.GetKEV().substr( 16, 16 ) ) );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear key %s KCV %s", clearKev.c_str(), keToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//get key encryption toKen 
	// gaojinkui 2011.03.06 , for single length key, token length is 69 instead of 77
	//int pos2 = reqStr.find( "E1", pos1+2+2+77*2 ); 
	int pos2 = reqStr.find( "E1", pos1+2+2+iE1_len*2 ); 
	KeyToken keTokenEnc;
	keTokenEnc.CompTokenFromStr( reqStr.substr( pos2, 79*2 ) );
		
	//get clear Kev for toKen_enc
	string clearKev_enc;
	clearKev_enc = SecBasic::Dec( KDKM, keTokenEnc.GetKEV().substr( 0, 16 ) );
	clearKev_enc.append( SecBasic::Dec( KDKM, keTokenEnc.GetKEV().substr( 16, 16 ) ) );
		
	/* TransToken */
	KeyToken TransToken;
	if( keTokenEnc.GetKT() == MD_SEC_BNT_TK_KM2BNTX ){
		//get key token
		TransToken.CompToken( keToken.GetKT(), keToken.GetKN(), 
                          keTokenEnc.GetKT(), keTokenEnc.GetKN(),
                          SecBasic::Enc( Km2bntxKey, clearKev.substr( 0, 16 ) ) + SecBasic::Enc( Km2bntxKey, clearKev.substr( 16, 16 ) ), 
                          SecBasic::Enc( clearKev, "0000000000000000" ).substr( 0, 6 ) );
	}
	else{
		//get key token
		TransToken.CompToken( keToken.GetKT(), keToken.GetKN(), 
                          keTokenEnc.GetKT(), keTokenEnc.GetKN(),
                          SecBasic::Enc( clearKev_enc, clearKev.substr( 0, 16 ) ) + SecBasic::Enc( clearKev_enc, clearKev.substr( 16, 16 ) ), 
                          SecBasic::Enc( clearKev, "0000000000000000" ).substr( 0, 6 ) );
	}
	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/*
	 * Response fmt:FF31 C0 C1:E1 
  */
	/* get command length */
	
	int iCmdLen = 0;
	int iC1Len=0;
	
	//FF31 NN
	//iCmdLen += 2 + 1;			
	
	//C0 02 00 00
	iCmdLen += 1 + 1 + 2;
	
	//C1 4E E1
	iCmdLen += 1 + 1 + 79;
		
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "FF31", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C002", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C14F", 4 );
	iLen += 2;
	DataFmtConv::asc_hex( pcResp + iLen, 79, TransToken.GetTokenStr().c_str(), 158 );
	iLen += 79;
	
	*piRespLen = iLen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF21_TransToken end", 0, pcResp, iLen );
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF64_PrntPIN
 Description: Print pin mailer
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF64_PrntPIN( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF64_PrntPIN request", 0, pcReq, iReqLen );

	//get request string
	char caReq[2048];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get enc toKen
	int pos1 = reqStr.find( "E1" );
	KeyToken keToken;
	keToken.CompTokenFromStr( reqStr.substr( pos1, 79*2 ) );

	//get clear Kev for keToken
	string clearKev;
	clearKev = SecBasic::Dec( LKey, keToken.GetKEV().substr( 0, 16 ) );
	clearKev.append( SecBasic::Dec( LKey, keToken.GetKEV().substr( 16, 16 ) ) );
		
	//get encrypted Pin Block
	int pos2 = reqStr.find( "DF05", pos1+2+2+77*2 ); 
	string encPinBlock = reqStr.substr( pos2 + 4 + 2, 8*2 );
	//get Pin Block format 
	int pos3 = reqStr.find( "DF06", pos2 ); 
	string pinBlockFmt = reqStr.substr( pos3 + 4 + 2, 1*2 );

	//get diversifier of the PIN Block
	pos3 = reqStr.find( "DF3C", pos3 ); 
	string pinBlockXorStr = "0000"+reqStr.substr( pos3 + 4 + 2, 6*2 );
	
	//decrypt Pin Block with toKen kev to get clear PIN
	string clearPINBlock;
	clearPINBlock = SecBasic::Dec( clearKev, encPinBlock.substr( 0, 16 ) );
	clearPINBlock.append( SecBasic::Dec( clearKev, encPinBlock.substr( 16, 16 ) ) );
	
	//get clear PIN
	string clearPIN;
	clearPIN = SecBasic::Xor( pinBlockXorStr, clearPINBlock );
	
	//get print format
	int pos4 = reqStr.find( "DF73", pos3 );
	int printFmtLen,printFmtLenLen;
	BNTFmt::getTLVvalueLen( reqStr.substr( pos4+4, 6 ), &printFmtLenLen, &printFmtLen );
	string printFmt;
	string printFmtLog;
	printFmt = reqStr.substr( pos4+4+printFmtLenLen*2, printFmtLen*2 );
	printFmtLog = reqStr.substr( pos4+4+printFmtLenLen*2, printFmtLen*2 );
	
	/* 
	 * replace Clear Pin in print format 
	 * 5C5E505E = "/^P^"
	 * 5C5E565E = "/^V^"
	*/
	int posForReplace;
	int PinForPrintLen;
	char caFmtInStr[1024];
	string FieldFmtStr;
	string PinForPrint;
	string PinForPrintInWordFmt;
	string ReplaceStr;
	
	PinForPrintLen = atoi( clearPIN.substr( 0, 2).c_str() );
	PinForPrint = clearPIN.substr( 2, PinForPrintLen );

	string unvisablePIN(PinForPrint.length()*2, '*');
	
	FieldFmtStr = "5C5E565E";
	PinForPrintInWordFmt = DataFmtConv::NumToWord( PinForPrint );
	posForReplace = printFmt.find( FieldFmtStr ); 
	if( posForReplace > 0 ){
		memset( caFmtInStr, 0, sizeof(caFmtInStr) );
		DataFmtConv::hex_asc( caFmtInStr, (unsigned char*)PinForPrintInWordFmt.c_str(), PinForPrintInWordFmt.length()*2 );
		PinForPrintInWordFmt = caFmtInStr;
		printFmt.replace( posForReplace, FieldFmtStr.length(), PinForPrintInWordFmt, 0, PinForPrintInWordFmt.length() );
		printFmtLog.replace( posForReplace, FieldFmtStr.length(), unvisablePIN, 0, unvisablePIN.length());
	}
	
	FieldFmtStr = "5C5E505E";
	posForReplace = printFmt.find( FieldFmtStr ); 
	if( posForReplace > 0 ){
		memset( caFmtInStr, 0, sizeof(caFmtInStr) );
		DataFmtConv::hex_asc( caFmtInStr, (unsigned char*)PinForPrint.c_str(), PinForPrint.length()*2 );
		PinForPrint = caFmtInStr;
		printFmt.replace( posForReplace, FieldFmtStr.length(), PinForPrint, 0, PinForPrint.length() );
		printFmtLog.replace( posForReplace, FieldFmtStr.length(), unvisablePIN, 0, unvisablePIN.length());
	}
	
	//get Variable Field 0-15 && replace in print format
	string varField;
	string tmpFieldID;
	char caTmp[2+1];
	char caFieldFmt[4+1];
	char caFieldFmtStr[8+1];
	int pos5=pos4;
	int iFieldLen;
	for( int i=0; i<=15; i++ ){
		memset( caTmp, 0, sizeof(caTmp) );
		sprintf( caTmp, "%02x", i );
		caTmp[1] = toupper(caTmp[1]);
		tmpFieldID="DF81"+(string)caTmp;
		pos5 = reqStr.find( tmpFieldID, pos5 );
		//DF81NN not exist,use default value
		if( pos5 < 0 ){
			varField = "";
		}
		else{
			//format: DF8100NNCC...	
			iFieldLen = DataFmtConv::HexStr2Int( reqStr.substr( pos5 + 6, 2 ) );
			varField=reqStr.substr( pos5 + 6 + 2, iFieldLen*2 );
		}
		
		//FieldFmtStr: \^N^
		memset( caFieldFmt, 0, sizeof(caFieldFmt) );
		caFieldFmt[0] = '\\';
		caFieldFmt[1] = '^';
		sprintf( caFieldFmt+2, "%x", i );
		caFieldFmt[2] = toupper(caFieldFmt[2]);
		caFieldFmt[3] = '^';
		memset( caFieldFmtStr, 0, sizeof(caFieldFmtStr) );
		DataFmtConv::hex_asc( caFieldFmtStr, (unsigned char*)caFieldFmt, 8 );
		FieldFmtStr=caFieldFmtStr;
		//replace corresponding field
		posForReplace = printFmt.find( FieldFmtStr, 0 );
		if( posForReplace > 0 ){
			printFmt.replace( posForReplace, FieldFmtStr.length(), varField, 0, varField.length() );
			printFmtLog.replace( posForReplace, FieldFmtStr.length(), varField, 0, varField.length() );
		}
	}

	unsigned char caPrintFmt[1024];
	int  iPrintFmtLen=0;
	memset( caPrintFmt, 0, sizeof(caPrintFmt) );
	iPrintFmtLen = printFmt.length()/2+printFmt.length()%2;

   /* for log */
#ifndef CLEAR_PIN_TEST_FLAG
	DataFmtConv::asc_hex( caPrintFmt, iPrintFmtLen, printFmtLog.c_str(), printFmtLog.length() );
	errLog.ErrLog( LOG_LEVEL_ERROR, "FF64_PrntPIN write data", 0, caPrintFmt, iPrintFmtLen );
#else
   /* for print */
	DataFmtConv::asc_hex( caPrintFmt, iPrintFmtLen, printFmt.c_str(), printFmt.length() );
	errLog.ErrLog( LOG_LEVEL_ERROR, "FF64_PrntPIN write data", 0, caPrintFmt, iPrintFmtLen );
#endif
	
#ifdef _PRINT_PINMAILER
	fstream	hWriteFile;
	hWriteFile.open( "./pinmailer", ios::out );
	hWriteFile.write( (char *)caPrintFmt, iPrintFmtLen );
	hWriteFile.close();
#endif

	commSerialPort hSerialPort;
	int iRet = hSerialPort.Open();
	if( iRet != 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Open Serial Port error", 0, 0, 0 );
		goto err;
	}
	iRet = hSerialPort.Write( caPrintFmt, iPrintFmtLen );
	if( iRet != 0 ){
		hSerialPort.Close();
		errLog.ErrLog( LOG_LEVEL_ERROR, "Write with Serial Port error", 0, 0, 0 );
		goto err;
	}
	
	hSerialPort.Close();
	
	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/*
	 * Response fmt:FF74 C0 C1:E1 
  */
	/* get command length */
	
	int iCmdLen = 0;
	int iC1Len=0;
	
	
	//C0 02 00 00
	iCmdLen += 1 + 1 + 2;
		
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "FF74", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C002", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	

	*piRespLen = iLen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF64_PrntPIN end", 0, pcResp, iLen );
	return 0;
}

/*
------------------------------------------------------------------
 Function   : FF23_KeyInput
 Description: Input Key from screen input
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::FF23_KeyInput( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF23_KeyInput request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get D2
	int pos1 = reqStr.find( "D2" );
	string kid = reqStr.substr( pos1+2+2, 38 );
	pos1 = reqStr.find( "D2", pos1+2+2+38 ); 
	string stokid = reqStr.substr( pos1 + 2 + 2, 38 );
	
	//get DF23
	int pos2 = reqStr.find( "DF23" );
	string tagValue_DF23 = reqStr.substr( pos2 + 4 + 2, 2 );
	
	//get clear Kev parts for D5
	string clearKEVParts[3];
	int pos3 = pos2;

      for( int i=0; i<3; i++ ){
		pos3 = reqStr.find( "D5", pos3 );
		if( pos3 > 0 ){
			pos3 = pos3 + 2 + 2;
			

			if(reqStr.substr( (pos3 -2), 2 ) == "08")
				{
					clearKEVParts[i] = reqStr.substr( pos3, 16 );
			    	pos3 += 16;
				}
			else
				{
			
			clearKEVParts[i] = reqStr.substr( pos3, 32 );
			pos3 += 32;
				}
		}
#ifdef CLEAR_PIN_TEST_FLAG
		errLog.ErrLog( LOG_LEVEL_DEBUG, "Parts String", 0, (unsigned char *)clearKEVParts[i].c_str(), clearKEVParts[i].length() );
#endif
	}

	string clearKEV;
	if( tagValue_DF23 == "01"){
		/* clear KEV compose with 2 parts */
		clearKEV = SecBasic::Xor( clearKEVParts[0], clearKEVParts[1] );
	}
	else{
		/* clear KEV compose with 3 parts */
		clearKEV = SecBasic::Xor( clearKEVParts[0], clearKEVParts[1] );
		clearKEV = SecBasic::Xor( clearKEV, clearKEVParts[2] );
	}
	
	//check storage key
	string kev;
	if ( stokid.substr( 0, 2 ) == MD_SEC_BNT_TK_KDKM )
		kev = SecBasic::EncECB( KDKM, clearKEV );
	else if ( stokid.substr( 0, 2 ) == MD_SEC_BNT_TK_KDK )
		kev = SecBasic::EncECB( LKey, clearKEV );
			
	KeyToken inputToken;
	string kevChkValue;
	kevChkValue = SecBasic::Enc( clearKEV, "0000000000000000" ).substr( 0, 6 );
	inputToken.CompToken( kid.substr( 0, 2 ), kid.substr( 36, 2 ), stokid.substr( 0, 2 ), stokid.substr( 36, 2 ), kev, kevChkValue );

	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/*
	 * Response fmt:FF33 C0 C1 D4
  */
	/* get command length */
	
	int iCmdLen = 0;
	int iC1Len=0;
	
	//C0 02 00 00
	iCmdLen += 1 + 1 + 2;
	
	//C1 4F E1
	iCmdLen += 1 + 1 + 79;
		
	//D4 03 12 34 56
	iCmdLen += 1 + 1 + 3;
	
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "FF33", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C002", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C14F", 4 );
	iLen += 2;
	DataFmtConv::asc_hex( pcResp + iLen, 79, inputToken.GetTokenStr().c_str(), 158 );
	iLen += 79;
	
	DataFmtConv::asc_hex( pcResp + iLen, 2, "D403", 4 );
	iLen += 2;
	DataFmtConv::asc_hex( pcResp + iLen, 3, kevChkValue.c_str(), 6 );
	iLen += 3;
	
	
	*piRespLen = iLen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::FF23_KeyInput end", 0, pcResp, iLen );
	return 0;
}

/*
------------------------------------------------------------------
 Function   : EB_ImpKey
 Description: Import key from improt files
 Input      : pcReq - request message
              iReqLen - request message length
 Output     : pcResp - response message
              piRespLen - pointer to response message length
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int BNTCmd::EB_ImpKey( const unsigned char *pcReq, const int iReqLen, unsigned char *pcResp, int *piRespLen )
{
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::EB_ImpKey request", 0, pcReq, iReqLen );

	//get request string
	char caReq[1024];
	memset( caReq, 0, sizeof(caReq) );
	DataFmtConv::hex_asc( caReq, pcReq, iReqLen * 2 );
	string reqStr = caReq;

	//error code
	unsigned char ucErrCode[2];
	memset( ucErrCode, 0, sizeof(ucErrCode) );

	//get E1:Token of Encrypted Key  
	int pos1 = reqStr.find( "E1" );
	KeyToken encToken;
	encToken.CompTokenFromStr( reqStr.substr( pos1, 79*2 ) );
	pos1 += 79*2;
	
	//get D2:Key ID of Import Key
	pos1 = reqStr.find( "D2", pos1 );
	string impKeyId;
	impKeyId = reqStr.substr( pos1+2+2, 19*2 );
	pos1 += (1+1+19)*2;
	
	//get D4:Check Value of Import Key
	string impKeyChkValue;
	int tmpPos;
	int iD4Flag;    //0-Single value 1-Double value
	tmpPos = reqStr.find( "D403", pos1 );
	if( tmpPos >0 ){
		pos1 = tmpPos;
		impKeyChkValue = reqStr.substr( pos1+2+2, 3*2 );
		pos1 += 5*2;
		iD4Flag = 0;
	}
	else{
		tmpPos = reqStr.find( "D406", pos1 );
		if( tmpPos > 0 ){
			pos1 = tmpPos;
			impKeyChkValue = reqStr.substr( pos1+2+2, 6*2 );
			pos1 += 8*2;
			iD4Flag = 1;
		}
		else{
			impKeyChkValue = "";
			iD4Flag = 2;
		}
	}
	sprintf( caMsg, "KCV:[%s] D4Flag:[%d] pos:[%d]", impKeyChkValue.c_str(), iD4Flag, pos1 );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	//get D5:Encrypted KEV of Import key
	string impKeyKEV;
	tmpPos = reqStr.find( "D510", pos1 );
	if( tmpPos >0 ){
		pos1 = tmpPos;
		impKeyKEV = reqStr.substr( pos1+2+2, 16*2 );
		pos1 += (1+1+16)*2;
	}
	else{
		tmpPos = reqStr.find( "D508", pos1 );
		if( tmpPos > 0 ){
			pos1 = tmpPos;
			impKeyKEV = reqStr.substr( pos1+2+2, 8*2 );
			pos1 += (1+1+8)*2;
		}
		else{
			impKeyKEV = "";
		}
	}
	
	//get Key ID of store key
	pos1 = reqStr.find( "D2", pos1 );
	string stoKeyId;
	stoKeyId = reqStr.substr( pos1+2+2, 19*2 );
	pos1 += (1+1+19)*2;
	
	//get clear KEV of Encrypted Key  
	string clearEncKev;
	
	if ( encToken.GetDKT().substr( 0, 2 ) == MD_SEC_BNT_TK_KDKM ){
		clearEncKev = SecBasic::Dec( KDKM, encToken.GetKEV().substr( 0, 16 ) );
		clearEncKev.append( SecBasic::Dec( KDKM, encToken.GetKEV().substr( 16, 16 ) ) );
	}
	else{
		clearEncKev = SecBasic::Dec( LKey, encToken.GetKEV().substr( 0, 16 ) );
		clearEncKev.append( SecBasic::Dec( LKey, encToken.GetKEV().substr( 16, 16 ) ) );
	}

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear enc key %s KCV %s", clearEncKev.c_str(), encToken.GetKCV().c_str());
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//get clear KEV of Import Key  
	string clearImpKev;
	if( impKeyKEV.length() == 16 ){
		clearImpKev = SecBasic::Dec( clearEncKev, impKeyKEV.substr( 0, 16 ) );
	}
	else{
		clearImpKev = SecBasic::Dec( clearEncKev, impKeyKEV.substr( 0, 16 ) );
		clearImpKev.append( SecBasic::Dec( clearEncKev, impKeyKEV.substr( 16, 16 ) ) );
	}
	
#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "clear imp key %s, imp KEV %s", clearImpKev.c_str(), impKeyKEV.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//enc KEV of Import key with store key
	string kev;
	if ( stoKeyId.substr( 0, 2 ) == MD_SEC_BNT_TK_KDKM )
		kev = SecBasic::EncECB( KDKM, clearImpKev );
	else if ( stoKeyId.substr( 0, 2 ) == MD_SEC_BNT_TK_KDK )
		kev = SecBasic::EncECB( LKey, clearImpKev );

  if( kev.length() == 16 ){
  	kev += kev;
  }
  
	KeyToken impKeyToken;
	string 	kevChkValue;
	if( iD4Flag == 0 ){
		kevChkValue = SecBasic::Enc( clearImpKev, "0000000000000000" ).substr( 0, 6 );
		if( kevChkValue != impKeyChkValue ){
			sprintf( caMsg, "Impkey check value is wrong, import value:[%s] calc value:[%s]", impKeyChkValue.c_str(), kevChkValue.c_str() );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			goto err;
		}
	}
	else if( iD4Flag == 2 ){ //No KCV, just generate 
		kevChkValue = SecBasic::Enc( clearImpKev, "0000000000000000" ).substr( 0, 6 );
	}
	else {
		string 	kevChkValue1;
		string 	kevChkValue2;
		kevChkValue1 = SecBasic::Enc( clearImpKev.substr(0,16), "0000000000000000" ).substr( 0, 6 );
		kevChkValue2 = SecBasic::Enc( clearImpKev.substr(16,16), "0000000000000000" ).substr( 0, 6 );
		kevChkValue = kevChkValue1+kevChkValue2;
		if( kevChkValue != impKeyChkValue ){
			sprintf( caMsg, "Impkey check values is wrong, import value:[%s] calc value:[%s]", impKeyChkValue.c_str(), kevChkValue.c_str() );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			goto err;
		}
		kevChkValue = SecBasic::Enc( clearImpKev, "0000000000000000" ).substr( 0, 6 );
	}
	
	sprintf( caMsg, "impKeyId %s", impKeyId.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	sprintf( caMsg, "stoKeyId %s", stoKeyId.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	impKeyToken.CompToken( impKeyId.substr( 0, 2 ), impKeyId.substr( 36, 2 ), stoKeyId.substr( 0, 2 ), stoKeyId.substr( 36, 2 ), kev, kevChkValue );

	goto end;

err:	
	if ( memcmp( ucErrCode, "\x00\x00", 2 ) == 0 )
		memcpy( ucErrCode, "\x00\x01", 2  );

end:
	/*
	 * Response fmt:FF0B C0 C1:E1 
  */
	/* get command length */
	
	int iCmdLen = 0;
	int iC1Len=0;
	
	//FF0B NN
	//iCmdLen += 2 + 1;			
	
	//C0 02 00 00
	iCmdLen += 1 + 1 + 2;
	
	//E1
	iCmdLen += 79;
		
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "BNTFmt::getlenbuf", 0, 0, 0 );
	  return -1;
	}

	int iLen = 0;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "FF0B", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, caCmdLenBuf, iCmdLenBufLen );
	iLen += iCmdLenBufLen;
	DataFmtConv::asc_hex( pcResp + iLen, 2, "C002", 4 );
	iLen += 2;
	memcpy( pcResp + iLen, ucErrCode, 2 );
	iLen += 2;
	
	DataFmtConv::asc_hex( pcResp + iLen, 79, impKeyToken.GetTokenStr().c_str(), 158 );
	iLen += 79;
	
	*piRespLen = iLen;
	
	errLog.ErrLog( LOG_LEVEL_INFO, "BNTCmd::EB_ImpKey end", 0, pcResp, iLen );
	return 0;
}

/*
------------------------------------------------------------------
 Function   : E9_GenKey_cmd
 Description: 
 Input      : keyId - key identifier
 Output     : none
 Return     : command string
 
------------------------------------------------------------------
*/
string KMCReq::E9_GenKey_cmd( const string keyId, const string stokeyId )
{
	/* get command length */
	int iCmdLen = 0;
	iCmdLen += 1 + 1 + 19;//identifier of the key to be generated
	iCmdLen += 1 + 1 + 19;//identifier of the key to be generated

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	string cmdStr = MD_SEC_BNT_HEADER_REQ;

	cmdStr += MD_SEC_BNT_CDE;

	cmdStr += "E9";

	cmdStr += caASCLen;

	cmdStr += keyId;

	cmdStr += stokeyId;

	return cmdStr;
}

/*
------------------------------------------------------------------
 Function   : FF09_GenKey_resp
 Description: 
 Input      : resp - response
 Output     : none
 Return     : token - success
              "" - failure
------------------------------------------------------------------
*/
string KMCReq::FF09_GenKey_resp( const string resp )
{
	int pos1 = resp.find( "C0020000" );
	if ( pos1 < 0 )	
		return "";
	else
		/* modify by guojy in 2011/01/26 
		return resp.substr( pos1 + 8, 158 ); */
		return resp.substr( pos1 + 8,158);
}

/*
------------------------------------------------------------------
 Function   : FF33_InputKey_resp
 Description: 
 Input      : resp - response
 Output     : none
 Return     : token - success
              "" - failure
------------------------------------------------------------------
*/
string KMCReq::FF33_InputKey_resp( const string resp )
{
	int pos1 = resp.find( "C0020000C14F" );
	if ( pos1 < 0 )	
		return "";
	else
		return resp.substr( pos1+12, 158 );
}

/*
------------------------------------------------------------------
 Function   : FF22_OutPKey_cmd
 Description: 
 Input      : keyId - key identifier
 Output     : none
 Return     : command string
 
------------------------------------------------------------------
*/
string KMCReq::FF22_OutPKey_cmd( const string keyToken, const string keySplitMethod )
{
	int iCmdLen = 0;
	
	/* 
	 * cmd tag:FF22
	 * cmd component:E1 DF22 DF23
  */
	iCmdLen += 1 + 1 + 77;//E1(keyToken)
	iCmdLen += 2 + 1 + 1;	//DF22(output key support)
	iCmdLen += 2 + 1 + 1;	//DF23(key splitting method)

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	string cmdStr = MD_SEC_BNT_HEADER_REQ;

	cmdStr += MD_SEC_BNT_CDE;

	cmdStr += "FF22";

	cmdStr += caASCLen;

	//E1
	cmdStr += keyToken;
	//DF22
	cmdStr += MD_SEC_BNT_DF22_DEFAULT;
	//DF23
	cmdStr.append("DF23").append("01").append("0");
	cmdStr += keySplitMethod;
	
	return cmdStr;
}

/*
------------------------------------------------------------------
 Function   : FF21_TransToken_cmd
 Description: 
 Input      : keyToken     - Token of the key to be translated
 	          : encKeyToken  - Encryption key token
 Output     : none
 Return     : command string
 
------------------------------------------------------------------
*/
string KMCReq::FF21_TransToken_cmd( const string keyToken, const string encKeyToken )
{
	int iCmdLen = 0;
	
	/* 
	 * cmd tag:FF21
	 * cmd component:E1 E1
  */
	iCmdLen += 1 + 1 + 77;//E1(keyToken)
	iCmdLen += 1 + 1 + 77;//E1(encKeyToken)


	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	string cmdStr = MD_SEC_BNT_HEADER_REQ;

	cmdStr += MD_SEC_BNT_CDE;

	cmdStr += "FF21";

	cmdStr += caASCLen;

	//E1
	cmdStr += keyToken;
	//E1
	cmdStr += encKeyToken;
	
	return cmdStr;
}

//=============================

/*
------------------------------------------------------------------
 Function   : FF23_KeyInput_cmd
 Description: 
 Input      : 
 Output     : none
 Return     : command string
 
------------------------------------------------------------------
*/
string KMCReq::FF23_KeyInput_cmd( const string keyId,
	                                const string stokeyId,
	                                const string keyPartMode,
	                                const string kevParts )
{
	int iCmdLen = 0;
	
	/* 
	 * cmd tag:FF23
	 * cmd component:D2 D3 DF24 DF23 D2 D5 D5 D5
  */
  string tag_D3   = MD_SEC_BNT_D3_DEFAULT;
  string tag_DF24 = MD_SEC_BNT_DF24_DEFAULT;
  string tag_DF23 = "DF23010";
  tag_DF23.append( keyPartMode.substr( 0, 1 ) );
  
	iCmdLen += keyId.length()/2;		//D2:keyId
	iCmdLen += tag_D3.length()/2;		//D3
	iCmdLen += tag_DF24.length()/2;	//DF24
	iCmdLen += tag_DF23.length()/2;	//DF23
	iCmdLen += stokeyId.length()/2;	//D2:stokeyId
	iCmdLen += kevParts.length()/2;	//kevParts=D5+D5+D5

	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	string cmdStr = MD_SEC_BNT_HEADER_REQ;

	cmdStr += MD_SEC_BNT_CDE;

	cmdStr += "FF23";

	cmdStr += caASCLen;

	//D2:keyId
	cmdStr += keyId;
	//D3
	cmdStr += tag_D3;
	//DF24
	cmdStr += tag_DF24;
	//DF23
	cmdStr += tag_DF23;
	//D2:stokeyId
	cmdStr += stokeyId;
	//kevParts
	cmdStr += kevParts;
	
	return cmdStr;
}

//=============================

/*
------------------------------------------------------------------
 Function   : EB_ImpKey_cmd
 Description: 
 Input      : 
 Output     : none
 Return     : command string
 
------------------------------------------------------------------
*/
string KMCReq::EB_ImpKey_cmd( const string encKeyToken,
                              const string impKeyId,
                              const string impKeyKCV,
                              const string impKeyKEV,
                              const string stoKeyId )
{
	int iCmdLen = 0;
	
	/* 
	 * cmd tag:EB
	 * cmd component:E1 D2 D4 D5 D2
  */
	iCmdLen += encKeyToken.length()/2;//E1:Token of Encrypted Key 
	iCmdLen += impKeyId.length()/2;		//D2:Key ID of Import Key
	iCmdLen += impKeyKCV.length()/2;	//D4:Check Value of Import Key
	iCmdLen += impKeyKEV.length()/2;	//D5:Encrypted KEV of Import key
	iCmdLen += stoKeyId.length()/2;		//D2:Key ID of store key


	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iCmdLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	string cmdStr = MD_SEC_BNT_HEADER_REQ;

	cmdStr += MD_SEC_BNT_CDE;

	cmdStr += "EB";

	cmdStr += caASCLen;

	//E1
	cmdStr += encKeyToken;
	//D2
	cmdStr += impKeyId;
	//D4
	cmdStr += impKeyKCV;
	//D5
	cmdStr += impKeyKEV;
	//D2
	cmdStr += stoKeyId;
	
	return cmdStr;
}

//=============================

