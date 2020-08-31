/*
******************************************************************
 File       : errlog.h
 Author     : Zuo Min
 Create Date: 20070531
 Description: Define error log.
 Modify list: 20070531 created
 
******************************************************************
*/
#ifndef _ERRLOG_H
#define _ERRLOG_H

#include <string>
using namespace std;

#define  LOG_LEVEL_INIT             0
#define  LOG_LEVEL_DEBUG            1
#define  LOG_LEVEL_INFO             2
#define  LOG_LEVEL_WARN             3
#define  LOG_LEVEL_ERROR            4

class ERRLOG{
	char LOG_FILE_NAME[100];
	int LOG_LEVEL;
	unsigned long BAK_LOG_SIZE;
public:
	ERRLOG();
	~ERRLOG();
	int InitLog( char *, int, unsigned long );
	int ErrLog( const int, const char *, const char, const unsigned char *, const long );
};//==============================================================

#endif //_ERRLOG_H

