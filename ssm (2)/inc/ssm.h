//=============================
// ssm.h
// 
//=============================
#ifndef _SSM_H
#define _SSM_H

#include <openssl/rsa.h>
#include <openssl/objects.h>
#include <string>
using namespace std;

#define RSA_NUM_BITS	 2048
#define RSA_PUB_EXPONENT   65537

class SSM{
private:
	int ProcReq();
public:
	int main( int argc, char **argv );
};//===========================

class LICENSE{
public:
	static int ValidateLicense_RSA(void);
	static int ValidateLicense_Date(void);
};//===========================

class RSAKEY{
public:
	static int RSA_GenKeyFile( char *pcaFileName );
	static RSA* RSA_ReadFromFile(char* filename);
	static int RSA_WriteToFile( RSA* rsa, char* filename, int include_private_data );
	static int RSA_SignWithPriKey(	char *pcaKeyFileName, 
										unsigned char *pcaMsg, 
										int iMsgLen,
										unsigned char *pcaSigMsg, 
										unsigned int *piSigMsgLen );
	static int RSA_VerWithPubKey(	char *pcaKeyFileName, 
										unsigned char *pcaMsg, 
										int iMsgLen,
										unsigned char *pcaSigMsg, 
										int iSigMsgLen );
};//===========================

#endif //_SSM_H
