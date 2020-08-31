//=============================
// kmcUI.h
// 
//=============================
#ifndef _KMCUI_H
#define _KMCUI_H
#include <iostream>
#include "kmcKeyInfo.h"
#include "kmcFileOp.h"
#include "commTCPIP.h"
#include "conffile.h"
#include "keyTokenTLV.h"
#include "bntcmd.h"
#include "bntfmt.h"

using namespace std;

#define KMC_SEND_TIMEOUT 5   //kmc send time out
#define KMC_SENDBUFLEN_LEN  2
#define KMC_KEYINFO_MAXITEM  128

#define KMC_CMD_TYPE_GENKEY       0		//generate Key
#define KMC_CMD_TYPE_INPUTKEY     1   //Input KMC Key
#define KMC_CMD_TYPE_IMPKEY       2   //import key
#define KMC_CMD_TYPE_OUTPUTKEYSTR 4   //output key string
#define KMC_CMD_TYPE_GENKEYFILE_KDK   5   //generate KDK key file
#define KMC_CMD_TYPE_GENKEYFILE_KSK   6   //generate KSK key file

class kmcUI{
private:
	string strFileType;		//0-KDK file 1-KSK file
	string strMenuID;
	string strInputFileName;
	string strOutputFileType;
	string strOutputFilePath;
	string strKmcKeyFileName;
	string strErrMsg;
	string strNKComponentType;		//"44"-1111 1111, "62"-111111 11,refer to SIR 2165
	string strSubTypeRange;
	string strKeyVerRange;
	string strSubType_DEFAULT;
	unsigned char   caBNTngCmd_snd[1024];
	unsigned char   caBNTngCmd_rcv[1024];
	unsigned long    iBNTngCmdLen_snd;
	int    iBNTngCmdLen_rcv;	
	kmcKeyInfo KeyInfo;
	CommTCPIP Comm_Cli;
	ConfFile kmcconf;
	KeyToken keyToken;

public:
kmcUI();
~kmcUI();
void ReConnect(void);
void DisplayChoiceMenu(void);	//display choice Menu
void ProcessForMenuChoice(void); //process Menu choice 

void DisplayMenu_GenKmcKey(void);  //display Menu for Generate KmcKey
void DisplayMenu_InputKmcKey(void); //display Menu for Input KmcKey
void DisplayMenu_ImportKey(void); //display Menu for Import Key
void DisplayMenu_QueryKey( int iTolCnt, kmcKeyInfo *pKeyInfo ); //display Menu for Query Key

void GenKmcKey(void);         //process Menu choice for GenKmcKey 
void InputKmcKey(void);         //process Menu choice for InputKmcKey 
void ImportKeyFile(void);        //process Menu choice for ImportKeyFile
void QueryKey(void);         //process Menu choice for QueryKey 

int  GenCmd( int iCmdType );  //generate BNTng Command
int  SendCmdToSSM();  //send command to SSM
void Test(void);
	
int QueryKeyFromFile( kmcKeyInfo *pKeyInfo ); //query KMC Key info from file
int DelKeyFromFile( string lineNo ); //delete KMC Key info from file
int WriteKeyIntoFile( kmcKeyInfo inpKeyInfo ); //write Kmc Key info into file
int DisplayKeyPartsStr(void); //display key parts string
int CheckExistOfKeyInfo( kmcKeyInfo *pKeyInfo );  //check the existence of KeyInfo
int CheckExistOfKeyInfo( string strTag_D2 ); //check the existence of KeyInfo
int FindKeyFromFile( string keyType, string subType, string keyVersion, kmcKeyInfo *pKeyInfo ); //find KeyInfo From kmc key file
int GenKeyFile_KDK( string fileName );   //generate KDK key file
int GenKeyFile_KSK( string fileName );   //generate KSK key file
int ImpDESKeyFileData(void);   //import DES Key data from DES Key file
};
#endif //_KMCUI_H

