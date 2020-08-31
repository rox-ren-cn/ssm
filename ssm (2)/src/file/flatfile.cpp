//=============================
// flatfile.cpp
// 
//=============================
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <iostream>
#include <unistd.h>
#include <fstream>

#include "flatfile.h"
using namespace std;
//-----------------------------

//------------------------------------------------------------------
// Function   : OpenFile
// Description: open file
// Input      : fileName - file name with absolute dir
//              openMode - open mode(etc:RW,WA...)
//              R -- open the file for reading 
//              W -- open the file for writing
//              A -- append output
//              B -- open the file in binary mode
//              T -- overwrite the existing file 
// Output     : none
// Return     : 0 - success
//             -1 - fail
//------------------------------------------------------------------
int FlatFile::OpenFile( const string fileName, const string openMode )
{
	ios_base::openmode  opMode;
	if( openMode.find( "R", 0 ) != string::npos )
	{
		opMode = ios::in;
		if( openMode.find( "B", 0 ) != string::npos ){
			opMode = opMode|ios::binary;
		}
		hInFile.open( fileName.c_str(), opMode );
		if( !hInFile )
		{
			sprintf( caMsg, "open file:%s", fileName.c_str() );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}	
	}
	else if( openMode.find( "W", 0 ) != string::npos )
	{
		opMode = ios::out;
		if( openMode.find( "A", 0 ) != string::npos ){
			opMode = opMode|ios::app;
		}
		
		if( openMode.find( "B", 0 ) != string::npos ){
			opMode = opMode|ios::binary;
		}
		
		if( openMode.find( "T", 0 ) != string::npos ){
			opMode = opMode|ios::trunc;
		}
		
		hOutFile.open( fileName.c_str(), opMode );
		if( !hOutFile )
		{
			sprintf( caMsg, "open file:%s", fileName.c_str() );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
			return -1;
		}	
	}
	else
	{
		sprintf( caMsg, "Error open mode,default is:RWABT" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}	

	return 0;
}

//------------------------------------------------------------------
// Function   : CloseFile
// Description: close file
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void FlatFile::CloseFile()
{
	hInFile.close();
	hOutFile.close();
}

//------------------------------------------------------------------
// Function   : Readln
// Description: read one line from current position and move to next line
// Input      : none
// Output     : none
// Return     : data of the line:success
//              "":failure
//------------------------------------------------------------------
string ASCFile::Readln()
{
	memset( caBuffStr, 0, sizeof(caBuffStr) );
	hInFile.getline( caBuffStr, MAXBUFFLEN);
	return (string)caBuffStr;
}

//------------------------------------------------------------------
// Function   : Writeln
// Description: write one line to the end of file
// Input      : inputStr - data to write
// Output     : none
// Return     : 0 - success
//             -1 - fail
//------------------------------------------------------------------
int ASCFile::Writeln( const string inputStr )
{
	hOutFile.seekp( 0, ios::end );
	hOutFile<<inputStr<<endl;
	return 0;
}

//------------------------------------------------------------------
// Function   : ReadBytes
// Description: read input bytes from current position and move cursor
// Input      : bytes - bytes to read
// Output     : pcBuf - bytes of read data
// Return     : 0 - success
//             -1 - fail
//------------------------------------------------------------------
int HexFile::ReadBytes( unsigned char *pcBuf, const int bytes )
{
	memset( caBuffStr, 0, sizeof(caBuffStr) );
	hInFile.read( caBuffStr, bytes );
	if( hInFile.bad() )
	{
		sprintf( caMsg, "Error while ReadBytes" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}
	
	memcpy( pcBuf, caBuffStr, bytes );
	return 0;
}

//------------------------------------------------------------------
// Function   : ReadLine
// Description: read one line or bytes less than readLen from current position and move cursor
// Input      : iMaxLen  - Max bytes to be read
// Output     : pcBuf - bytes of read data
//              pireadLen - read bytes
// Return     : 0 - success
//              1 - eof
//             -1 - fail
//------------------------------------------------------------------
int HexFile::ReadLine( char *pcBuf, int iMaxLen, int *pireadLen )
{
	memset( caBuffStr, 0, sizeof(caBuffStr) );
	hInFile.getline( caBuffStr, iMaxLen );
	if( hInFile.bad() )
	{
		sprintf( caMsg, "Error while ReadLine" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0 );
		return -1;
	}
	
	*pireadLen = hInFile.gcount();
	memcpy( pcBuf, caBuffStr, *pireadLen );
	
	if( hInFile.eof() ){
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------
// Function   : WriteBytes
// Description: write data of bytes to the end of the file
// Input      : pcBuf - bytes of data to write
//              bytes - bytes to write
// Output     : none
// Return     : 0 - success
//             -1 - fail
//------------------------------------------------------------------
int HexFile::WriteBytes( const unsigned char *pcBuf, const int bytes )
{
	hOutFile.seekp( 0, ios::end );
	hOutFile.write( (char *)pcBuf, bytes );
	return 0;
}
//=============================

