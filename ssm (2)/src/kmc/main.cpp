#include <iostream>
#include "kmcKeyInfo.h"
#include "keyTokenTLV.h"
#include "flatfile.h"
#include "kmcUI.h"
#include "errlog.h"

using namespace std;
ERRLOG errLog;
char caMsg[1024]; 

int main()
{
	char caKMCTracePath[100];
	memset( caKMCTracePath, 0, sizeof(caKMCTracePath));
	sprintf( caKMCTracePath, "%s/trace/kmc.log", getenv("SSMWORKPATH") );
	
	memset( caMsg, 0, sizeof(caMsg) );
	errLog.InitLog( caKMCTracePath, LOG_LEVEL_DEBUG, 10*1024*1024 );
	
	errLog.ErrLog( LOG_LEVEL_INIT, "KMC Start", 0, 0, 0);
	kmcUI kmcUIScr;
	kmcUIScr.ProcessForMenuChoice();
	errLog.ErrLog( LOG_LEVEL_INIT, "KMC Quit", 0, 0, 0);
	exit(0);
}
