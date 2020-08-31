//=============================
// bntfmt.cpp
// 
//=============================
#include "bntfmt.h"
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <fstream>
#include <iostream>
using namespace std;
//-----------------------------

//------------------------------------------------------------------
// Function   : GetKT
// Description: get KT of exchanged key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetKT()
{
	return token.kid.kt;
}

//------------------------------------------------------------------
// Function   : GetKN
// Description: get KN of exchanged key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetKN()
{
	return token.kid.kn;
}

//------------------------------------------------------------------
// Function   : GetIdP
// Description: get IdP of exchanged key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetIdP()
{
	return token.kid.idp;
}

//------------------------------------------------------------------
// Function   : GetIdC
// Description: get IdC of exchanged key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetIdC()
{
	return token.kid.idc;
}

//------------------------------------------------------------------
// Function   : GetDKT
// Description: get KT of decryption key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetDKT()
{
	return token.dkid.kt;
}

//------------------------------------------------------------------
// Function   : GetDKN
// Description: get KN of decryption key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetDKN()
{
	return token.dkid.kn;
}

//------------------------------------------------------------------
// Function   : GetKEV
// Description: get KEV of exchanged key, tag D5
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetKEV()
{
	return token.kev;
}

//------------------------------------------------------------------
// Function   : GetKCV
// Description: get KCV of exchanged key, tag D4
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
string KeyToken::GetKCV()
{
	return token.kcv;
}

//------------------------------------------------------------------
// Function   : GetDKid
// Description: get D2 decryption key,first tag D2
// Input      : none
// Output     : none
// Return     : dkid
//------------------------------------------------------------------
string KeyToken::GetDKid()
{
	
	return ("D213"+token.dkid.kt+token.dkid.idp+token.dkid.idc+token.dkid.kn);
}

//------------------------------------------------------------------
// Function   : GetKid
// Description: get D2 exchanged key,second tag D2
// Input      : none
// Output     : none
// Return     : kid
//------------------------------------------------------------------
string KeyToken::GetKid()
{
	return ("D213"+token.kid.kt+token.kid.idp+token.kid.idc+token.kid.kn);
}

//------------------------------------------------------------------
// Function   : GetTokenStr
// Description: get key token string
// Input      : none
// Output     : none
// Return     : key token string
//------------------------------------------------------------------
string KeyToken::GetTokenStr()
{
	string s;

	if ((token.kid.kt == "62")||(token.kid.kt == "06"))
	{
		s = "E145";
	}
	else
	{
		s = "E14D";
	}
	s += "DF1001";
	s += token.idt; 
	s += "D213";
	s += token.dkid.kt + token.dkid.idp + token.dkid.idc + token.dkid.kn;
	s += "D213";
	s += token.kid.kt + token.kid.idp + token.kid.idc + token.kid.kn;;
	s += "D302";
	s += token.usg;
	s += "DF1101";
	s += token.mthd;
	s += "D403";
	s += token.kcv;
	if ((token.kid.kt == "62")||(token.kid.kt == "06"))
	{
		s += "D508";
	}
	else
	{
		s += "D510";
	}
	s += token.kev;
	return s;
}

//------------------------------------------------------------------
// Function   : CompTokenFromStr
// Description: compose token from token string
// Input      : tokenStr - token string
// Output     : none
// Return     : 0 - success
//             -1 - failure
//------------------------------------------------------------------
int KeyToken::CompTokenFromStr( string tokenStr )
{
	int idx = tokenStr.find( "DF1001" );
	token.idt = tokenStr.substr( idx + 6, 2 );

	idx = tokenStr.find( "D213", idx );
	token.dkid.kt = tokenStr.substr( idx + 4, 2 );
	token.dkid.idp = tokenStr.substr( idx + 4 + 2, 10 );
	token.dkid.idc = tokenStr.substr( idx +4+2 + 10, 24 );
	token.dkid.kn = tokenStr.substr( idx + 4 + 2 + 10 + 24, 2 );

	idx = tokenStr.find( "D213", idx + 4 );
	token.kid.kt = tokenStr.substr( idx + 4, 2 );
	token.kid.idp = tokenStr.substr( idx + 4 + 2, 10 );
	token.kid.idc = tokenStr.substr( idx + 4 + 2 + 10, 24 );
	token.kid.kn = tokenStr.substr( idx + 4 + 2 + 10 + 24, 2 );

	idx = tokenStr.find( "D302", idx );
	token.usg = tokenStr.substr( idx + 4, 4 );

	idx = tokenStr.find( "DF1101", idx );
	token.mthd = tokenStr.substr( idx + 6, 2 );

	idx = tokenStr.find( "D403", idx );
	token.kcv = tokenStr.substr( idx + 4, 6 );

	idx = tokenStr.find( "D510", idx );
	token.kev = tokenStr.substr( idx + 4, 32 );

	return 0;
}

