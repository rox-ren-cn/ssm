//=============================
// xmlapp.h
// 
//=============================
#ifndef _XMLAPP_H
#define _XMLAPP_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <iostream>
using namespace std;
//--------------------------

class XMLApp{
	xmlDocPtr doc;
	xmlNodePtr curNode;
public: 
	XMLApp();
	~XMLApp();
	string OpenXMLFile( const string );
	int SaveXMLFile( const string );
	int OpenXMLFileNew( const string, char * );
	int MovNextNodeNew( char * );
	int MovNextNodeSkipCommentNew( char * );
	int MovChildNodeNew( char * );
	int MovChildNodeSkipCommentNew( char *  );
	int MovParentNodeNew( char * );
	int GetNodeTextNew( char * );
	int GetAttrValueNew( const string, char * );
	void FreeDoc();
};//===========================

#endif //_XMLAPP_H
