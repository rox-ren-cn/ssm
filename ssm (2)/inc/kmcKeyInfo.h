//=============================
// kmcKeyInfo.h
// 
//=============================
#ifndef _KMCKEYINFO_H
#define _KMCKEYINFO_H
#include <iostream>
using namespace std;

#define  KMC_KEYTYPE_LABEL		"KeyType"
#define  KMC_SUBTYPE_LABEL		"SubType"
#define  KMC_KEYVERSION_LABEL		"KeyVersion"
#define  KMC_DESC_LABELSTR		"Desc"
#define  KMC_KEYPART_1_LABELSTR		"KEYPART 1"
#define  KMC_KEYPART_2_LABELSTR		"KEYPART 2"
#define  KMC_KEYPART_3_LABELSTR		"KEYPART 3"
#define  KMC_KEYPARTMODE_LABELSTR		"KeyPartMode"
#define  KMC_KEYPARTNUM_LABELSTR		"KeyPartNum"
#define  KMC_KEYCHKVAL_LABELSTR		"KeyCheckValue"

class kmcKeyInfo{
private:
	string keyType;
	string subType;
	string keyVersion;
	string desc;
	string keyParts[3];
	string keyPartMode;				//1:2 XOR components 2:3 XOR components
	string keyPartNum;
	string keyCheckValue;
	string keyToken;
	string NKComponentType;
	unsigned char	caKeyToken[256+1];
	char	 caTmpStr[1024];
int CheckKeyType( string strInput );
int CheckSubType( string strInput );
int CheckKeyVersion( string strInput );
int CheckKeyPartsMode( string strInput );
int CheckKeyPartsNum( string strInput );
int CheckKeyParts( string strInput );
int CheckKeyChkValue( string strInput );

public:
void ResetValue(void);
int  Check(void);

void SetKeyType( string strInput );
void SetSubType( string strInput );
void SetKeyVersion( string strInput );
void SetDesc( string strInput );
void SetKeyParts( string strInput, int iPartIndex );
void SetKeyPartsMode( string strInput );
void SetKeyPartsNum( string strInput );
void SetKeyCheckValue( string strInput );
void SetKeyToken( string strInput );
void SetNKComponentType( string strInput );

string GetKeyType( void );
string GetSubType( void );
string GetKeyVersion( void );
string GetDesc( void );
string GetKeyParts( int iPartIndex );
string GetKeyPartsMode( void );
string GetKeyPartsNum( void );
string GetKeyCheckValue( void );
string GetKeyToken( void );
string GetNKComponentType( void );
	
int SetStrFromScrInput( string strInputLabel, string strInputExample );
string SetStrFromScrInputNoChk( string strInputLabel, string strInputExample );
};
#endif //_KMCKEYINFO_H