//------------------------------------------------------------------
// Function   : CompTokenFromStr1
// Description: compose token from token string
// Input      : tokenStr - token string
// Output     : none
// Return     : 0 - success
//             -1 - failure
//------------------------------------------------------------------
int KeyToken::CompTokenFromStr1( string tokenStr )
{
	int idx = tokenStr.find( "DF1001" );
	token.idt = tokenStr.substr( idx + 6, 2 );

	idx = tokenStr.find( "D213", idx );
	token.dkid.kt = tokenStr.substr( idx + 4, 2 );
	token.dkid.idp = tokenStr.substr( idx + 4 + 2, 10 );
	token.dkid.idc = tokenStr.substr( idx +4+2 + 10, 24 );
	token.dkid.kn = tokenStr.substr( idx + 4 + 2 + 10 + 24, 2 );

	idx = tokenStr.find( "D213", idx + 4 );
	token.kid.kt = tokenStr.substr( idx + 4, 2 );
	token.kid.idp = tokenStr.substr( idx + 4 + 2, 10 );
	token.kid.idc = tokenStr.substr( idx + 4 + 2 + 10, 24 );
	token.kid.kn = tokenStr.substr( idx + 4 + 2 + 10 + 24, 2 );

	idx = tokenStr.find( "D302", idx );
	token.usg = tokenStr.substr( idx + 4, 4 );

	idx = tokenStr.find( "DF1101", idx );
	token.mthd = tokenStr.substr( idx + 6, 2 );

	idx = tokenStr.find( "D403", idx );
	token.kcv = tokenStr.substr( idx + 4, 6 );

	idx = tokenStr.find( "D508", idx );
	token.kev = tokenStr.substr( idx + 4, 16 );

	return 0;
}


