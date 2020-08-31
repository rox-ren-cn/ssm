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
	char pinlen[2];
	memset( pinlen, 0, sizeof(pinlen) );
	unsigned long l = pin.length();
	DataFmtConv::long_asc( pinlen, 1, &l );
	string pinFmt = "0";
	pinFmt += pinlen;
	pinFmt += pin;
	for ( int i = 0; i < 16 - 2 - pin.length(); i++ )
		pinFmt += "F";

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "panFmt %s pinFmt %s", panFmt.c_str(), pinFmt.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
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

	if ((pinFmt.find("F") - 2) != atoi(pinFmt.substr( 1, 1 ).c_str()) )
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
	unsigned char keyH[16], encH[8], encHH[8];
	memset( keyH, 0, sizeof(keyH) );
	memset( encH, 0, sizeof(encH) );
	memset( encHH, 0, sizeof(encHH) );

	DataFmtConv::asc_hex( keyH, key.length()/2, key.c_str(), key.length() );
	DataFmtConv::asc_hex( encH, 8, enc.c_str(), 16 );
	
	Des::decrypt( keyH, encH, encHH );

	if ( key.length()/2 == DOUBLE_KEY_LEN )
	{
		Des::encrypt( keyH + SINGLE_KEY_LEN, encHH, encH );
		Des::decrypt( keyH, encH, encHH );
	}

	char clearA[17];
	memset( clearA, 0, sizeof(clearA) );
	DataFmtConv::hex_asc( clearA, encHH, 16 );

	string res = clearA;

	return res;
}

//------------------------------------------------------------------
// Function   : EncECB
// Description: DES/3DES encryption with ECB mode
//              clear = clear1 || clear2 || ...
//              enc = enc1 || enc2 || ...
//              enc1 = DES/3DES(clear1)
//              enc2 = DES/3DES(clear2)
//              ...
// Input      : key - key
//              clear - clear data
// Output     : none
// Return     : encrypted result string
//------------------------------------------------------------------
string SecBasic::EncECB( const string key, const string clear )
{
	string res = "";
	
	for ( int i = 0; i < clear.length()/16; i++ )
	{
		res += Enc( key, clear.substr( i * 16, 16 ) );
	}
	
	return res;
}

