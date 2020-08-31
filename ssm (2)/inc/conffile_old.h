//=============================
// conffile.h
// 
//=============================
#ifndef _FILEPROC_H
#define _FILEPROC_H

#include <string>
#include "util.h"
using namespace std;

const int MAX_GROUPNUM = 128;
const int MAX_CONFPARMNUM = 128;

class ConfFile{
	Item item[MAX_GROUPNUM][ MAX_CONFPARMNUM ];
public:
	int ReadConfFile( const string, const string, const string );
	string GetItem( string, const int = 0 );
};//---------------------------

#endif //_FILEPROC_H

