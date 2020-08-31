/*
******************************************************************
 File       : errlog.cpp
 Author     : Zuo Min
 Create Date: 20070531
 Description: Define error log.
 Modify list: 20070531 created
 
******************************************************************
*/
#include "errlog.h"
#include "util.h"
#include "sys/errno.h"
#include <time.h>
#include <sys/file.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
//#include <fstream>
//#include <iostream>
#include <unistd.h>

//char caMsg[1024];
using namespace std;
//-------------------------------

ERRLOG::ERRLOG()
{
	//printf("ERRLOG\n");
	memset( LOG_FILE_NAME, 0, sizeof(LOG_FILE_NAME) );
	strcpy( LOG_FILE_NAME, "./ics.log" );
	LOG_LEVEL = LOG_LEVEL_INIT; 
	BAK_LOG_SIZE = 1024 * 1024; 
}//-------------------------------

ERRLOG::~ERRLOG()
{
	//printf("~ERRLOG\n");
}//--------------------------------

/*
------------------------------------------------------------------
 Function   : InitLog
 Description: Initialize log file
 Input      : pcLogName - log name
              iLogLevel - log level
                   1 normal
                   2 test
              iLogSize - log size in bytes
 OutPut     : None
 Return     : 0 - OK
             -1 - error
 
------------------------------------------------------------------
*/
int ERRLOG::InitLog( char *pcLogName, int iLogLevel, unsigned long ulLogSize)
{
    int iRc;

    iRc = strlen(pcLogName);
    if((iRc < 1) 
    || (iRc > 100)
    || (( iLogLevel < LOG_LEVEL_DEBUG ) || ( iLogLevel > LOG_LEVEL_ERROR)))
    {
        return -1;
    }

    strcpy( LOG_FILE_NAME, pcLogName );
    LOG_LEVEL = iLogLevel;
    BAK_LOG_SIZE = ulLogSize;

    return 0;
}

