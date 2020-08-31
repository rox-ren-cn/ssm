//=============================
// xmlapp.cpp
// 
//=============================
#include "xmlapp.h"
#include "errlog.h"
#include "util.h"
#include "global.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
using namespace std;
//-----------------------------

XMLApp::XMLApp()
{
	//printf( "XMLApp\n" );
	memset( caMsg, 0, sizeof(caMsg) );
	doc = NULL;
	curNode = NULL;
}//-----------------------------

XMLApp::~XMLApp()
{
	if ( doc != NULL )
	{
		xmlFreeDoc( doc );
		doc = NULL;
		//xmlCleanupParser();
		//xmlMemoryDump();
	}
	//printf( "~XMLApp\n" );
}//-----------------------------

void XMLApp::FreeDoc()
{
	if ( doc != NULL )
	{
		xmlFreeDoc( doc );
		doc = NULL;
		//xmlCleanupParser();
		//xmlMemoryDump();
	}
}//-----------------------------

#if 0
string XMLApp::OpenXMLFile( const string file )
{
	doc = xmlReadFile( file.c_str(), "GB2312",XML_PARSE_RECOVER );

	if ( NULL == doc ) 
	{  
		sprintf( caMsg, "xmlReadFile" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		return ""; 
	} 

	curNode = xmlDocGetRootElement(doc); 
	if ( curNode == NULL )
	{
		sprintf( caMsg, "empty file" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		xmlFreeDoc( doc );
		doc = NULL;
		return ""; 
	}
	
	string s = (const char *)curNode->name;
	return s;
}//-----------------------------

string XMLApp::MovChildNode()
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->xmlChildrenNode;
	if ( curNode == NULL )
	{
		sprintf( caMsg, "info:no child" );
		errLog.ErrLog(1000, caMsg, 0, 0, 0);
		curNode = prevNode;
		return "";
	}

	string s = (const char *)curNode->name;
	if ( s == "text" )
	{
		curNode = curNode->next;
		if ( curNode == NULL )
		{
			sprintf( caMsg, "info:no child" );
			errLog.ErrLog(1000, caMsg, 0, 0, 0);
			curNode = prevNode;
			return "";
		}
		s = (const char *)curNode->name;
	}
		
	return s;
}//-----------------------------

string XMLApp::MovChildNodeSkipComment()
{
	string s = MovChildNode();
	if ( s == "comment" )
	{
		s = MovNextNodeSkipComment();
	}
	return s;
}//-----------------------------

string XMLApp::MovNextNode()
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->next;
	if ( curNode == NULL )
	{
		curNode = prevNode;
		return "";
	}

	string s = (const char *)curNode->name;
	if ( s == "text" )
	{
		curNode = curNode->next;
		if ( curNode == NULL )
		{
			curNode = prevNode;
			return "";
		}
		s = (const char *)curNode->name;
	}
	return s;
}//-----------------------------

string XMLApp::MovNextNodeSkipComment()
{
	string s = "comment";
	while ( s == "comment" )
	{
		s = MovNextNode();
	}
	return s;
}//-----------------------------

string XMLApp::MovParentNode()
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->parent;
	if ( curNode == NULL )
	{
		sprintf( caMsg, "fail:get parent" );
		errLog.ErrLog(1000, caMsg, 0, 0, 0);
		curNode = prevNode;
		return "";
	}

	string s = (const char *)curNode->name;
	return s;
}//-----------------------------

string XMLApp::GetNodeText()
{
	xmlChar* szKey = xmlNodeGetContent(curNode);
	string s = (char *)szKey;            
   xmlFree(szKey);
	szKey = NULL;
	return s;
}//-----------------------------

string XMLApp::GetAttrValue( const string attr )
{
	xmlAttrPtr attrPtr = curNode->properties;
	while (attrPtr != NULL)
	{
		if (!xmlStrcmp(attrPtr->name, BAD_CAST attr.c_str()))
		{
		  xmlChar* szAttr = xmlGetProp( curNode, BAD_CAST attr.c_str() );
		  string s = (char *)szAttr;
		  xmlFree(szAttr);
		  szAttr = NULL;
		  return s;
		}
		attrPtr = attrPtr->next;
	}
}
#endif

