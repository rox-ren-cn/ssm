//=============================
// keyTokenTLV.h
// 
//=============================
#ifndef _KEYTOKENTLV_H
#define _KEYTOKENTLV_H
#include <iostream>
using namespace std;

class keyTokenTLV{
private:
	string	strTagName;
	string  strTagValue;
	unsigned long  iTagValueLen;
	int     iTLVStrLen;
	unsigned char		*pcaTagValue;
	unsigned char   *pcaTLVStr;
//	string	strTagFormat;
	/*
	an-ASCII data
	b	-Binary data
	cn-Compressed numeric data
	n	-Numeric data
	var-Variable
	nh-Hexadecimal numeric data
	*/
	
public:
keyTokenTLV( string tagName, int lenOfTagValueLen, unsigned char *pTagValue, int tagValueLen );
keyTokenTLV( string tagName, int lenOfTagValueLen, string TagValue );
~keyTokenTLV();

int SetTagValue_ASC( int lenOfTagValueLen );
unsigned char * GetTLVStr( void );
int GetTLVStrLen( void );
string GetTagName(void);
string TLVStrToStr(void);
string GenTLVStr( string tagName, int lenOfTagValueLen, string TagValue );
};
#endif //_KEYTOKENTLV_H

