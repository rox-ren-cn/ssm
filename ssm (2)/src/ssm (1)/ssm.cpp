//=============================
// ssm.cpp
// 
//=============================
#include "errlog.h"
#include "conffile.h"
#include "commTCPIP.h"
#include "hsmcmd.h"
#include "bntcmd.h"
#include "global.h"
#include "ssm.h"
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
//-----------------------------
ConfFile ssmconf;
CommTCPIP commTCPIP;
ERRLOG errLog;
char caMsg[1024];
//-----------------------------

int ProcReq()
{
	HSMCmd *cmd = NULL;
	if ( ssmconf.GetItem("SSMType") == "BNTng" )
			cmd = new BNTCmd;
	else{
		return -1;
	}
	
	//read request data
	int iRecvBufLen = 0;
	int iSendBufLen = 0;
	int iRet = 0;
	unsigned char ucRecvBuf[1024], ucSendBuf[1024];
	memset( ucRecvBuf, 0, sizeof(ucRecvBuf) );
	memset( ucSendBuf, 0, sizeof(ucSendBuf) );
	iRet = commTCPIP.read( ucRecvBuf, &iRecvBufLen, 0 );
	if ( iRet != 0 )
	{
		delete cmd;
		return iRet;
	}

	if( iRecvBufLen < 14 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "Error Request Format", 0, 0, 0 );
                //send request back in case of error
                memset( ucSendBuf, 0, sizeof(ucSendBuf) );
                memcpy( ucSendBuf, ucRecvBuf, iRecvBufLen );
                iSendBufLen = iRecvBufLen;
	}else
        {
	     //handle request and prepare response
	     if ( cmd->Proc( ucRecvBuf, iRecvBufLen, ucSendBuf, &iSendBufLen ) != 0 )
	     {
                //send request back in case of error
                memset( ucSendBuf, 0, sizeof(ucSendBuf) );
                memcpy( ucSendBuf, ucRecvBuf, iRecvBufLen );
                iSendBufLen = iRecvBufLen;
	     }
	}
	//write response data
	if ( commTCPIP.write( ucSendBuf, iSendBufLen ) != 0 )
	{
		delete cmd;
		return -1;
	}

	delete cmd;
	return 0;
}//-----------------------------

int main( int argc, char **argv )
{
	int iRet = 0;
	void sig_chld(int);
	char caSSMConfigPath[100];
	
	memset( caSSMConfigPath, 0, sizeof(caSSMConfigPath));
	sprintf( caSSMConfigPath, "%s/etc/ssmconf.xml", getenv("SSMWORKPATH") );
	//get ssm config params
	if ( ssmconf.ReadConfFile( caSSMConfigPath, "SSMConf", "conf" ) != 0 )
	{
		printf( "Read config file error,will exit!\n" );
		exit(-1);
	}

	//Get Log Debug Level
	int iLogLvl = 0;
	string LogLvl = ssmconf.GetItem("LOGLEVEL");
	if( LogLvl == "ERROR" )
	{
		iLogLvl = LOG_LEVEL_ERROR;
	}
	else if( LogLvl == "WARN" )
	{
		iLogLvl = LOG_LEVEL_WARN;
	}
	else if( LogLvl == "INFO" )
	{
		iLogLvl = LOG_LEVEL_INFO;
	}
	else
	{
		iLogLvl = LOG_LEVEL_DEBUG;
	}
	
	char caSSMTracePath[100];
	memset( caSSMTracePath, 0, sizeof(caSSMTracePath));
	sprintf( caSSMTracePath, "%s/trace/ssm.log", getenv("SSMWORKPATH") );
	
	errLog.InitLog( caSSMTracePath, iLogLvl, 10*1024*1024 );
	memset( caMsg, 0, sizeof(caMsg) );
	sprintf( caMsg, "SSM starting..." );
	errLog.ErrLog( LOG_LEVEL_INIT, caMsg, 0, 0, 0);
	printf( caMsg );
	printf( "\n" );
	
#ifdef _RSA_VALIDATION
	//license RSA validation
	iRet = LICENSE::ValidateLicense_RSA();
	if( iRet != 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "ValidateLicense_RSA", 0, 0, 0 );
		printf( "Validate ssm License fail,will exit!\n" );
		exit(-1);
	}
