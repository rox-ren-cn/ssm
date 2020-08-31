//=============================
// commTCPIP.cpp
// 
//=============================
#include "commTCPIP.h"
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <unistd.h>   
#include <sys/uio.h>   
#include <arpa/inet.h>
#include <string.h>   
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;
//-----------------------------
/*
------------------------------------------------------------------
 Function   : RtcsInitTcp
 Description: Initialize TCP
 Input      : TcpFd - file ID
 Output     : null
 Return     : 0 - success
             -1 - fail
 
------------------------------------------------------------------
*/
int CommTCPIP::RtcsInitTcp (int TcpFd)
{
    int optlen;
    int   on=1,rcvLowBuf=5;
    struct linger LingerVar;

    LingerVar.l_onoff = 1;
    LingerVar.l_linger = 0;
    optlen = sizeof (LingerVar);
    if (setsockopt (TcpFd, SOL_SOCKET, SO_LINGER,
                        (char *)&LingerVar, optlen) !=0)
    {
			sprintf( caMsg, "setsockopt error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}

    if (setsockopt (TcpFd, SOL_SOCKET, SO_OOBINLINE,
                        (char *)&on, sizeof (on)) !=0)
    {
			sprintf( caMsg, "setsockopt error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}
		
		if (setsockopt ( TcpFd, SOL_SOCKET, SO_RCVLOWAT,
                        (int *)&rcvLowBuf, sizeof (rcvLowBuf)) !=0)
    {
			sprintf( caMsg, "setsockopt error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}

    return 0;
}

/*
------------------------------------------------------------------
 Function   : bind
 Description: bind socket
 Input      : ip - local IP
            : port - local port
 OutPut     : none
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::Bind( const char *ip, const char *port )
{
	long laddr;
	struct sockaddr_in stServer;
	
	iS = socket(AF_INET, SOCK_STREAM, 0);
  if ( iS < 0) 
  {
		sprintf( caMsg, "Create socket error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}
    
	bzero( &stServer, sizeof(stServer) );
	inet_pton( AF_INET, ip, (struct in_addr *)&laddr );
	stServer.sin_family = AF_INET;
	stServer.sin_addr.s_addr = laddr;
	stServer.sin_port = htons( atoi(port) );
	
	if( bind( iS, (struct sockaddr *)&stServer, sizeof(stServer) ) < 0 )
	{
		sprintf( caMsg, "bind socket error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		close(iS);
		return -1;
	}
	
	RtcsInitTcp(iS);
	return 0;
}

/*
------------------------------------------------------------------
 Function   : listen
 Description: listen to the bind socket
 Input      : backlog - max connections number
 OutPut     : none
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::Listen( int backlog )
{
	if( listen( iS, backlog ) < 0 )
	{
		sprintf( caMsg, "listen error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		close(iS);
		return -1;
	}
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : accept
 Description: accept a connection
 Input      : null
 OutPut     : null
 Return     : >0 Client Socket ID
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::Accept()
{
	iSockID = accept( iS, NULL, NULL );
	if ( iSockID < 0 )
	{
		sprintf( caMsg, "accept error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}
	return iSockID;	
}

/*
------------------------------------------------------------------
 Function   : conn
 Description: setup collection as client
 Input      : ip - target IP
            : port - target port
 OutPut     : none
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::conn( const char *ip, const char *port )
{
    int iRet;
    struct sockaddr_in dest_addr;

    iRet = 0;
    memset((char *)&dest_addr, 0, sizeof(struct sockaddr_in));

    iSockID = socket(AF_INET, SOCK_STREAM, 0);
    if ( iSockID < 0) 
		{
			sprintf( caMsg, "Create socket error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(port));
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    iRet = connect(iSockID, (struct sockaddr *)&dest_addr,
       sizeof(struct sockaddr));
    if ( iRet < 0) 
    {
			sprintf( caMsg, "connect error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			close(iSockID);
			return -1;
		}
		
    RtcsInitTcp(iSockID);

    return 0;
}

/*
------------------------------------------------------------------
 Function   : read
 Description: read data from established socket
 Input      : iTimeOut - timeout by seconds
 Output     : pcRecvBuf - pointer to receiver buffer
              piRecvLen - pointer to receiver buffer length
 Return     : 0 - success
             -1 - failed
              1 - connection close 
------------------------------------------------------------------
*/
int CommTCPIP::read( unsigned char *pcRecvBuf, int *piRecvLen, const int iTimeOut )
{
    int  iMsgLen=0;
    char caMsgBuf[MD_SEC_LEN_MSGBUF+1];
    fd_set rfds;
    struct timeval tv;
    int retval;
 
    iMsgLen = 0;

    FD_ZERO(&rfds);
    FD_SET(iSockID, &rfds);
    tv.tv_sec = iTimeOut;
    tv.tv_usec = 0;

	 
	if ( iTimeOut == 0 )
		retval = select(iSockID+1, &rfds, NULL, NULL, NULL );
	else
		retval = select(iSockID+1, &rfds, NULL, NULL, &tv);
    
    switch(retval)
    {
        case -1:
        				sprintf( caMsg, "Select Error: %s", strerror(errno) );
								errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
                return -1;
        case 0:
        	sprintf( caMsg, "Read Message TimeOut" );
								errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
                return -1;
        default:
                break;/* continue to read */
    }

		if( FD_ISSET( iSockID, &rfds ) )
		{
			iMsgLen = recv(iSockID, caMsgBuf, MD_SEC_MAXLEN_TCPIPBUF, 0 );
		}
    

    if ( iMsgLen < 0 ) 
    {
			sprintf( caMsg, "recv error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}
		else if ( iMsgLen == 0 ) 
    {
    	sprintf( caMsg, "Client disconnect" );
			errLog.ErrLog( LOG_LEVEL_WARN, caMsg, 0, 0, 0 );
			return 1;
		}
		
    *piRecvLen = iMsgLen;
    memcpy( pcRecvBuf,caMsgBuf,iMsgLen );
    return 0;
}

/*
------------------------------------------------------------------
 Function   : write
 Description: write data to established socket
 Input      : pcSndBuf - pointer to send buffer
              iSndLen  - send buffer length
 Output     : null
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::write( const unsigned char *pcSndBuf, const int iSndLen )
{
	int iMsgLen;
	char caMsgBuf[MD_SEC_LEN_MSGBUF+1];

	iMsgLen = 0;
	memset(caMsgBuf,0x00,sizeof(caMsgBuf));

	/* modified 20091029 for exclude length and include length
	caMsgBuf[0] = iSndLen/256;
	caMsgBuf[1] = iSndLen%256;
	memcpy(caMsgBuf+2,pcSndBuf,iSndLen);
	*/
	memcpy(caMsgBuf,pcSndBuf,iSndLen);
	iMsgLen = send(iSockID, caMsgBuf, iSndLen, 0);
	if ( iMsgLen <= 0 )
	{
		sprintf( caMsg, "send error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}

	return 0;
}

/*
------------------------------------------------------------------
 Function   : disconn
 Description: disconnect and close server
 Input      : null
 Output     : null
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::disconn()
{
    int iRet;
  
    iRet = 0;

    iRet = close( iSockID );
    if ( iRet != 0 )
    {
			sprintf( caMsg, "close client socket error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}
		
		iRet = close(iS);
    if ( iRet != 0 )
    {
			sprintf( caMsg, "close server socket error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}
		
    return 0;
}//=============================

/*
------------------------------------------------------------------
 Function   : Disconn
 Description: close accepted socket
 Input      : iAcceptID - accpeted socket ID
 Output     : null
 Return     : 0 - success
             -1 - failed
 
------------------------------------------------------------------
*/
int CommTCPIP::Disconn( int iAcceptID )
{
    int iRet;
  
    iRet = 0;

    iRet = close( iAcceptID );
    if ( iRet != 0 )
    {
			sprintf( caMsg, "close client socket error: %s", strerror(errno) );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
    }
    return 0;
}//=============================

int CommTCPIP::SetClientID( int iClientID )
{
	iSockID = iClientID;
	return 0;
}
