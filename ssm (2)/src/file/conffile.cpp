//=============================
// conffile.cpp
// 
//=============================
#include "conffile.h"
#include "xmlapp.h"
#include "errlog.h"
#include "global.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
using namespace std;

#ifndef XMLFILE
int ConfFile::ReadConfFile( const string f )
{
	ifstream hFile;
	hFile.open( f.c_str() );
	if ( !hFile )
	{
      sprintf( caMsg, "Open %s for read", f.c_str() );
      errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
      return -1;
	}

	string s, t;
	int i = 0;
	int pos1 = 0;
	int pos2 = 0;
	while ( getline( hFile, s ) )
	{
		int pos1 = s.find( ":", 0 );
		if ( pos1 == -1 )
		{
			if ( s.find( "END" ) != -1 )
				break;
			else
				continue;
		}else{
		  item[i].name = "";
			item[i].name.append( s, 0, pos1 );
			pos1 = s.find( "[", pos1 );
			pos2 = s.find( "]", pos1 );
			item[i].val = "";
			item[i].val.append( s, pos1 + 1, pos2 - pos1 - 1 );
			i++;
			if ( i > MAX_CONFPARMNUM )
				break;
		}
	}
	hFile.close();
	return 0;
}//--------------------------------
#else
int ConfFile::ReadConfFile( const string f, const string root, const string type )
{
	XMLApp xmlapp;
	char s[1000];
	memset( caMsg, 0, sizeof(caMsg) );

	memset( s, 0, sizeof(s) );
	if ( ( xmlapp.OpenXMLFileNew( f, s ) != 0 )||( strcmp( s, root.c_str() ) != 0 ) )
	{
		sprintf( caMsg, "xmlapp.OpenXMLFile %s", f.c_str() );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		return -1;
	}

	memset( s, 0, sizeof(s) );
	if ( ( xmlapp.MovChildNodeSkipCommentNew(s) != 0 )||( strcmp( s, type.c_str() ) != 0 ) )
	{
		sprintf( caMsg, "xmlapp.MovChildNodeSkipComment %s", type.c_str() );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		return -1;
	}

	for ( int i = 0; i < MAX_GROUPNUM; i++ )
	{
		for ( int j = 0; j < MAX_CONFPARMNUM; j++ )
		{
			item[i][j].name = "";
			item[i][j].val = "";
		}
	}
	
	for ( int i = 0; i < MAX_GROUPNUM; i++ )
	{
		memset( s, 0, sizeof(s) );
		if ( ( xmlapp.MovChildNodeSkipCommentNew(s) != 0 )||( strcmp( s, "param" ) != 0 ) )
		{
			sprintf( caMsg, "xmlapp.MovChildNodeSkipComment param" );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
			return -1;
		}

		for ( int j = 0; j < MAX_CONFPARMNUM; j++ )
		{
			memset( s, 0, sizeof(s) );
			if (( xmlapp.GetAttrValueNew( "name", s ) != 0 )||(( item[i][j].name = s)== ""))
			{
				sprintf( caMsg, "wrong name %d", j );
				errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
				return -1;
			}

			memset( s, 0, sizeof(s) );
			xmlapp.GetAttrValueNew( "value", s ); //could be ""
			item[i][j].val = s;

			memset( s, 0, sizeof(s) );
			if ( ( xmlapp.MovNextNodeSkipCommentNew(s) != 0 )||( strlen(s) == 0 ) )
			{
				break;
			}
		}

		memset( s, 0, sizeof(s) );
		if (( xmlapp.MovParentNodeNew(s) != 0)||( strcmp( s, type.c_str() ) != 0 ) )
		{
			sprintf( caMsg, "xmlapp.MovParentNode %s", type.c_str() );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
			return -1;
		}

		memset( s, 0, sizeof(s) );
		if (( xmlapp.MovNextNodeSkipCommentNew(s) != 0 )||( strcmp( s, type.c_str() ) != 0 ) )
		{
			break;
		}
	}

	return 0;
}//--------------------------
#endif

string ConfFile::GetItem( const string s, const int idx  )
{
	for ( int i = 0; i < MAX_CONFPARMNUM; i++ )
	{
		if ( item[idx][i].name == s )
			return item[idx][i].val;
	}

	return "";
}
//===================================================