//------------------------------------------------------------------
// Function   : CompToken
// Description: compose token and return token string
// Input      : kt - kt for exchanged key
//              kn - kn for exchanged key
//              dkt - kt for decryption key
//              dkn - kn for decryption key
//              kev - KEV for exchanged key
//              kcv - KCV for exchanged key
// Output     : none
// Return     : key token string
//------------------------------------------------------------------
string KeyToken::CompToken( string kt, string kn, string dkt, string dkn, string kev, string kcv )
{
	sprintf( caMsg, "CompToken kt %s kn %s dkt %s dkn %s kev %s kcv %s", kt.c_str(), kn.c_str(), dkt.c_str(), dkn.c_str(), kev.c_str(), kcv.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	token.idt = "03";
	
	token.dkid.kt = dkt;
	token.dkid.idp = SSM_IDP;
	token.dkid.idc = SSM_IDC;
	token.dkid.kn = dkn;

	token.kid.kt = kt;
	token.kid.idp = SSM_IDP;
	token.kid.idc = SSM_IDC;
	token.kid.kn = kn;

	token.usg = SSM_USG;
	token.mthd = SSM_ENC_MTHD;
	token.kcv = kcv;
	token.kev = kev;

	return GetTokenStr();
}

/*
------------------------------------------------------------------
 Function   : getlenbuf
 Description: 
 Input      : len - length
 Output     : buf - formated length buffer
              buflen - formated length buffer length
 Return     : 0 - success
             -1 - failure
------------------------------------------------------------------
*/
int BNTFmt::getlenbuf(int len,char *buf,int *buflen)
{
	char tmpbuf[3];
	if ( len <= 127 )
	{
		memset(buf,len,1);
		*buflen = 1;
	}
	else
	{
		if ( len <= 255 )
		{
			memset(buf,0x81,1);
			memset(buf+1,len,1);
			*buflen = 2;
		}
		else
		if ( len <= 2048 )
		{
			memset(buf,0x82,1);
			memset(buf+1,len/256,1);
			memset(buf+2,len%256,1);
			*buflen = 3;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}
//=============================

/*
------------------------------------------------------------------
 Function   : getTLVvalueLen
 Description: 
 Input      : strInput -- begin from TLV length
 Example    : TLV string -- DF10 03 123456
              strInput = 03123456...
 Output     : Lenlen -- length of TLV Length
 	            ValueLen -- length of TLV Value
 Return     : 0  succ
 	            -1 fail
------------------------------------------------------------------
*/
int BNTFmt::getTLVvalueLen( string strInput, int *Lenlen, int *ValueLen )
{
	//823f01
	if( strInput.substr( 0, 2 ) == "82" ){
		*Lenlen = 3;
		*ValueLen = DataFmtConv::HexStr2Int( strInput.substr( 2, 4 ) );
	}
	else if( strInput.substr( 0, 2 ) == "81" ){
		*Lenlen = 2;
		*ValueLen = DataFmtConv::HexStr2Int( strInput.substr( 2, 2 ) );
	}
	else{
		*Lenlen = 1;
		*ValueLen = DataFmtConv::HexStr2Int( strInput.substr( 0, 2 ) );
	}
	
	return 0;
}
//=============================

/*
------------------------------------------------------------------
 Function   : GenTagStr_D2
 Description: Generate Tag string for D2
 Input      : NKComponentType "44"-1111 1111, "62"-111111 11,refer to SIR 2165
 	            keyType
 	            subType
 	            keyVersion
 Output     : 
 Return     : D2 with string fmt
------------------------------------------------------------------
*/
string BNTFmt::GenTagStr_D2( string NKComponentType, string keyType, string subType, string keyVersion )
{
	int iTagLen = 0;
	int iSubType = 0;
	int iKeyVer = 0;
	int iKN = 0;
	unsigned char caKN[1+1];
	char caKNStr[2+1];
	string tagStr;
	
	//D2:
	tagStr = "D213";
	tagStr.append( keyType.substr( 0, 2 ) );		 //TK: Key Type
	tagStr.append( SSM_IDP ); //IPD: Default value
	tagStr.append( SSM_IDC ); //IDC: Default value
	
	if( NKComponentType == "62" ){
		iSubType = atoi(subType.substr(0,2).c_str());
		iKeyVer = atoi(keyVersion.substr(0,1).c_str());
		iKN = iSubType*4+iKeyVer;
		memset( caKN, 0, sizeof(caKN) );
		caKN[0] = iKN;
		memset( caKNStr, 0, sizeof(caKNStr) );
		DataFmtConv::hex_asc( caKNStr, (unsigned char *)caKN, 1 * 2 );
		tagStr.append( caKNStr );			
	}
	else{
		tagStr.append( subType.substr( 0, 1 ) );			
		tagStr.append( keyVersion.substr( 0, 1 ) );
	}
	return tagStr;
}
//=============================

/*
------------------------------------------------------------------
 Function   : GenTagStr_FF41
 Description: Generate Tag string for FF41
 Input      : decKeyId     --   Decryption key identifier
 	            exchgKeyId   --   Exchange key identifier
 	            exchgKeyKCV  --   KEY check value of Exchange key
 	            exchgKeyKEV  --   Key Encrypted Value of Exchange Key
 Output     : 
 Return     : FF41 with string fmt
------------------------------------------------------------------
*/
string BNTFmt::GenTagStr_FF41( string decKeyId, string exchgKeyId, string exchgKeyKCV, string exchgKeyKEV )
{
	int iTagLen = 0;
	string tagStr;
	
	//FF41: DF10 D2 D2 D3 DF34 D4 D5 
	iTagLen += 2 + 1 + 1;//DF10
	iTagLen += 1 + 1 + 19;//D2
	iTagLen += 1 + 1 + 19;//D2
	iTagLen += 1 + 1 + 2;//D3
	iTagLen += 2 + 1 + 1;//DF34
	iTagLen += 1 + 1 + 3;//D4
	iTagLen += 1 + 1 + 16;//D5
	
	int iCmdLenBufLen = 0;
	char caCmdLenBuf[3+1];
	memset( caCmdLenBuf, 0, sizeof(caCmdLenBuf) );
	int iRet = BNTFmt::getlenbuf(iTagLen,caCmdLenBuf,&iCmdLenBufLen);
	if ( iRet != 0 )
	{
	  return "";
	}
	char caASCLen[8];
	memset( caASCLen, 0, sizeof(caASCLen));
	DataFmtConv::hex_asc( caASCLen, (unsigned char *)caCmdLenBuf, iCmdLenBufLen * 2 );

	tagStr = "FF41";

	tagStr += caASCLen;

	//DF10
	tagStr += "DF100103";
	//D2
	tagStr += decKeyId;
	//D2
	tagStr += exchgKeyId;
	//D3
	tagStr += "D302FFFF";
	//DF34
	tagStr += "DF340102";
	//D4
	tagStr += "D403";
	tagStr += exchgKeyKCV;
	//D5
	tagStr += "D510";
	tagStr += exchgKeyKEV;
	
	return tagStr;
}
//=============================
