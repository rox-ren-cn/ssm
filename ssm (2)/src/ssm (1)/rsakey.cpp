//=============================
// rsakey.cpp
// 
//=============================
#include "errlog.h"
#include "conffile.h"
#include "commTCPIP.h"
#include "hsmcmd.h"
#include "bntcmd.h"
#include "sec.h" 
#include "global.h"
#include "ssm.h"
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <openssl/rsa.h>
#include <openssl/objects.h>
using namespace std;

/*
** Function Name : RSA_WriteToFile
** Description   : Write RSA Key Data to special file
** Input         : RSA* rsa      -- RSA struct point
**               : char* filename  -- FileName 
**               : int include_private_data
	                       0-don't include private data
	                       1-include private data
**  Output       : 
**  Return       : 0   Succ
**               : -1  Fail
*/
int RSAKEY::RSA_WriteToFile( RSA* rsa, char* filename, int include_private_data )
{
  FILE* keyfile = fopen( filename, "wb");
	if( keyfile == NULL )
	{
		sprintf( caMsg, "Open file:[%s] for write error!", filename );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}

  fprintf(keyfile,"%d\n",include_private_data);
  fprintf(keyfile,"%s\n",BN_bn2hex(rsa->n));
  fprintf(keyfile,"%s\n",BN_bn2hex(rsa->e));
  if (include_private_data)
  {
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->d));
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->p));
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->q));
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->dmp1));
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->dmq1));
    fprintf(keyfile,"%s\n",BN_bn2hex(rsa->iqmp));
  }
  fclose(keyfile);
  
  return 0;
}

/*
** Function Name : RSA_ReadFromFile
** Description   : Read RSA Key from special file
** Input         : char* filename  -- FileName 
**  Output       : 
**  Return       : RSA* rsa   --Succ
**               : NULL       --Fail
*/
RSA* RSAKEY::RSA_ReadFromFile( char* filename )
{
	RSA* rsa = RSA_new();
	FILE* keyfile = fopen(filename,"rb");
	int include_private_data = 0;
	int max_hex_size = (RSA_NUM_BITS / 4) + 1;
	char *buffer = (char*) malloc(max_hex_size);

	if( keyfile == NULL )
	{
		RSA_free(rsa);
		free(buffer);
		sprintf( caMsg, "Open file:[%s] for read error!", filename );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return NULL;
	}
		
	fscanf(keyfile,"%d",&include_private_data);

	fscanf(keyfile,"%s",buffer);
	BN_hex2bn(&rsa->n, buffer);

	fscanf(keyfile,"%s",buffer);
	BN_hex2bn(&rsa->e, buffer);

	if (include_private_data)
	{
	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->d, buffer);

	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->p, buffer);

	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->q, buffer);

	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->dmp1, buffer);

	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->dmq1, buffer);

	  fscanf(keyfile,"%s",buffer);
	  BN_hex2bn(&rsa->iqmp, buffer);
	}

	free(buffer);
	return rsa;
}

