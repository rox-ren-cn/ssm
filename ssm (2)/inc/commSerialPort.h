//=============================
// commSerialPort.h
// 
//=============================
#ifndef _COMMSERIALPORT_H
#define _COMMSERIALPORT_H
#include "conffile.h"

using namespace std;

//class CommTCPIP contains functions of both client side and server side
class commSerialPort{
	int    fdForComm;
	string baudRate;
	string deviceName;
	ConfFile SerialPortConf;
private:
	int InitSerialPort(void);
public:	
	int Open( void ); 
	int Write( const unsigned char *pcSndBuf, const int iSndLen ); //write data to established socket
	int Read( unsigned char *pcRecvBuf, int *piRecvLen, const int iTimeOut ); //read data from established socket
	void Close(void);//disconnect as client
};//===========================

#endif //_COMMSERIALPORT_H

