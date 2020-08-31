//=============================
// kmcFileOp.cpp
// 
//=============================
/*
#include "errlog.h"
#include "util.h"
#include "global.h"
*/
#include <iostream>
#include <unistd.h>
#include <fstream>

#include "kmcFileOp.h"
using namespace std;

kmcFileOp::kmcFileOp( string fileName, string opMode )
{
  strFileName = fileName;
  strOpMode = opMode;
}

int kmcFileOp::OpenFile()
{
  return FileHandle.OpenFile( strFileName, strOpMode );
}

void kmcFileOp::Close()
{
	FileHandle.CloseFile();
  return;
}

//------------------------------------------------------------------
// Function   : ReadKmcKeyDataFile
// Description: Read one line from KMC key data file
// Input      : none
// Output     : kmcKeyInfo *pKeyInfo
// Return     : 0  success
//							1  eof
//							-1 fail
//------------------------------------------------------------------
int kmcFileOp::ReadKmcKeyDataFile( kmcKeyInfo *pKeyInfo )
{
	int iRet,iReadLen = 0;
	char caTmpStr[1024+1];
	char caKeyInfoStr[256];
	char *pcaStrBegin,*pcaStrEnd;
	
	memset( caTmpStr, 0, sizeof(caTmpStr) );
	
	iRet = FileHandle.ReadLine( caTmpStr, 1024, &iReadLen );
	if( iRet < 0 ) {
		return -1;
	}
	else if( iRet > 0 && iReadLen <= 12 ) {
		return 2;
	}
	
	pcaStrBegin = NULL;
	pcaStrEnd = NULL;
	strcat( caTmpStr, "|" );
	pcaStrBegin = &caTmpStr[0];
	
	/* 
	 * Line Fmt:KeyType|SubType|KeyVer|ChkValue|Desc|KeyTokey|
	*/
	//KeyType
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetKeyType( (string)caKeyInfoStr );
	
	//SubType
	pcaStrBegin = pcaStrEnd+1;
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetSubType( (string)caKeyInfoStr );
	
	//KeyVer
	pcaStrBegin = pcaStrEnd+1;
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetKeyVersion( (string)caKeyInfoStr );
	
	//ChkValue
	pcaStrBegin = pcaStrEnd+1;
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetKeyCheckValue( (string)caKeyInfoStr );
	
	//Desc
	pcaStrBegin = pcaStrEnd+1;
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetDesc( (string)caKeyInfoStr );
	
	//KeyTokey
	pcaStrBegin = pcaStrEnd+1;
	pcaStrEnd = strstr( pcaStrBegin, "|" );
	if( pcaStrEnd == NULL ){
		//error Key Data Format
		return -1;
	}
	memset( caKeyInfoStr, 0, sizeof(caKeyInfoStr) );
	memcpy( caKeyInfoStr, pcaStrBegin, pcaStrEnd-pcaStrBegin );
	pKeyInfo->SetKeyToken( (string)caKeyInfoStr );
	
	return iRet;
}

#if 0
//------------------------------------------------------------------
// Function   : WriteKmcKeyDataFile
// Description: Write one line into KMC key data file
// Input      : kmcKeyInfo *pKeyInfo
// Output     : none
// Return     : 0  success
//							-1 fail
//------------------------------------------------------------------
int kmcFileOp::WriteKmcKeyDataFile( kmcKeyInfo *pKeyInfo )
{
	int iRet,iBytes;
	unsigned char caTmpStr[1024+1];
	
	memset( caTmpStr, 0, sizeof(caTmpStr) );
  
  /* 
	 * Line Fmt:KeyType|SubType|KeyVer|ChkValue|Desc|KeyTokey|
	*/
  iBytes = sprintf( (char *)caTmpStr, "%s|%s|%s|%s|%s|%s|\n", 
  									 pKeyInfo->GetKeyType().c_str(),
  									 pKeyInfo->GetSubType().c_str(),
  									 pKeyInfo->GetKeyVersion().c_str(),
  									 pKeyInfo->GetKeyCheckValue().c_str(),
  									 pKeyInfo->GetDesc().c_str(),
  									 pKeyInfo->GetKeyToken().c_str() );
  									 
	iRet = FileHandle.WriteBytes( caTmpStr, iBytes );
	if( iRet < 0 ) {
		return -1;
	}

	return iRet;
}
#endif
