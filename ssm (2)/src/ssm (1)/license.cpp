//=============================
// license.cpp
// 
//=============================
#include "errlog.h"
#include "conffile.h"
#include "commTCPIP.h"
#include "hsmcmd.h"
#include "bntcmd.h"
#include "sec.h" 
#include "global.h"
#include "ssm.h"
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctime>
using namespace std;

int LICENSE::ValidateLicense_RSA()
{
	int iRet = 0;
	unsigned char   caLicCtlCmd_snd[1024];
	unsigned char   caLicCtlCmd_rcv[1024];
	ConfFile confHandle;
	CommTCPIP commTCPIPCli;
	int    iLicCtlCmdLen_snd;
	int    iLicCtlCmdLen_rcv;	
	
	errLog.ErrLog( LOG_LEVEL_INFO, "LICENSE::ValidateLicense_RSA start", 0, 0, 0 );
	
	char caSSMConfigPath[100];
	
	memset( caSSMConfigPath, 0, sizeof(caSSMConfigPath));
	sprintf( caSSMConfigPath, "%s/etc/ssmconf.xml", getenv("SSMWORKPATH") );
	//get ssm config params
	if ( confHandle.ReadConfFile( caSSMConfigPath, "SSMConf", "conf" ) != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "Read ssmconf.xml", 0, 0, 0 );
		return -1;
	}
	
	//get ssm config params
	iRet = commTCPIPCli.conn( confHandle.GetItem("LicCheckIP").c_str(), confHandle.GetItem("LicCheckIPort").c_str() );
	if( iRet != 0 ){
		sprintf( caMsg, "Connect to %s %s error!", confHandle.GetItem("LocalIP").c_str(), confHandle.GetItem("LocalPort").c_str() );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}

	//generate clear key
	int iOffSet = 0;
	int iOriMsgLen;
	unsigned long lRand = 0;
	unsigned char ucRand[5];
	unsigned char caOriMsg[16+1];
	char caRandomStr[32+1];
	string  clearCheckStr;
	
	srand( time(NULL) );
	memset( caOriMsg, 0, sizeof(caOriMsg) );
	for ( int i = 0; i < 4; i++ )
	{
		lRand = rand();
		memset( ucRand, 0, sizeof(ucRand) );
		DataFmtConv::long_tab( ucRand, 4, &lRand );

		memcpy( caOriMsg+i*4, ucRand, 4 );
		DataFmtConv::hex_asc( caRandomStr + i * 8, ucRand, 8 );
	}
	iOriMsgLen = 16;
	clearCheckStr = caRandomStr;
	
	memset( caLicCtlCmd_snd, 0, sizeof(caLicCtlCmd_snd) );
	memcpy( caLicCtlCmd_snd, "\x00\x00", 2 );
	iOffSet = 2;
	
	memcpy( caLicCtlCmd_snd + iOffSet, caOriMsg, iOriMsgLen );
	iOffSet += iOriMsgLen;
	
	/* reset length of send string */
	memset( caLicCtlCmd_snd+1, iOriMsgLen, 1 );
	
	iLicCtlCmdLen_snd = iOffSet;
	iRet = commTCPIPCli.write( caLicCtlCmd_snd, iLicCtlCmdLen_snd );
	if( iRet != 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Send Msg to Monedia error!", 0, caLicCtlCmd_snd, iLicCtlCmdLen_snd );
		return -1;
	}
	errLog.ErrLog( LOG_LEVEL_DEBUG, "Send Msg to Monedia succ!", 0, caLicCtlCmd_snd, iLicCtlCmdLen_snd );
	
	memset( caLicCtlCmd_rcv, 0, sizeof(caLicCtlCmd_rcv) );
	iRet = commTCPIPCli.read( caLicCtlCmd_rcv, &iLicCtlCmdLen_rcv, 0 );
	if( iRet != 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Receive Msg From Monedia error!", 0, 0, 0 );
		return -1;
	}
	errLog.ErrLog( LOG_LEVEL_DEBUG, "Receive Msg From Monedia!", 0, caLicCtlCmd_rcv, iLicCtlCmdLen_rcv );
	
	int iSigMsgLen;
	iSigMsgLen = caLicCtlCmd_rcv[0]*256+caLicCtlCmd_rcv[1];
	
	unsigned char caSigMsg[iSigMsgLen+1];
	memset( caSigMsg, 0, sizeof(caSigMsg) );
	memcpy( caSigMsg, caLicCtlCmd_rcv+2, iSigMsgLen );
	
	char caPubKeyFileName[255];
	memset( caPubKeyFileName, 0, sizeof(caPubKeyFileName) );
	sprintf( caPubKeyFileName, "%s/etc/%s", getenv("SSMWORKPATH"), confHandle.GetItem("SSMRSAKeyPubFile").c_str() );
	iRet = RSAKEY::RSA_VerWithPubKey(	caPubKeyFileName, caOriMsg, iOriMsgLen, caSigMsg, iSigMsgLen );
	if( iRet != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "RSA_VerWithPubKey error!", 0, 0, 0 );
		return -1;
	}

