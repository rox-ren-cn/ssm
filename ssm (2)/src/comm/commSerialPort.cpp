//=============================
// commSerialPort.cpp
//
//=============================
#include "commSerialPort.h"
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>


using namespace std;
//-----------------------------
/*
------------------------------------------------------------------
 Function   : InitSerialPort
 Description: Initialize Serial Port
 Input      :
 Output     : null
 Return     : 0 - success
             -1 - fail

------------------------------------------------------------------
*/
int commSerialPort::InitSerialPort()
{
	char caSSMConfigPath[100];
	
	memset( caSSMConfigPath, 0, sizeof(caSSMConfigPath));
	sprintf( caSSMConfigPath, "%s/etc/ssmconf.xml", getenv("SSMWORKPATH") );
	if ( SerialPortConf.ReadConfFile( caSSMConfigPath, "SSMConf", "conf" ) != 0 )
	{
		errLog.ErrLog( LOG_LEVEL_ERROR, "read SerialPortConf error", 0, 0, 0 );
		return -1;
	}

	baudRate = SerialPortConf.GetItem("BaudRate");
	deviceName = SerialPortConf.GetItem("DeviceName");
	return 0;
}

/*
------------------------------------------------------------------
 Function   : Open
 Description: Open Serial Port
 Input      :
 OutPut     :
 Return     : 0 - success
             -1 - failed

------------------------------------------------------------------
*/
int commSerialPort::Open()
{
	int iRet = 0;

	iRet = InitSerialPort();
	if( iRet != 0 ){
		return iRet;
	}

	struct	termios	term;
	fdForComm = open( deviceName.c_str(), O_RDWR|O_NOCTTY|O_NDELAY );
	if( fdForComm < 0 ){
		sprintf( caMsg, "open Serial Port error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}

	memset( &term, 0, sizeof(term) );
	iRet = tcgetattr( fdForComm, &term );
	if( iRet < 0 ){
		sprintf( caMsg, "tcgetattr error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		Close();
		return -1;
	}

	term.c_cflag = CS8|CREAD|CLOCAL;
	term.c_oflag = OPOST;
	term.c_iflag = IXON|IXOFF|IGNBRK|ISTRIP|IGNCR;
	term.c_lflag = 0;
	term.c_cc[VMIN]  = 1;
	term.c_cc[VTIME] = 0;
	
	if( baudRate == "B4800" ){
		cfsetispeed( &term, B4800 );
		cfsetospeed( &term, B4800 );
	}
	else if( baudRate == "B9600" ){
		cfsetispeed( &term, B9600 );
		cfsetospeed( &term, B9600 );
	}
	else if( baudRate == "B19200" ){
		cfsetispeed( &term, B19200 );
		cfsetospeed( &term, B19200 );
	}
	else{
		sprintf( caMsg, "UnSupported Baud Rate: %s", baudRate.c_str() );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		Close();
		return -1;
	}

	iRet = tcsetattr( fdForComm, TCSANOW, &term );
	if( iRet < 0 ){
		sprintf( caMsg, "tcsetattr error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		Close();
		return -1;
	}

	return 0;
}

/*
------------------------------------------------------------------
 Function   : read
 Description: read data from Serial Port
 Input      : iTimeOut - timeout by seconds
 Output     : pcRecvBuf - pointer to receiver buffer
              piRecvLen - pointer to receiver buffer length
 Return     : 0 - success
             -1 - failed

------------------------------------------------------------------
*/
int commSerialPort::Read( unsigned char *pcRecvBuf, int *piRecvLen, const int iTimeOut )
{
	memset( pcRecvBuf, 0, sizeof(pcRecvBuf) );
	*piRecvLen = 0;
	return 0;
}

/*
------------------------------------------------------------------
 Function   : write
 Description: write data to Serial Port
 Input      : pcSndBuf - pointer to send buffer
              iSndLen  - send buffer length
 Output     : null
 Return     : 0 - success
             -1 - failed

------------------------------------------------------------------
*/
int commSerialPort::Write( const unsigned char *pcSndBuf, const int iSndLen )
{
	int iRet;

	iRet = write( fdForComm, pcSndBuf, iSndLen );
	if( iRet < 0 ){
		sprintf( caMsg, "write data error: %s", strerror(errno) );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}
	
	return 0;
}

/*
------------------------------------------------------------------
 Function   : Close
 Description: Close Serial Port
 Input      : null
 Output     : null
 Return     : null
------------------------------------------------------------------
*/
void commSerialPort::Close()
{
	close(fdForComm);
	return;
}//=============================

