//=============================
// sec.cpp
// 
//=============================
#include "sec.h"
#include "des.h"
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <fstream>
#include <iostream>
using namespace std;
//-----------------------------

//------------------------------------------------------------------
// Function   : EncPINBlk00
// Description: encrypt PIN block of ISO format 00
// Input      : key - key
//              pan - PAN 
//              pin - clear PIN
// Output     : none
// Return     : encrypted PIN block string
//------------------------------------------------------------------
string SecISO::EncPINBlk00( const string key, const string pan, const string pin )
{
	string panFmt = "0000" + pan.substr( pan.length() - 1 - 12, 12 );
	char pinLen = '0' + pin.length();
	string pinFmt = "0" + pinLen + pin;
	for ( int i = 0; i < 16 - 2 - pin.length(); i++ )
		pinFmt += "F";

	return SecBasic::Enc( key, SecBasic::Xor( panFmt, pinFmt ) );
}

//------------------------------------------------------------------
// Function   : DecPINBlk00
// Description: decrypt PIN block of ISO format 00
// Input      : key - key
//              pan - PAN 
//              enc - encrypted data
// Output     : pin - clear PIN
// Return     : 0 - success
//              "1" - PIN format error
//              "" - other failure
//------------------------------------------------------------------
string SecISO::DecPINBlk00( const string key, const string pan, const string enc )
{
	string panFmt = "0000" + pan.substr( pan.length() - 1 - 12, 12 );
	string pinFmt = SecBasic::Xor( panFmt, SecBasic::Dec( key, enc ));
	
	if ( pinFmt.substr( 0, 1 ) != "0" )
		return "1";

	if ((pinFmt.find( 'F' ) - 2) != atoi(pinFmt.substr( 1, 1 ).c_str()) )
		return "1";
	
	return pinFmt.substr( 2, atoi(pinFmt.substr( 1, 1 ).c_str()) );
}
//------------------------------------------------------------------
// Function   : Enc
// Description: basic DES/3DES encryption
// Input      : key - key
//              clear - 8-byte/16-digit data to be encrypted
// Output     : none
// Return     : encrypted result string
//------------------------------------------------------------------
string SecBasic::Enc( const string key, const string clear )
{
	unsigned char keyH[16], clearH[8], encH[8];
	memset( keyH, 0, sizeof(keyH) );
	memset( clearH, 0, sizeof(clearH) );
	memset( encH, 0, sizeof(encH) );

	DataFmtConv::asc_hex( keyH, key.length()/2, key.c_str(), key.length() );
	DataFmtConv::asc_hex( clearH, 8, clear.c_str(), 16 );
	
	Des::encrypt( keyH, clearH, encH );

	if ( key.length()/2 == DOUBLE_KEY_LEN )
	{
		Des::decrypt( keyH + SINGLE_KEY_LEN, encH, encH );
		Des::encrypt( keyH, encH, encH );
	}

	char encA[17];
	memset( encA, 0, sizeof(encA) );
	DataFmtConv::hex_asc( encA, encH, 16 );

	string res = encA;

	return res;
}

//------------------------------------------------------------------
// Function   : Dec
// Description: basic DES/3DES decryption
// Input      : key - key
//              enc - 8-byte/16-digit data to be decrypted
// Output     : none
// Return     : decrypted result string
//------------------------------------------------------------------
string SecBasic::Dec( const string key, const string enc )
{
	unsigned char keyH[16], clearH[8], encH[8];
	memset( keyH, 0, sizeof(keyH) );
	memset( clearH, 0, sizeof(clearH) );
	memset( encH, 0, sizeof(encH) );

	DataFmtConv::asc_hex( keyH, key.length()/2, key.c_str(), key.length() );
	DataFmtConv::asc_hex( encH, 8, enc.c_str(), 16 );
	
	Des::decrypt( keyH, encH, encH );

	if ( key.length()/2 == DOUBLE_KEY_LEN )
	{
		Des::encrypt( keyH + SINGLE_KEY_LEN, encH, encH );
		Des::decrypt( keyH, encH, clearH );
	}

	char clearA[17];
	memset( clearA, 0, sizeof(clearA) );
	DataFmtConv::hex_asc( clearA, clearH, 16 );

	string res = clearA;

	return res;
}

//------------------------------------------------------------------
// Function   : Xor
// Description: Xor
// Input      : data1 - data1 for Xor
//              data2 - data2 for Xor
// Output     : none
// Return     : result string
//------------------------------------------------------------------
string SecBasic::Xor( const string data1, const string data2 )
{
	unsigned char data1H[1024], data2H[1024], res[1024];
	memset( data1H, 0, sizeof(data1H) );
	memset( data2H, 0, sizeof(data2H) );
	memset( res, 0, sizeof(res) );

	DataFmtConv::asc_hex( data1H, data1.length()/2, data1.c_str(), data1.length() );
	DataFmtConv::asc_hex( data2H, data2.length()/2, data2.c_str(), data2.length() );
	
	for ( int i = 0; i < data1.length()/2; i++ )
	{
		res[i] = data1H[i] ^ data2H[i]; 
	}

	char resA[1024];
	memset( resA, 0, sizeof(resA) );
	DataFmtConv::hex_asc( resA, res, data1.length() );

	string resStr = resA;

	return resStr;
}

//------------------------------------------------------------------
// Function   : Invert
// Description: Xor with 0xFF byte by byte
// Input      : in - input
//              len - length
// Output     : out - output
// Return     : none
//------------------------------------------------------------------
void SecBasic::Invert( const unsigned char *in, unsigned char *out, const unsigned int len )
{	int i;	
   for ( i = 0; i< len; i++)		
		out[i] = in[i] ^ 0xFF;
}

