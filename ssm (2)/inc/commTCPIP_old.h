//=============================
// commTCPIP.h
// 
//=============================
#ifndef _COMMTCPIP_H
#define _COMMTCPIP_H

using namespace std;

#define MD_SEC_LEN_MSGBUF            2048
#define MD_SEC_READHSM_INTERVAL      3
#define MD_SEC_MAXLEN_TCPIPBUF       2048

//class CommTCPIP contains functions of both client side and server side
class CommTCPIP{
	int iSockID; //accepted socket
	int iS; //bind socket
private:
	int RtcsInitTcp (int);
public:	
	int Bind( const char *, const char * ); //bind socket
	int Listen( int ); //listen to the bind socket
	int Accept(); //accept a connection
	int conn( const char *, const char * ); //setup collection as client
	int write( const unsigned char *, const int ); //write data to established socket
	int read( unsigned char *, int *, const int ); //read data from established socket
	int disconn();//disconnect as client
};//===========================

#endif //_COMMTCPIP_H