int XMLApp::OpenXMLFileNew( const string file, char * root )
{
	doc = xmlReadFile( file.c_str(), "GB2312",XML_PARSE_RECOVER );

	if ( NULL == doc ) 
	{  
		sprintf( caMsg, "xmlReadFile" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		return -1; 
	} 

	curNode = xmlDocGetRootElement(doc); 
	if ( curNode == NULL )
	{
		sprintf( caMsg, "empty file" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		xmlFreeDoc( doc );
		doc = NULL;
		return -1; 
	}

	strncpy( root, (const char *)curNode->name, 1000 );
	return 0;
}//-----------------------------

int XMLApp::MovChildNodeNew( char * child )
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->xmlChildrenNode;
	if ( curNode == NULL )
	{
		sprintf( caMsg, "no child" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		curNode = prevNode;
		return -1;
	}

	strncpy( child, (const char *)curNode->name, 1000 );
	if ( strcmp( child, "text" ) == 0 )
	{
		curNode = curNode->next;
		if ( curNode == NULL )
		{
			sprintf( caMsg, "no child" );
			errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
			curNode = prevNode;
			return -1;
		}
		strncpy( child, (const char *)curNode->name, 1000 );
	}
		
	return 0;
}//-----------------------------

int XMLApp::MovChildNodeSkipCommentNew( char * child )
{
	if ( MovChildNodeNew(child) != 0 )
		return -1;
	
	if ( strcmp(child, "comment" ) == 0 )
	{
		if ( MovNextNodeSkipCommentNew(child) != 0 )
			return -1;
	}
	return 0;
}//-----------------------------

int XMLApp::MovNextNodeNew( char * node )
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->next;
	if ( curNode == NULL )
	{
		curNode = prevNode;
		return -1;
	}

	strncpy( node, (const char *)curNode->name, 1000 );
	if ( strcmp( node, "text" ) == 0 )
	{
		curNode = curNode->next;
		if ( curNode == NULL )
		{
			curNode = prevNode;
			return -1;
		}
		strncpy( node, (const char *)curNode->name, 1000 );
	}
	return 0;
}//-----------------------------

int XMLApp::MovNextNodeSkipCommentNew( char * node )
{
	strcpy( node, "comment" );
	while ( strcmp( node, "comment" ) == 0 )
	{
		if ( MovNextNodeNew( node ) != 0 )
			return -1;
	}
	return 0;
}//-----------------------------

int XMLApp::MovParentNodeNew( char * node )
{
	xmlNodePtr prevNode = curNode;
	curNode = curNode->parent;
	if ( curNode == NULL )
	{
		sprintf( caMsg, "get parent" );
		errLog.ErrLog( LOG_LEVEL_ERROR, caMsg, 0, 0, 0);
		curNode = prevNode;
		return -1;
	}

	strncpy( node, (const char *)curNode->name, 1000 );
	return 0;
}//-----------------------------

int XMLApp::GetNodeTextNew( char * node )
{
	xmlChar* szKey = xmlNodeGetContent(curNode);
	strncpy( node, (char *)szKey, 1000 );
   xmlFree(szKey);
	szKey = NULL;
	return 0;
}//-----------------------------

int XMLApp::GetAttrValueNew( const string attr, char * value )
{
	xmlAttrPtr attrPtr = curNode->properties;
	while (attrPtr != NULL)
	{
		if (!xmlStrcmp(attrPtr->name, BAD_CAST attr.c_str()))
		{
		  xmlChar* szAttr = xmlGetProp( curNode, BAD_CAST attr.c_str() );
		  strncpy( value, (char *)szAttr, 1000 );
		  xmlFree(szAttr);
		  szAttr = NULL;
		  return 0;
		}
		attrPtr = attrPtr->next;
	}
}
//=============================
