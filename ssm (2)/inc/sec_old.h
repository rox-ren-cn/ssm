//=============================
// sec.h
// 
//=============================
#ifndef _SEC_H
#define _SEC_H

#include <string>
using namespace std;

const int DOUBLE_KEY_LEN = 16;
const int SINGLE_KEY_LEN = 8;

class Track2Fmt{
private:
	string pan;
	string expdate;
	string svccode;
	string pvvidx;
	string pvv;
	string cvv;
public:
	string GetPAN();
	string GetExpDate();
	string GetSvcCode();
	string GetPVVIdx();
	string GetPVV();
	string GetCVV();
	void PutPAN();
	void PutExpDate();
	void PutSvcCode();
	void PutPVVIdx();
	void PutPVV();
	void PutCVV();
	void CompTrack2( string pan, string expdate, string svccode, string pvvlist, string cvv );
	void CompTrack2FromStr( string track2 );
	string GetTrack2Str();
};//------------------------

//basic DES/3DES encryption and decryption functions and Xor function
class SecBasic{
public:
	static string Enc( const string key, const string clear );//encrypt data	
	static string Dec( const string key, const string enc );//decrypt data
	static string EncECB( const string key, const string enc );//decrypt data
	static string DecECB( const string key, const string enc );//decrypt data
	static string Xor( const string data1, const string data2 );//Xor data1 and data2
	static void Invert( const unsigned char *in, unsigned char *out, const unsigned int len );//Xor with 0xFF byte by byte
	static void Odd_parity( const unsigned char in, unsigned char *out );//Odd parity for a byte
};//------------------------

//ISO format encryption functions
class SecISO{
public:
	static string EncPINBlk00( const string key, const string pan, const string pin );	
	static string DecPINBlk00( const string key, const string pan, const string enc );	
};//------------------------

class SecVisaMag{
public:
	static string GenPVV( const string key, const string pvk, const string pvki, const string pan, const string enc );
	static string GenCVV( const string cvk, const string pan, const string exp, const string svc );
	static string GenPIN( const string key, const int pinlen, const string pan );
};//------------------------

//===========================
#endif //_SEC_H