//------------------------------------------------------------------
// Function   : Odd_parity
// Description: Odd parity for a byte
// Input      : in - input
// Output     : out - output
// Return     : none
//------------------------------------------------------------------
void SecBasic::Odd_parity( const unsigned char in, unsigned char *out )
{
	int i = 0;
	int j = 0;
	unsigned char tmp;

	tmp = in;
	for ( i = 0; i< 7; i++)
	{
	    if ( ( tmp & 0x80 )== 0x80 )
	        j += 1;
	    tmp = tmp << 1;
	}

	if ( j % 2 != 0)
	    *out = in & 0xFE;
	else
	    *out = in | 0x01;
}

//------------------------------------------------------------------
// Function   : GenPVV
// Description: generate PVV of VISA algorithm, ch6 of paystd.pdf
// Input      : key - PIN block encryption key
//              pvk - PVK
//              pvki - PVK index
//              pan - PAN 
//              enc - 8-byte/16-digit data of encrypted PIN block
// Output     : none
// Return     : PVV - success
//              "1" - PIN block format error
//              "" - other failure
//------------------------------------------------------------------
string SecVisaMag::GenPVV( const string key, const string pvk, const int pvki, const string pan, const string enc )
{
	//get clear PIN
	string sPIN = SecISO::DecPINBlk00( key, pan, enc );
	if ( sPIN.length() < 4 )
		return sPIN;

	//build TSP
	//right most 11 digits excluding the mod-10 check digit||PVKI||left most 4 digits of PIN
	char cpvki = pvki + 0x30;
	string TSP = pan.substr( pan.length() - 12, 11 ) + cpvki + sPIN.substr( 0, 4 );
	
	//compute with PVK
	string encStr = SecBasic::Enc( pvk, TSP );
	
	//select PVV digits
	string PVV;
	for ( int i = 0; i < 16; i++ )//first scan
	{
		if ( encStr.c_str()[i] <= '9' )
		{
			PVV += encStr.c_str()[i];
			if ( PVV.length() == 4 )
				return PVV;
		}
	}

	if ( PVV.length() < 4 )
	{
		for ( int i = 0; i < 16; i++ )//second scan
		{
			if ( encStr.c_str()[i] == 'A' )
				PVV += '0';
			else if ( encStr.c_str()[i] == 'B' )
				PVV += '1';
			else if ( encStr.c_str()[i] == 'C' )
				PVV += '2';
			else if ( encStr.c_str()[i] == 'D' )
				PVV += '3';
			else if ( encStr.c_str()[i] == 'E' )
				PVV += '4';
			else if ( encStr.c_str()[i] == 'F' )
				PVV += '5';
			else
				continue;

			if ( PVV.length() == 4 )
				return PVV;
		}
	}
		
	return "";
}

//------------------------------------------------------------------
// Function   : GenCVV
// Description: generate CVV of VISA algorithm, ch2 of paystd.pdf
// Input      : cvk - CVK
//              pan - PAN 
//              exp - expiration date of YYMM
//              svc - service code
// Output     : none
// Return     : CVV - success
//------------------------------------------------------------------
string SecVisaMag::GenCVV( const string cvk, const string pan, const string exp, const string svc )
{
	//extract data and pad
	string sField = pan + exp + svc;
	for ( int i = 0; i < 32 - sField.length(); i++ )
		sField += "0";
	
	//encrytion
	unsigned char clear[8], enc[8];//encryption result
	memset( clear, 0, sizeof(clear) );
	memset( enc, 0, sizeof(enc) );
	string res = SecBasic::Enc( cvk, sField.substr( 0, 16 ) );

	res = SecBasic::Xor( res, sField.substr( 16, 16 ) );
	res = SecBasic::Enc( cvk, res );

	//extract and select CVV digits
	string CVVList1, CVVList2, CVV;
	for ( int i = 0; i < 16; i++ )
	{
		if ( res.c_str()[i] <= '9' )
			CVVList1 += res.c_str()[i];
		else if ( res.c_str()[i] == 'A' )
			CVVList2 += '0';
		else if ( res.c_str()[i] == 'B' )
			CVVList2 += '1';
		else if ( res.c_str()[i] == 'C' )
			CVVList2 += '2';
		else if ( res.c_str()[i] == 'D' )
			CVVList2 += '3';
		else if ( res.c_str()[i] == 'E' )
			CVVList2 += '4';
		else if ( res.c_str()[i] == 'F' )
			CVVList2 += '5';
	}

	CVV = CVVList1 + CVVList2;

	return CVV.substr( 0, 3 );
}

//------------------------------------------------------------------
// Function   : GenPIN
// Description: generate random PIN and encrypt PIN block as ISO00 format
// Input      : key - key to encrypt PIN block
//              pinlen - PIN length
//              pan - PAN
// Output     : none - encrypted PIN block
// Return     : encrypted PIN block string
//------------------------------------------------------------------
string SecVisaMag::GenPIN( const string key, const int pinlen, const string pan )
{
	unsigned long lRand = rand();
	char caPIN[13];

	memset( caPIN, 0, sizeof(caPIN) );
	DataFmtConv::long_asc( caPIN, lRand, (unsigned long *)&pinlen );

	string pin = caPIN;
	return SecISO::EncPINBlk00( key, pan, pin );
}

//=============================