/*
** Function Name : RSA_SignWithPriKey
** Description   : Signature the pcaMsg with the RSA Private Key which read from pcaKeyFileName
** Input         : pcaKeyFileName   -- File Name which stored RSA Private Key
**               : pcaMsg           -- Message to be Signatured
**               : iMsgLen          -- Length of Message to be Signatured
**  Output       : pcaSigMsg        -- Signatured Message
**               : piSigMsgLen      -- Length of Signatured Message
**  Return       : 0   Succ
**               : -1  Fail
*/
int RSAKEY::RSA_SignWithPriKey(	char *pcaKeyFileName, 
										unsigned char *pcaMsg, 
										int iMsgLen,
										unsigned char *pcaSigMsg, 
										unsigned int *piSigMsgLen )
{
	int iRet=0;

  //Sign a message using private key
  RSA* private_key = RSA_ReadFromFile( pcaKeyFileName );
  if( private_key == NULL )
	{
		sprintf( caMsg, "RSA_ReadFromFile error!" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		RSA_free(private_key);
		return -1;
	}
	
  iRet = RSA_sign( NID_md5, pcaMsg, iMsgLen, pcaSigMsg, piSigMsgLen, private_key );
  if( iRet != 1 )
	{
		sprintf( caMsg, "RSA_sign error!", __FILE__, __LINE__ );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		RSA_free(private_key);
		return -1;
  }
  
  RSA_free(private_key);
  return 0;
}

/*
** Function Name : RSA_VerWithPubKey
** Description   : Verify the pcaMsg with the RSA Public Key which read from pcaKeyFileName
** Input         : pcaKeyFileName   -- File Name which stored RSA Public Key
**               : pcaMsg           -- Original Message
**               : iMsgLen          -- Original Message Length
**               : pcaSigMsg        -- Signatured Message
**               : iSigMsgLen       -- Length of Signatured Message
**  Output       : 
**  Return       : 0   --Signature Verification Successful
**               : -1  --Fail
*/
int RSAKEY::RSA_VerWithPubKey(	char *pcaKeyFileName, 
										unsigned char *pcaMsg, 
										int iMsgLen,
										unsigned char *pcaSigMsg, 
										int iSigMsgLen )
{
	int iRet=0;

	//Verify the message using public key
  RSA* public_key = RSA_ReadFromFile( pcaKeyFileName );
  if( public_key == NULL )
	{
		sprintf( caMsg, "RSA_ReadFromFile error!" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		RSA_free(public_key);
		return -1;
	}
  iRet = RSA_verify( NID_md5, pcaMsg, iMsgLen, pcaSigMsg, iSigMsgLen, public_key );
	if( iRet != 1 )
	{
		sprintf( caMsg, "RSA_verify error!" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		errLog.ErrLog( LOG_LEVEL_ERROR, "Ori Msg:", 0, pcaMsg, iMsgLen );
		errLog.ErrLog( LOG_LEVEL_ERROR, "Sig Msg:", 0, pcaSigMsg, iSigMsgLen );
		RSA_free(public_key);
		return -1;
	}
  
  RSA_free(public_key);
  return 0;
}

/*
** Function Name : RSA_GenKeyFile
** Description   : Generate RSA Key pair and Write them in files
** Input         : pcaFileName   -- File Name which stored RSA Keys
**  Output       : 
**  Return       : 0   --Succ
**               : -1  --Fail
*/
int RSAKEY::RSA_GenKeyFile( char *pcaFileName )
{
	int iRet = 0;
	int iRSAKeyFlag;    //0-Public Key  1-Private Key
	char caFileName[255];
	RSA *rsa = RSA_generate_key( RSA_NUM_BITS, RSA_PUB_EXPONENT, NULL, NULL );

	iRSAKeyFlag = 0;
	memset( caFileName, 0, sizeof(caFileName) );
	sprintf( caFileName, "%s%s", pcaFileName, ".pub" );
	iRet = RSA_WriteToFile( rsa, caFileName, iRSAKeyFlag );
	if( iRet != 0 )
	{
		sprintf( caMsg, "RSA_WriteToFile for pubkey error!" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		RSA_free(rsa);
		return -1;
	}
	sprintf( caMsg, "RSA_WriteToFile [%s] succ", caFileName );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
		
	iRSAKeyFlag = 1;
	memset( caFileName, 0, sizeof(caFileName) );
	sprintf( caFileName, "%s%s", pcaFileName, ".pri" );
	iRet = RSA_WriteToFile( rsa, caFileName, iRSAKeyFlag );
	if( iRet != 0 )
	{
		sprintf( caMsg, "RSA_WriteToFile for private error!" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		RSA_free(rsa);
		return -1;
	}	
	sprintf( caMsg, "RSA_WriteToFile [%s] succ!", caFileName );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	RSA_free(rsa);
	return 0;
}