//------------------------------------------------------------------
// Function   : DecECB
// Description: DES/3DES decryption with ECB mode
//              enc = enc1 || enc2 || ...
//              clear = clear1 || clear2 || ...
//              clear1 = DES/3DES(enc1)
//              clear2 = DES/3DES(enc2)
//              ...
// Input      : key - key
//              enc - encrypted data
// Output     : none
// Return     : decrypted result string
//------------------------------------------------------------------
string SecBasic::DecECB( const string key, const string enc )
{
	string res = "";
	
	for ( int i = 0; i < enc.length()/16; i++ )
	{
		res += Dec( key, enc.substr( i * 16, 16 ) );
	}
	
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
string SecVisaMag::GenPVV( const string key, const string pvk, const string pvki, const string pan, const string enc )
{
#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "GenPVV %s %s %s", key.c_str(), pan.c_str(), enc.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//get clear PIN
	string sPIN = SecISO::DecPINBlk00( key, pan, enc );
	if ( sPIN.length() < 4 ){
#ifdef CLEAR_PIN_TEST_FLAG
		errLog.ErrLog( LOG_LEVEL_ERROR, "Clear PIN length error", 0, (unsigned char *)sPIN.c_str(), sPIN.length() );
#else
		errLog.ErrLog( LOG_LEVEL_ERROR, "Clear PIN length error", 0, 0, 0);
#endif
		return sPIN;
	}

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "sPIN %s", sPIN.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif

	//build TSP
	//right most 11 digits excluding the mod-10 check digit||PVKI||left most 4 digits of PIN
	string TSP = pan.substr( pan.length() - 12, 11 ) + pvki + sPIN.substr( 0, 4 );

#ifdef CLEAR_PIN_TEST_FLAG
	sprintf( caMsg, "TSP %s", TSP.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
#endif
	
	//compute with PVK
	string encStr = SecBasic::Enc( pvk, TSP );

	sprintf( caMsg, "encStr %s", encStr.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
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

	sprintf( caMsg, "1st scan %s", PVV.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

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

			sprintf( caMsg, "2nd scan %s", PVV.c_str() );
			errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );

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
// Function   : GetPAN
// Description: get PAN of track2
// Input      : none 
// Output     : none
// Return     : PAN string
//------------------------------------------------------------------
string Track2Fmt::GetPAN()
{
	return pan;
}

//------------------------------------------------------------------
// Function   : GetExpDate
// Description: get expiration date of track2
// Input      : none 
// Output     : none
// Return     : expiration date string
//------------------------------------------------------------------
string Track2Fmt::GetExpDate()
{
	return expdate;
}

//------------------------------------------------------------------
// Function   : GetSvcCode
// Description: get service code of track2
// Input      : none 
// Output     : none
// Return     : service code string
//------------------------------------------------------------------
string Track2Fmt::GetSvcCode()
{
	return svccode;
}

//------------------------------------------------------------------
// Function   : GetPVVIdx
// Description: get PVV index of track2
// Input      : none 
// Output     : none
// Return     : PVV index string
//------------------------------------------------------------------
string Track2Fmt::GetPVVIdx()
{
	return pvvidx;
}

//------------------------------------------------------------------
// Function   : GetPVV
// Description: get PVV of track2
// Input      : none 
// Output     : none
// Return     : PVV string
//------------------------------------------------------------------
string Track2Fmt::GetPVV()
{
	return pvv;
}

//------------------------------------------------------------------
// Function   : GetCVV
// Description: get CVV of track2
// Input      : none 
// Output     : none
// Return     : CVV string
//------------------------------------------------------------------
string Track2Fmt::GetCVV()
{
	return cvv;
}

//------------------------------------------------------------------
// Function   : PutPAN
// Description: put PAN of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutPAN( string inpan )
{
	pan = inpan;
}

//------------------------------------------------------------------
// Function   : PutExpDate
// Description: put expiration date of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutExpDate( string inexpdate )
{
	expdate = inexpdate;
}

//------------------------------------------------------------------
// Function   : PutSvcCode
// Description: put service code of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutSvcCode( string insvccode )
{
	svccode = insvccode;
}

//------------------------------------------------------------------
// Function   : PutPVVIdx
// Description: put PVV index of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutPVVIdx( string inpvvidx )
{
	pvvidx = inpvvidx;
}

//------------------------------------------------------------------
// Function   : PutPVV
// Description: put PVV of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutPVV( string inpvv )
{
	pvv = inpvv;
}

//------------------------------------------------------------------
// Function   : PutCVV
// Description: put CVV of track2
// Input      : none 
// Output     : none
// Return     : none
//------------------------------------------------------------------
void Track2Fmt::PutCVV( string incvv )
{
	cvv = incvv;
}

//------------------------------------------------------------------
// Function   : GetTrack2Str
// Description: Compose track2 according to VISA format
// Input      : none
// Output     : none
// Return     : Track2 string
//------------------------------------------------------------------
string Track2Fmt::GetTrack2Str()
{
	//start digit:0, end digit:5 dont check it
	string trk2 = "0" + pan + "D" + expdate + svccode + pvvidx + pvv + cvv + "05";
	return trk2;
}

//------------------------------------------------------------------
// Function   : CompTrack2
// Description: Compose track2 according to VISA format
// Input      : inpan - 
//              inexpdate - expiration date MMYY
//              insvccode - service code
//              inpvvlist - PVK index + PVV
//              incvv 
// Output     : none
// Return     : Track2 string
//------------------------------------------------------------------
string Track2Fmt::CompTrack2( string inpan, string inexpdate, string insvccode, string inpvvlist, string incvv )
{
	pan = inpan;
	expdate = inexpdate;
	svccode = insvccode;
	pvvidx = inpvvlist.substr( 0, 1 );
	pvv = inpvvlist.substr( 1, 4 );
	cvv = incvv;
	return GetTrack2Str();
}

//------------------------------------------------------------------
// Function   : CompTrack2FromStr
// Description: Compose track2 from string
//              1-digit start sentinel || 12 to 19 digits of PAN||seperator('=')||4-digit card expiration date||3-digit service code||1-digit PVV index + 4-digit PVV||3-digit CVV||1-digit end sentinel||1-digit LRC

// Input      : track2 - track2 string 
// Output     : none
// Return     : Track2 string
//------------------------------------------------------------------
void Track2Fmt::CompTrack2FromStr( string track2 )
{
	int idx = track2.find("=");

	if ( idx < 0 )
		idx = track2.find("D");
	
	pan = track2.substr( 0, idx );
	expdate = track2.substr( idx + 1, 4 );
	svccode = track2.substr( idx + 1 + 4, 3 );
	pvvidx = track2.substr( idx + 1 + 4 + 3, 1 );
	pvv = track2.substr( idx + 1 + 4 + 3 + 1, 4 );
	cvv = track2.substr( idx + 1 + 4 + 3 + 1 + 4, 3 );
}//=============================

