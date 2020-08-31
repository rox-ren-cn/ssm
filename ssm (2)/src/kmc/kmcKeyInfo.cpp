//=============================
// kmcKeyInfo.cpp
// 
//=============================
#include <iostream>
#include "kmcKeyInfo.h"
#include "util.h"
#include "errlog.h"
#include "global.h"

using namespace std;

void kmcKeyInfo::SetNKComponentType( string strInput )
{
	NKComponentType = strInput;
}

void kmcKeyInfo::SetKeyType( string strInput )
{
	keyType = DataFmtConv::CvtLetterCase( strInput, 1 );
}

void kmcKeyInfo::SetSubType( string strInput )
{
	subType = DataFmtConv::CvtLetterCase( strInput, 1 );
}

void kmcKeyInfo::SetKeyVersion( string strInput )
{
	keyVersion = DataFmtConv::CvtLetterCase( strInput, 1 );
}

void kmcKeyInfo::SetDesc( string strInput )
{
	desc = strInput;
}

void kmcKeyInfo::SetKeyParts( string strInput, int iPartIndex )
{
	if( iPartIndex <= 2 )
		keyParts[iPartIndex] = DataFmtConv::CvtLetterCase( strInput, 1 );
}

void kmcKeyInfo::SetKeyPartsMode( string strInput )
{
	keyPartMode = strInput;
}

void kmcKeyInfo::SetKeyPartsNum( string strInput )
{
	keyPartNum = strInput;
}

void kmcKeyInfo::SetKeyCheckValue( string strInput )
{
	keyCheckValue = DataFmtConv::CvtLetterCase( strInput, 1 );
}

void kmcKeyInfo::SetKeyToken( string strInput )
{
	keyToken = strInput;
}

string kmcKeyInfo::GetNKComponentType()
{
	return NKComponentType;
}

string kmcKeyInfo::GetKeyType()
{
	return keyType;
}

string kmcKeyInfo::GetSubType()
{
	return subType;
}

string kmcKeyInfo::GetKeyVersion()
{
	return keyVersion;
}

string kmcKeyInfo::GetDesc()
{
	return desc;
}

string kmcKeyInfo::GetKeyParts( int iPartIndex )
{
	if( iPartIndex <= 3 && iPartIndex > 0 )
		return keyParts[iPartIndex-1];
	else		
		return "";
}

string kmcKeyInfo::GetKeyPartsMode()
{
	return keyPartMode;
}

string kmcKeyInfo::GetKeyPartsNum()
{
	return keyPartNum;
}

string kmcKeyInfo::GetKeyCheckValue()
{
	return keyCheckValue;
}

string kmcKeyInfo::GetKeyToken()
{
	return keyToken;
}

int kmcKeyInfo::SetStrFromScrInput( string strInputLabel, string strInputExample )
{
	int iRet = 0;
	string strInput = "";
	
	memset( caTmpStr, 0, sizeof(caTmpStr) );
	cout<<"Please Input "<< strInputLabel <<":"<<strInputExample<<endl;
	cin.getline( caTmpStr, 1024, '\n' );
	strInput = caTmpStr;
	
	if( strInputLabel.compare( KMC_KEYTYPE_LABEL ) == 0 ){
		iRet = CheckKeyType(strInput);
		if( iRet == 0 ) {
			SetKeyType( strInput );
		}
	}
	else if( strInputLabel.compare( KMC_SUBTYPE_LABEL ) == 0 ){
		iRet = CheckSubType(strInput);
		if( iRet == 0 ) {
			SetSubType( strInput );
		}
	}
	else if( strInputLabel.compare( KMC_KEYVERSION_LABEL ) == 0 ){
		iRet = CheckKeyVersion(strInput);
		if( iRet == 0 ) {
			SetKeyVersion( strInput );
		}
	}
	else if( strInputLabel.compare( KMC_DESC_LABELSTR ) == 0 ){
		SetDesc( strInput );
	}
	else if( strInputLabel.compare( KMC_KEYPART_1_LABELSTR ) == 0 ){
		iRet = CheckKeyParts(strInput);
		if( iRet == 0 ) {
			SetKeyParts( strInput, 0 );
		}
		else
			iRet = -1;
	}
	else if( strInputLabel.compare( KMC_KEYPART_2_LABELSTR ) == 0 ){
		iRet = CheckKeyParts(strInput);
		if( iRet == 0 ) {
			SetKeyParts( strInput, 1 );
		}
		else
			iRet = -2;
	}
	else if( strInputLabel.compare( KMC_KEYPART_3_LABELSTR ) == 0 ){
		iRet = CheckKeyParts(strInput);
		if( iRet == 0 ) {
			SetKeyParts( strInput, 2 );
		}
		else
			iRet = -4;
	}
	else if( strInputLabel.compare( KMC_KEYPARTMODE_LABELSTR ) == 0 ){
		iRet = CheckKeyPartsMode(strInput);
		if( iRet == 0 ) {
			SetKeyPartsMode( strInput );
		}
	}
	else if( strInputLabel.compare( KMC_KEYPARTNUM_LABELSTR ) == 0 ){
		iRet = CheckKeyPartsNum(strInput);
		if( iRet == 0 ) {
			SetKeyPartsNum( strInput );
		}
	}
	else if( strInputLabel.compare( KMC_KEYCHKVAL_LABELSTR ) == 0 ){
		iRet = CheckKeyChkValue(strInput);
		if( iRet == 0 ) {
			SetKeyCheckValue( strInput );
		}
	}

	return iRet;
}

