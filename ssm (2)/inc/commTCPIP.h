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
	int SetClientID( int iClientID );
	int Bind( const char *ip, const char *port ); //bind socket
	int Listen( int backlog ); //listen to the bind socket
	int Accept(); //accept a connection
	int conn( const char *ip, const char *port ); //setup collection as client
	int write( const unsigned char *pcSndBuf, const int iSndLen ); //write data to established socket
	int read( unsigned char *pcRecvBuf, int *piRecvLen, const int iTimeOut ); //read data from established socket
	int disconn( void);//disconnect as client and close server
	int Disconn( int iClientID );//close client socket
};//===========================

#endif //_COMMTCPIP_H

