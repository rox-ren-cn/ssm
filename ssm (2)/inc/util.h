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
}Item;//--------------------------

class DataFmtConv{
private:
	static unsigned char Conv(unsigned char Oct);
public:	
	static void asc_hex( unsigned char *Ptd, const unsigned long Ld, const char *Pts, const unsigned long Ls);
	static void hex_asc( char *Ptd, const unsigned char *Pts, const unsigned long Lg );
	static void long_asc( char *Ptd, const int Lgd, const unsigned long *Pts );
	static unsigned long asc_long( const char *Pts, const int Ls);
	static void long_tab( unsigned char *Ptd , const int Lgd, const unsigned long *Pts);
	static unsigned long tab_long( const unsigned char *Pts, const unsigned int Ls );
	static void long_bcd( char *Ptd , const int Lgd, const unsigned long *Pts);
	static unsigned long bcd_long( const char *Pts, const int Ls );
	static void asc_bcd( unsigned char *Ptd, const unsigned long Lgd, const char *Pts, const unsigned long Lgs);
	static void bcd_asc( char *Ptd , const unsigned char *Pts , const unsigned long Lg);
	static int Pack( unsigned char *pbyInBuffer, unsigned char *pbyOutBuffer, int iInBuffLen );
	static int Unpack( unsigned char *pbyInBuffer, unsigned char *pbyOutBuffer, int iInBufLen );
	static string CvtLetterCase( string inputStr, int cvtFlag );
	static int HexStr2Int( string inputStr );
	static string NumToWord( string numStr );
};//--------------------------

class DateFmt{
public:	
	static void GetDateTimeStr( char *pcDateTime );
	static void GetFmtDateTime( char *pcDateTime );
};//===========================

class DataFmtCheck{
public:	
	static int CheckIntStr( const char *pcaInputStr, int iStrLen );
	static int CheckLetterStr( const char *pcaInputStr, int iStrLen );
	static int CheckHexStr( const char *pcaInputStr, int iStrLen );
};//===========================

#endif //_UTIL_H