string kmcKeyInfo::SetStrFromScrInputNoChk( string strInputLabel, string strInputExample )
{
	int iRet = 0;
	string strInput = "";
	
	memset( caTmpStr, 0, sizeof(caTmpStr) );
	cout<<"Please Input "<< strInputLabel <<":"<<strInputExample<<endl;
	cin.getline( caTmpStr, 1024, '\n' );
	strInput = caTmpStr;
	
	if( strInputLabel.compare( KMC_KEYTYPE_LABEL ) == 0 ){
		SetKeyType( strInput );
	}
	else if( strInputLabel.compare( KMC_SUBTYPE_LABEL ) == 0 ){
		SetSubType( strInput );
	}
	else if( strInputLabel.compare( KMC_KEYVERSION_LABEL ) == 0 ){
		SetKeyVersion( strInput );
	}
	else if( strInputLabel.compare( KMC_DESC_LABELSTR ) == 0 ){
		SetDesc( strInput );
	}
	else if( strInputLabel.compare( KMC_KEYPART_1_LABELSTR ) == 0 ){
		SetKeyParts( strInput, 0 );
	}
	else if( strInputLabel.compare( KMC_KEYPART_2_LABELSTR ) == 0 ){
		SetKeyParts( strInput, 1 );
	}
	else if( strInputLabel.compare( KMC_KEYPART_3_LABELSTR ) == 0 ){
		SetKeyParts( strInput, 2 );
	}
	else if( strInputLabel.compare( KMC_KEYPARTMODE_LABELSTR ) == 0 ){
		SetKeyPartsMode( strInput );
	}
	else if( strInputLabel.compare( KMC_KEYPARTNUM_LABELSTR ) == 0 ){
		SetKeyPartsNum( strInput );
	}
	else if( strInputLabel.compare( KMC_KEYCHKVAL_LABELSTR ) == 0 ){
		SetKeyCheckValue( strInput );
	}

	return strInput;
}

void kmcKeyInfo::ResetValue()
{
	keyType="";
	subType="";
	keyVersion="";
	desc="";
	keyParts[0]="";
	keyParts[1]="";
	keyParts[2]="";
	keyPartMode="";
	keyPartNum="";
	keyCheckValue="";
	keyToken="";
	memset( caKeyToken, 0, sizeof(caKeyToken) );
	memset( caTmpStr, 0, sizeof(caTmpStr) );

	return;
}

int kmcKeyInfo::Check()
{
	int iRet=0;
	
	memset( caMsg, 0, sizeof(caMsg) );
	sprintf( caMsg, "NKComponentType:%s KeyType:%s SubType:%s KeyVer:%s", 
				NKComponentType.c_str(), keyType.c_str(), subType.c_str(), keyVersion.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	if( NKComponentType == "62" ){
		if( keyType.length() != 2 ||
				subType.length() != 2 ||
				keyVersion.length() != 1 ){
			iRet = -1;
		}
	}
	else{
		if( keyType.length() != 2 ||
				subType.length() != 1 ||
				keyVersion.length() != 1 ){
			iRet = -1;
		}
	}

	return iRet;
}

int kmcKeyInfo::CheckKeyType( string strInput )
{
	int iRet = 0;
	
	if( strInput.length() != 2 ){
		return -1;
	}
	
	iRet = DataFmtCheck::CheckHexStr( strInput.c_str(), strInput.length() );
	if( iRet != 0 ){
		return -1;
	}

	return 0;
}

int kmcKeyInfo::CheckSubType( string strInput )
{
	int iRet = 0;

	sprintf( caMsg, "NKComponentType:%s SubType:%s", NKComponentType.c_str(), strInput.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	if( NKComponentType == "62" ){
		if( strInput.length() != 2 ){
		return -1;
		}
		
		if( atoi(strInput.c_str()) >= 64 ){
			return -1;
		}
		
		iRet = DataFmtCheck::CheckIntStr( strInput.c_str(), strInput.length() );
		if( iRet != 0 ){
			return -1;
		}
	}
	else{
		if( strInput.length() != 1 ){
			return -1;
		}
		
		iRet = DataFmtCheck::CheckHexStr( strInput.c_str(), strInput.length() );
		if( iRet != 0 ){
			return -1;
		}
	}
	
	return 0;
}

int kmcKeyInfo::CheckKeyVersion( string strInput )
{
	int iRet=0;
	
	sprintf( caMsg, "NKComponentType:%s KeyVer:%s", NKComponentType.c_str(), strInput.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	if( NKComponentType == "62" ){
		if( strInput != "0" &&
			  strInput != "1" && 
			  strInput != "2" && 
			  strInput != "3" ){
			return -1;
		}
	}
	else{
		if( strInput.length() != 1 ){
			return -1;
		}
		
		iRet = DataFmtCheck::CheckHexStr( strInput.c_str(), strInput.length() );
		if( iRet != 0 ){
			return -1;
		}
	}
	
	return 0;
}

int kmcKeyInfo::CheckKeyPartsMode( string strInput )
{
	int iValue=atoi(strInput.c_str());
	if( iValue > 3 || iValue < 0 )
		return -1;
	return 0;
}

int kmcKeyInfo::CheckKeyPartsNum( string strInput )
{
	int iValue=atoi(strInput.c_str());
	if( iValue > 3 || iValue <= 0 )
		return -1;
	return 0;
}

int kmcKeyInfo::CheckKeyParts( string strInput )
{
/* modified by guojy in 2011/01/20 */
	if(( strInput.length() != 32)&&( strInput.length() != 16)) {
		return -1;
	}
	return 0;
}

int kmcKeyInfo::CheckKeyChkValue( string strInput )
{
	if( strInput.length() != 6 ){
		return -1;
	}
	return 0;
}

