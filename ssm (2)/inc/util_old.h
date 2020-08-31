//=============================
// util.h
// 
//=============================
#ifndef _UTIL_H
#define _UTIL_H

#include <string>
using namespace std;

const int DATETIME_LEN    = 17;//YYYYMMDDHHMMSS
	
typedef struct{
	string name;
	string val;
}Item;
//--------------------------

class DataFmtConv{
private:
	unsigned char Conv(unsigned char Oct);
public:	
	void asc_hex( unsigned char *Ptd, const unsigned long Ld, const char *Pts, const unsigned long Ls);
	void hex_asc( char *Ptd, const unsigned char *Pts, const unsigned long Lg );
	void long_asc( char *Ptd, const int Lgd, const unsigned long *Pts );
	unsigned long asc_long( const char *Pts, const int Ls);
	void long_tab( unsigned char *Ptd , const int Lgd, const unsigned long *Pts);
	unsigned long tab_long( const unsigned char *Pts, const unsigned int Ls );
	void long_bcd( char *Ptd , const int Lgd, const unsigned long *Pts);
	unsigned long bcd_long( const char *Pts, const int Ls );
	void asc_bcd( unsigned char *Ptd, const unsigned long Lgd, const char *Pts, const unsigned long Lgs);
	void bcd_asc( char *Ptd , const unsigned char *Pts , const unsigned long Lg);
};//--------------------------

class DateFmt{
public:	
	void GetDateTimeStr( char *pcDateTime );
	void GetFmtDateTime( char *pcDateTime );
}//===========================

#endif //_UTIL_H
