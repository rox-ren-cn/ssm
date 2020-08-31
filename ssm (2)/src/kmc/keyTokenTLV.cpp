//=============================
// keyTokenTLV.cpp
// 
//=============================
#include <iostream>
#include <string>
#include "keyTokenTLV.h"
#include "util.h"

using namespace std;

keyTokenTLV::keyTokenTLV( string tagName, int lenOfTagValueLen, unsigned char *pTagValue, int tagValueLen )
{
	int iTmpLen = 0;
	strTagName = tagName;
	iTagValueLen = tagValueLen;
	pcaTagValue = new unsigned char[tagValueLen+1];
	memcpy( pcaTagValue, pTagValue, tagValueLen );
	
	iTmpLen = strTagName.length()/2 + lenOfTagValueLen + iTagValueLen;
	pcaTLVStr = new unsigned char[iTmpLen+1];
}

keyTokenTLV::keyTokenTLV( string tagName, int lenOfTagValueLen, string TagValue )
{
	int iTmpLen = 0;
	int iTagNameLen = 0;
	
	strTagName = tagName;
	
	/* pack TagValue from string to unsigned char * */
	iTagValueLen = TagValue.length()/2+TagValue.length()%2;
	pcaTagValue = new unsigned char[iTagValueLen+1];
	iTagValueLen = DataFmtConv::Pack( (unsigned char *)TagValue.c_str(), pcaTagValue, TagValue.length() );
	
	/* calc size of pcaTLVStr */
	iTagNameLen = strTagName.length()/2+strTagName.length()%2;
	iTmpLen = iTagNameLen + lenOfTagValueLen + iTagValueLen;
	pcaTLVStr = new unsigned char[iTmpLen+1];
	
	/* add TagName in TLV Fmt */
	iTLVStrLen = 0; 
	iTLVStrLen = DataFmtConv::Pack( (unsigned char *)strTagName.c_str(), pcaTLVStr+iTLVStrLen, strTagName.length() );
	
	/* add TagLen in TLV Fmt */
	DataFmtConv::long_tab( pcaTLVStr+iTLVStrLen, lenOfTagValueLen, &iTagValueLen );
	iTLVStrLen += lenOfTagValueLen;
	
	/* add TagValue in TLV Fmt */
	memcpy( pcaTLVStr+iTLVStrLen, pcaTagValue, iTagValueLen );
	iTLVStrLen += iTagValueLen;

}

keyTokenTLV::~keyTokenTLV()
{
	delete(pcaTagValue);
	delete(pcaTLVStr);
}

int keyTokenTLV::SetTagValue_ASC( int lenOfTagValueLen )
{
	int iTmpLen,iOffSet = 0;
	unsigned char *pcaHexLen = new unsigned char [lenOfTagValueLen+1];
	unsigned char *pcaTagName = new unsigned char[strTagName.length()+1];

	iTLVStrLen = 0;
	
	/* add Tag Name */
	iTmpLen = DataFmtConv::Pack( (unsigned char *)strTagName.c_str(), pcaTagName, strTagName.length() );
	memcpy( pcaTLVStr, pcaTagName, iTmpLen );
	iTLVStrLen += iTmpLen;

	iTmpLen = lenOfTagValueLen;
	DataFmtConv::long_tab( pcaHexLen, lenOfTagValueLen, &iTagValueLen );
	/* convert iTagValueLen to Hex value */
	memcpy( pcaTLVStr+iTLVStrLen, pcaHexLen, iTmpLen );
	iTLVStrLen += iTmpLen;
	
	/* add Tag Value */
	memcpy( pcaTLVStr+iTLVStrLen, pcaTagValue, iTagValueLen );
	iTLVStrLen += iTagValueLen;
	
	delete pcaTagName;
	delete pcaHexLen;
	return 0;
}

unsigned char * keyTokenTLV::GetTLVStr()
{
	return pcaTLVStr;
}

int keyTokenTLV::GetTLVStrLen()
{
	return iTLVStrLen;
}

string keyTokenTLV::GetTagName()
{
	return strTagName;
}

string keyTokenTLV::TLVStrToStr()
{
	unsigned char caUnPackStr[iTLVStrLen*2+1];
	memset( caUnPackStr, 0, sizeof(caUnPackStr) );
	DataFmtConv::Unpack( pcaTLVStr, caUnPackStr, iTLVStrLen );
	
	return string((char *)caUnPackStr);
}

string keyTokenTLV::GenTLVStr( string tagName, int lenOfTagValueLen, string TagValue )
{
	int iTmpLen = 0;
	int iTagNameLen = 0;
	
	strTagName = tagName;
	
	/* pack TagValue from string to unsigned char * */
	iTagValueLen = TagValue.length()/2+TagValue.length()%2;
	pcaTagValue = new unsigned char[iTagValueLen+1];
	iTagValueLen = DataFmtConv::Pack( (unsigned char *)TagValue.c_str(), pcaTagValue, TagValue.length() );
	
	/* calc size of pcaTLVStr */
	iTagNameLen = strTagName.length()/2+strTagName.length()%2;
	iTmpLen = iTagNameLen + lenOfTagValueLen + iTagValueLen;
	pcaTLVStr = new unsigned char[iTmpLen+1];
	
	/* add TagName in TLV Fmt */
	iTLVStrLen = 0; 
	iTLVStrLen = DataFmtConv::Pack( (unsigned char *)strTagName.c_str(), pcaTLVStr+iTLVStrLen, strTagName.length() );
	
	/* add TagLen in TLV Fmt */
	DataFmtConv::long_tab( pcaTLVStr+iTLVStrLen, lenOfTagValueLen, &iTagValueLen );
	iTLVStrLen += lenOfTagValueLen;
	
	/* add TagValue in TLV Fmt */
	memcpy( pcaTLVStr+iTLVStrLen, pcaTagValue, iTagValueLen );
	iTLVStrLen += iTagValueLen;
	
	return TLVStrToStr();
}