#endif

#ifdef _DATE_VALIDATION
	//license RSA validation
	iRet = LICENSE::ValidateLicense_Date();
	if( iRet != 0 ){
		errLog.ErrLog( LOG_LEVEL_ERROR, "ValidateLicense_Date", 0, 0, 0 );
		printf( "Validate ssm License fail,will exit!\n" );
		exit(-1);
	}
#endif

	//communication
	if ( commTCPIP.Bind ( ssmconf.GetItem("LocalIP").c_str(), ssmconf.GetItem("LocalPort").c_str() ) != 0 )
	{
		sprintf( caMsg, "Bind %s %s", ssmconf.GetItem("LocalIP").c_str(), ssmconf.GetItem("LocalPort").c_str() );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		printf( "Fail bind the socket,will exit!\n", caMsg );
		exit(-1);
	}

	if ( commTCPIP.Listen( atoi(ssmconf.GetItem("MaxConnections").c_str())) != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "Listen", 0, 0, 0 );
		printf( "Fail listen on the socket,will exit!\n", caMsg );
		exit(-1);
	}

	printf( "SSM is ready!\n" );
	errLog.ErrLog( LOG_LEVEL_INIT, "SSM is ready", 0, 0, 0 );

	signal( SIGCHLD, sig_chld );
	
	/* exit father process */
	if ( fork() )
		exit(0);

	int iMemFd=0;
	int iChildCnt = 0;
	int *ptr = NULL;
	char caMemFileName[255];
	
	memset( caMemFileName, 0, sizeof(caMemFileName) );
	sprintf( caMemFileName, "./MemSSM.dat" );
	
	iMemFd = open( caMemFileName, O_RDWR|O_CREAT, S_IRWXU );
	if( iMemFd < 0 )
	{
		sprintf( caMsg, "open mmap file error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		commTCPIP.disconn();
		exit(-1);
	}
		
	write( iMemFd, &iChildCnt, sizeof(int) );
	ptr = (int *)mmap( NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, iMemFd, 0 );
	close( iMemFd );

	//check connections
	int status = 0;
	int pid = 0;
	int iAcceptFD = 0;
	while(1)
	{
		if( *ptr > 5 )
		{
			sprintf( caMsg, "Child Process count:[%d] overlimit,sleep for a while", *ptr );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			usleep( 500 );
			continue;
		}
		
		sprintf( caMsg, "Child Process count:[%d]", *ptr );
		errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
		
		iAcceptFD = commTCPIP.Accept();
		if ( iAcceptFD > 0 )
		{
			commTCPIP.SetClientID(iAcceptFD);
			pid = fork();
			if ( pid == -1 )
			{
				errLog.ErrLog( LOG_LEVEL_ERROR, "Fork children process", 0, 0, 0 );
				commTCPIP.disconn();
				exit(-1);
			}
			
			if ( pid == 0 )//child
			{
				//process request
				sprintf( caMsg, "New connection accepted" );
				errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
			
				while(1)
				{
					iRet = ProcReq();
					if ( iRet != 0 )
					{
						if( iRet < 0 )
							errLog.ErrLog( LOG_LEVEL_ERROR, "ProcReq fail,quit!", 0, 0, 0 );

						commTCPIP.disconn();
						break;
					}
					usleep( 100 );
				}
				
				(*ptr) --;
				exit(0);
			}
			else	//father
			{
			        errLog.ErrLog( LOG_LEVEL_INFO, "parent close socket", 0, 0, 0 );
                                commTCPIP.Disconn(iAcceptFD);//parent process close this socket, let only child use it
				(*ptr) ++;
			}
			
		}else{
			errLog.ErrLog( LOG_LEVEL_ERROR, "Connection accept", 0, 0, 0 );
			continue;
		}
	}
}//============================

void sig_chld(int signo)
{
	pid_t pid;
	int istat;

	while ( (pid = waitpid(-1, &istat, WNOHANG ) ) > 0 ){
		sprintf( caMsg, "Child process %d terminated", pid );
		errLog.ErrLog( LOG_LEVEL_INFO, caMsg, 0, 0, 0 );
	}

	return;
}

