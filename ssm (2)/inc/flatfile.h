//=============================
// flatfile.h
// 
//=============================
#ifndef _FLATFILE_H
#define _FLATFILE_H

#include <string>
#include <iostream>
#include <fstream>
using namespace std;
//--------------------------

#define	MAXBUFFLEN	10240
class FlatFile{
protected:
	fstream	hInFile;
	fstream	hOutFile;
	char		caBuffStr[MAXBUFFLEN+1];
public:
	int OpenFile( const string strFileName, const string openMode= "RD" ); //open file with absolute dir
	void CloseFile(); //close file
};//--------------------------

class ASCFile:public FlatFile{
public: 
	string Readln(); //read one line from current position and move to next line
	int Writeln( const string ); //write one line to the end of file
	//int MoveNext(); //move cursor to next line
	//int MovePrev(); //move cursor to previous line
};//===========================

class HexFile:public FlatFile{
public: 
	int ReadBytes( unsigned char *, const int ); //read bytes
	int WriteBytes( const unsigned char *, const int ); //write bytes
	int ReadLine( char *, int iMaxLen, int * );
	//int MoveNext( const int ); //move cursor to next input bytes
	//int MovePrev( const int ); //move cursor to previous input bytes
};//===========================
#endif //_FLATFILE_H
