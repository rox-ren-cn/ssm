//=============================
// des.h
// 
//=============================
#ifndef _DES_H
#define _DES_H

class Des{
private:
	static void transdata( const unsigned char *, unsigned char *, unsigned char * );
	static void DesSel( unsigned char *, unsigned char * );
	static void fnct( unsigned char *, unsigned char * );
public:
	static void encrypt( const unsigned char *, const unsigned char *, unsigned char * );
	static void decrypt( const unsigned char *, const unsigned char *, unsigned char * );
};//===========================

#endif //_DES_H