/*
------------------------------------------------------------------
 Function   : ErrLog
 Description: Write data into error log file
 Input      : iErrLel - error log level
                  LOG_LEVEL_INIT             0
									LOG_LEVEL_DEBUG            1
									LOG_LEVEL_INFO             2
									LOG_LEVEL_WARN             3
									LOG_LEVEL_ERROR            4
              pcaDispMsg - message with zero terminated
              cDumpDev - reserved
              pcaDumpAddr - memory dump address
              lDumpLen - memory dump length
 OutPut     : None
 Return     : 0 - OK
              1 - error
 
------------------------------------------------------------------
*/
int ERRLOG::ErrLog(const int iErrLel, const char *pcaDispMsg, const char cDumpDev, const unsigned char *pucaDumpAddr
                                                          ,const long lDumpLen)
{
    int i = 0;
	long l = 0;
	long len = 0;
	pid_t logPid;
   unsigned long ulLen = 0;
	unsigned char caDateTime[18];
        char caMsg[8192];
	char caASCMsg[8192];
	char caMsgLabel[10];

/*	mask by ZhangFan,2010.07.16
	if (( LOG_LEVEL == 1 ) && ( iErrLel == 2000 ))
        return 0;
*/
	/* Modify by ZhangFan,2010.07.16 */
	if( iErrLel != LOG_LEVEL_INIT && iErrLel < LOG_LEVEL)
	{
		return 0;
	}
	memset( caMsgLabel, 0, sizeof(caMsgLabel) );
	if( iErrLel == LOG_LEVEL_DEBUG ){
		strcpy( caMsgLabel, "[DEBUG] " );
	}
	else if( iErrLel == LOG_LEVEL_INFO ){
		strcpy( caMsgLabel, "[INFO] " );
	}
	else if( iErrLel == LOG_LEVEL_WARN ){
		strcpy( caMsgLabel, "[WARN] " );
	}
	else if( iErrLel == LOG_LEVEL_ERROR ){
		strcpy( caMsgLabel, "[ERROR] " );
	}
	
	/* Modify end */
        int iLogFd;
        struct stat buf;     
        memset( caDateTime, 0, sizeof(caDateTime));
        DateFmt::GetDateTimeStr((char *)caDateTime);

        if ( (iLogFd = open(LOG_FILE_NAME, O_WRONLY|O_APPEND|O_CREAT, 0777)) == -1 )    
        {
	   fprintf( stderr, "OpenLog: %d %s open error %s %d\n", getpid(), caDateTime, LOG_FILE_NAME, errno );
           return 1;
        }
        
        while(1)
        {
           if ( flock(iLogFd, LOCK_EX) == 0 )
               break;
           else if ( errno = EINTR )
               continue;
           
           fprintf( stderr, "OpenLog: flock error %s %d %d\n", LOG_FILE_NAME, iLogFd, errno );
           close(iLogFd);
           return 1;
        }

	memset( caDateTime, 0, sizeof(caDateTime));
	logPid = getpid();
	DateFmt::GetDateTimeStr((char *)caDateTime);
	sprintf( (char *)caMsg, "pid:%8d| ", logPid );
	strcat( (char *)caMsg,"================== " );
	strncat( (char *)caMsg, (char *)caDateTime, 4 );
	strcat( (char *)caMsg, "-" );
	strncat( (char *)caMsg, (char *)caDateTime + 4, 2 );
	strcat( (char *)caMsg, "-" );
	strncat( (char *)caMsg, (char *)caDateTime + 6, 2 );
	strcat( (char *)caMsg, "  " );
	strncat( (char *)caMsg, (char *)caDateTime + 8, 2 );
	strcat( (char *)caMsg, ":" );
	strncat( (char *)caMsg, (char *)caDateTime + 10, 2 );
	strcat( (char *)caMsg, ":" );
	strncat( (char *)caMsg, (char *)caDateTime + 12, 2 );
	strcat( (char *)caMsg, " " );
	strncat( (char *)caMsg, (char *)caDateTime + 14, 3 );
	strcat( (char *)caMsg, " ===================");
	strcat( (char *)caMsg, "\n" );

    {
        write( iLogFd, caMsg, strlen(caMsg));

    	if ( pcaDispMsg != NULL )
    	{
    	  memset( caMsg, 0, sizeof(caMsg) );
		  sprintf( (char *)caMsg, "pid:%8d| %s", logPid, caMsgLabel );
		  strcat( (char *)caMsg, pcaDispMsg );
        //hFile << caMsg << "\n";
        write( iLogFd, caMsg, strlen(caMsg));
        write( iLogFd, "\n", 1);
    	
    		if ( !( lDumpLen > 0 ))
    		{
    			//hFile << "\n";;
                        write( iLogFd, "\n", 1 );
    		}
    	}

    	if ( lDumpLen > 0 )
    	{
    		memset(caMsg, 0, sizeof(caMsg));
    		len = sprintf((char *)caMsg, "pid:%8d| -1--2--3--4--5--6--7--8-Hex-0--1--2--3--4--5--6", logPid);
    		sprintf((char *)caMsg + len, " --ASCII Value--\n");
    		len = strlen((char *)caMsg);
    		DataFmtConv::hex_asc( caASCMsg, pucaDumpAddr, lDumpLen * 2 );
			sprintf( (char *)caMsg + len, "pid:%8d| ", logPid );
			len += 14;
    		for (l=0; l<lDumpLen; l++)
    		{
    			caMsg[len] = caASCMsg[l*2];
    			caMsg[len+1] = caASCMsg[ l*2 + 1 ];
    			caMsg[len+2] = ' ';
    			len += 3;
    			if ((l+1)%16 == 0)
    			{
    			    for ( i = 0; i<16; i++ )
 			    	 {
            			caMsg[len++] = ((pucaDumpAddr[l + 1 - 16 + i]<0x20)||(pucaDumpAddr[l + 1 - 16 + i]>0x80))?'*':pucaDumpAddr[l + 1 - 16 + i];
    				 }
    				strcat((char *)caMsg, "\n");
					len += 1;
					sprintf((char *)caMsg + len, "pid:%8d| ", logPid );
    				len += 14;
    			}
    		}

    		if ( (lDumpLen%16) > 0 ) //last block
    		{
    			memset(caMsg + len, ' ', 3 * (16-lDumpLen%16));
    			len += 3 * (16-lDumpLen%16);
				
				for ( i = 0; i<lDumpLen%16; i++ )
				{
				      caMsg[len++] = ((pucaDumpAddr[lDumpLen - lDumpLen%16 + i]<0x20)||(pucaDumpAddr[lDumpLen - lDumpLen%16 + i]>0x80))?'*':pucaDumpAddr[lDumpLen - lDumpLen%16 + i];
				}
    		}
    		
    		caMsg[len] = '\n';
    		caMsg[len + 1] = '\n';

			//hFile << caMsg;
                write( iLogFd, caMsg, strlen(caMsg));
    	}
    }
    /*
    catch(CException, e)
    {
        printf("fatal error:write log file!!!\n");
        hFile.close();
        return -1;
    }
    END_CATCH
    */	 
	//hFile.seekp( 0, ios::end );
	//ulLen = hFile.tellp();
	//hFile.close();

    /*
    if ( ulLen >= BAK_LOG_SIZE )
    {
        char caTmp[120];
        memset( caTmp, 0, sizeof(caTmp) );
        strcpy( caTmp, LOG_FILE_NAME );
        strcat( caTmp, "." );
        memcpy( caTmp + strlen(caTmp), caDateTime, 14 );

		  rename( LOG_FILE_NAME, caTmp );
    }
    */

    memset( &buf, 0, sizeof(buf));
    fsync(iLogFd);
    fstat( iLogFd, &buf);
    
    if ( buf.st_size >= BAK_LOG_SIZE )
    {
       char sBakLogFile[255];
       memset( sBakLogFile, 0, sizeof(sBakLogFile));
       sprintf( sBakLogFile, "%s_%s.bak", LOG_FILE_NAME, caDateTime ); 
       if ( link(LOG_FILE_NAME, sBakLogFile) == 0 )
       {
           unlink(LOG_FILE_NAME);
       }
    }

    flock(iLogFd, LOCK_UN);
    close(iLogFd);
    return 0;
}

/*
end of errlog.cpp
*/

