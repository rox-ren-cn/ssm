//=============================
// bntfmt.h
// 
//=============================
#ifndef _BNTFMT_H
#define _BNTFMT_H

#include <string>
using namespace std;

const string SSM_IDP = "1122334455";
const string SSM_IDC = "300000321000000000000000";
const string SSM_USG = "FFFF";
const string SSM_ENC_MTHD = "02"; //DES ECB

typedef struct{
	string kt;
	string idp;
	string idc;
	string kn;
}KeyIDFmtDef;

typedef struct{
	string idt; //DF10
	KeyIDFmtDef dkid;//D2 decryption key
	KeyIDFmtDef kid;//D2 exchanged key
	string usg;//D3
	string mthd;//DF11
	string kcv;//D4
	string kev;//D5
}KeyTokenFmtDef;

class KeyToken{
private:
	KeyTokenFmtDef token;
public:
	string GetKT();//get KT of exchanged key
	string GetKN();//get KN of exchanged key
	string GetDKT();//get KT of decryption key
	string GetDKN();//get KN of decryption key
	string GetKEV();//get KEV of exchanged key, tag D5
	string GetKCV();//get KCV of exchanged key, tag D4
	string GetTokenStr();//get token string
	int CompTokenFromStr( string tokenStr );//compose token from token string
	string CompToken( string kt, string kn, string dkt, string dkn, string kev, string kcv );//compose token and return token string
};

class BNTFmt{
public:
	static int getlenbuf(int len,char *buf,int *buflen);//get formated length in hex
};
//===========================
#endif //_BNTFMT_H