#if 0
{
	/*
  these operation are for DES implement
	*/
	char caRsp[32+1];
	char caRsp[32+1];
	memset( caRsp, 0, sizeof(caRsp) );
	DataFmtConv::hex_asc( caRsp, caLicCtlCmd_rcv+2, 16*2 );
	string encCheckStr = caRsp;
	string decCheckStr;
	string encSsmLicKey;
	string ClearSsmLicKey;
	
	encSsmLicKey = confHandle.GetItem("SSMLicKey");
	ClearSsmLicKey = SecBasic::Dec( LicCtlKey, encSsmLicKey.substr( 0, 16 ) );
	ClearSsmLicKey.append( SecBasic::Dec( LicCtlKey, encSsmLicKey.substr( 16, 16 ) ) );
		
	decCheckStr = SecBasic::Dec( ClearSsmLicKey, encCheckStr.substr( 0, 16 ) );
	decCheckStr.append( SecBasic::Dec( ClearSsmLicKey, encCheckStr.substr( 16, 16 ) ) );
	
#ifdef CLEAR_PIN_TEST_FLAG
	errLog.ErrLog( 2000, "Clear Check String", 0, (unsigned char *)clearCheckStr.c_str(), clearCheckStr.length() );
	errLog.ErrLog( 2000, "Encrypted Check String", 0, (unsigned char *)encCheckStr.c_str(), encCheckStr.length() );
	errLog.ErrLog( 2000, "Decrypted Check String", 0, (unsigned char *)decCheckStr.c_str(), decCheckStr.length() );
	errLog.ErrLog( 2000, "Clear Ssm LicKey", 0, (unsigned char *)ClearSsmLicKey.c_str(), ClearSsmLicKey.length() );
#endif
	
	if( clearCheckStr != decCheckStr ){
		errLog.ErrLog( 1000, "Invalid SSM license!", 0, 0, 0 );
		return -1;
	}
}
#endif 	
	commTCPIPCli.disconn();
	
	errLog.ErrLog( LOG_LEVEL_INFO, "LICENSE::ValidateLicense_RSA end", 0, 0, 0 );
	return 0;
}//============================

int LICENSE::ValidateLicense_Date()
{
	FILE *fp=NULL;
	
	fp = fopen( "../etc/DateLic.dat", "rb" );
	
	if( fp == NULL ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Open Date License File:[../etc/DateLic.dat] error!", 0, 0, 0 );
		return -1;
	}
	
	char caEncStr[22+1];
	int iLen=0;
	//Read Date License
	memset( caEncStr, 0, sizeof(caEncStr) );
	iLen = fread( caEncStr, sizeof(char), 22, fp );
	fclose(fp);
	if( iLen != 22 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Date License File Format error!", 0, 0, 0 );
		return -1;
	}
	
	//Decrypted 16 bytes HEX string to get begin date and end date
	//Format: YYYYMMDDYYYYMMDD
	//first YYYYMMDD is begin date
	//second YYYYMMDD is end date
	string encLicStr = caEncStr;
	string clearLicStr;
	clearLicStr = SecBasic::Dec( LicCtlKey, encLicStr.substr( 0, 16 ) );
	string readKCVStr = encLicStr.substr( 16, 6 );
	
	string calcKCVStr = SecBasic::Enc( clearLicStr+clearLicStr, "0000000000000000" ).substr( 0, 6 );
	
#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "Read Lic Data : %s", encLicStr.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	sprintf( caMsg, "Clear Lic Date : %s, Check Value: %s", clearLicStr.c_str(), calcKCVStr.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
	if( calcKCVStr != readKCVStr ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Invalid Check Value!", 0, 0, 0 );
		return -1;
	}
		
	//get system date and compare with the begin date and end date
	time_t t = time(0);
	char caToday[8+1];
	memset( caToday, 0, sizeof(caToday) );
	strftime( caToday, sizeof(caToday), "%Y%m%d",localtime(&t) );
	
	string beginDate = clearLicStr.substr( 0, 8 );
	string endDate = clearLicStr.substr( 8, 8 );
	string currDate = caToday;
	
	long ibeginDays,iendDays,icurrDays;
	
	ibeginDays = atoi(beginDate.substr(0,4).c_str())*12*30 + atoi(beginDate.substr(4,2).c_str())*30+atoi(beginDate.substr(6,2).c_str());
	iendDays = atoi(endDate.substr(0,4).c_str())*12*30 + atoi(endDate.substr(4,2).c_str())*30+atoi(endDate.substr(6,2).c_str());
	icurrDays = atoi(currDate.substr(0,4).c_str())*12*30 + atoi(currDate.substr(4,2).c_str())*30+atoi(currDate.substr(6,2).c_str());
	
	if( icurrDays < ibeginDays || icurrDays > iendDays ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "Invalid SSM Date License!", 0, 0, 0 );
		return -1;
	}
	
	return 0;
}
