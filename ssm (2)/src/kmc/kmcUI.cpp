//=============================
// kmcUI.cpp
//
//=============================
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "errlog.h"
#include "global.h"
#include "kmcUI.h"
#include "kmcKeyInfo.h"
#include "bntcmd.h"
#include "util.h"

#include "bntfmt.h"
#include "bntcmd.h"
#include "hsmcmd.h" 
#include "sec.h" 

using namespace std;

kmcUI::kmcUI()
{
	int iRet;
	string strChoice;
	string strWorkPath = getenv("SSMWORKPATH");
	char caKMCConfigPath[100];
	
	memset( caKMCConfigPath, 0, sizeof(caKMCConfigPath));
	sprintf( caKMCConfigPath, "%s/etc/kmcconf.xml", getenv("SSMWORKPATH") );
	if ( kmcconf.ReadConfFile( caKMCConfigPath, "KMCConf", "conf" ) != 0 )
	{
		cout<<"read kmcconf.xml error!"<<endl;
	}

	strKmcKeyFileName = strWorkPath+"/kmcfiles/"+kmcconf.GetItem("KMCKeyFile");
	strOutputFilePath = kmcconf.GetItem("OutFilePath");
	strNKComponentType = kmcconf.GetItem("NKComponentType");
	
	//44 is default value
	if( strNKComponentType != "62" ){
		strNKComponentType = "44";
		strSubTypeRange = "0-F";
		strKeyVerRange = "0-F";
		strSubType_DEFAULT = "0";
	}
	else{
		strSubTypeRange = "00-63";
		strKeyVerRange = "0-3";
		strSubType_DEFAULT = "00";
	}
	sprintf( caMsg, "NKComponentType:%s", strNKComponentType.c_str() );
	errLog.ErrLog( LOG_LEVEL_DEBUG, caMsg, 0, 0, 0 );
	
	KeyInfo.SetNKComponentType(strNKComponentType);
	
	iRet = Comm_Cli.conn( kmcconf.GetItem("SecIP").c_str(), kmcconf.GetItem("SecPort").c_str() );
	if( iRet != 0 ){
		cout<<"Connect to SSM server error,Input q for quit or any other key for continue..."<<endl;
		strChoice = getchar();
		if( strChoice == "q" || strChoice == "Q" ){
			exit(0);
		}
		//exit(0);
	}
  strErrMsg = "";
	iBNTngCmdLen_snd = 0;
	iBNTngCmdLen_rcv = 0;
	memset( caBNTngCmd_snd, 0, sizeof(caBNTngCmd_snd) );
	memset( caBNTngCmd_rcv, 0, sizeof(caBNTngCmd_rcv) );

}

kmcUI::~kmcUI()
{
	Comm_Cli.disconn();
}

void kmcUI::ReConnect()
{
	Comm_Cli.disconn();
	Comm_Cli.conn( kmcconf.GetItem("SecIP").c_str(), kmcconf.GetItem("SecPort").c_str() );
}

//------------------------------------------------------------------
// Function   : DisplayChoiceMenu
// Description: Display main choice menu for KMC control
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::DisplayChoiceMenu()
{
	system("clear");
	cout<<"********************************************************************************"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**                    Welcome to use Key ManageMent Centre                    **"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**     1. Generate Key                                                        **"<<endl;
	cout<<"**     2. Input Key                                                           **"<<endl;
	cout<<"**     3. Import Key                                                          **"<<endl;
	cout<<"**     4. Query and OutPut Key                                                **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**     q. Quit                                                                **"<<endl;
	cout<<"********************************************************************************"<<endl;
}

//------------------------------------------------------------------
// Function   : ProcessForMenuChoice
// Description: process choice from main menu
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::ProcessForMenuChoice()
{
	while(1){
		DisplayChoiceMenu();
		cout<<"Please Input your Choice:"<<endl;
		cin>>strMenuID;
		cin.ignore();

		if( strMenuID.length() > 1 ){
			cout<<"Invalid Choice!"<<endl;
			getchar();
			continue;
		}

		switch (strMenuID.c_str()[0]) {
			case '1':
				//Generate Key;
				GenKmcKey();
				break;
			case '2':
				//Input Key;
				InputKmcKey();
				break;
			case '3':
				//Import Key;
				ImportKeyFile();
				break;
			case '4':
				//Query Key;
				QueryKey();
				break;
#if 0
			case 'r':
				ReConnect();
				break;
			case 't':
				Test();
				break;
#endif
			case 'q':
				return;
				break;
			default:
				cout<<"Invalid Choice!"<<endl;
				getchar();
				break;
		}
	}
}

//------------------------------------------------------------------
// Function   : DisplayMenu_GenKmcKey
// Description: Display choice menu for GenKmcKey control
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::DisplayMenu_GenKmcKey()
{
	system("clear");
	cout<<"********************************************************************************"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**                              1. Generate Key                               **"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**     1. KeyType                       Input Value:"<<KeyInfo.GetKeyType();	cout.width((28-KeyInfo.GetKeyType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     2. SubType                       Input Value:"<<KeyInfo.GetSubType();	cout.width((28-KeyInfo.GetSubType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     3. KeyVersion                    Input Value:"<<KeyInfo.GetKeyVersion();	cout.width((28-KeyInfo.GetKeyVersion().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     4. Description                   Input Value:";
	if( KeyInfo.GetDesc().length() <= 26 ){
		cout<<KeyInfo.GetDesc();
		cout.width((28-KeyInfo.GetDesc().length()));
		cout<<"**"<<endl;	cout.width();
	}
	else{
		cout<<KeyInfo.GetDesc().substr(0,26); cout<<"**"<<endl;
		cout<<"**                                      ";
		cout<<KeyInfo.GetDesc().substr(26,KeyInfo.GetDesc().length()-26);
		cout.width((40+26-KeyInfo.GetDesc().length()));
		cout<<"**"<<endl;
	}
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**     r. Return            g. Generate Key         o. Output Key             **"<<endl;
	cout<<"********************************************************************************"<<endl;
}

//------------------------------------------------------------------
// Function   : GenKmcKey
// Description: Generate KMC Key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::GenKmcKey()
{
	int    iRet=0;
	unsigned char caUnPackStr[1024+1];
	string inputStr;

	KeyInfo.ResetValue();

	while(1){
		iRet = 0;
		strErrMsg = "";
		DisplayMenu_GenKmcKey();
		cout<<"Please Input your Choice:"<<endl;
		cin>>inputStr;
		//ignore \n in cin input stream
		cin.ignore();

		if( inputStr.length() > 1 ){
			cout<<"Invalid Choice!"<<endl;
			getchar();
			continue;
		}

		switch (inputStr.c_str()[0]) {
			case '1':	//KeyType
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYTYPE_LABEL, "00-FF" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyType!";
				}
				break;
			case '2':	//SubType
				iRet = KeyInfo.SetStrFromScrInput( KMC_SUBTYPE_LABEL, strSubTypeRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid SubType!";
				}
				break;
			case '3':	//KeyVersion
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYVERSION_LABEL, strKeyVerRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyVersion!";
				}
				break;
			case '4':	//Description
				KeyInfo.SetStrFromScrInput( KMC_DESC_LABELSTR, "" );
				break;
			case 'g':
				iRet = KeyInfo.Check();
				if( iRet != 0 ){
					strErrMsg = "Check KeyInfo Error!";
					break;
				}

				iRet = GenCmd( KMC_CMD_TYPE_GENKEY );
				if( iRet != 0 ){
					break;
				}

				iRet = SendCmdToSSM();
				if( iRet == 0 ){
					strErrMsg = "Generate Kmc Key succ!";

					/* get token from SSM Response */
					string strToken = "";

					memset( caUnPackStr, 0, sizeof(caUnPackStr) );
					DataFmtConv::Unpack( caBNTngCmd_rcv, caUnPackStr, iBNTngCmdLen_rcv );
					strToken = KMCReq::FF09_GenKey_resp( string( (char *)caUnPackStr) );
					if(( strToken.length() != 79*2 ) && ( strToken.length() != 71*2 ))
					{
						strErrMsg = "Token Fmt Error!";
						iRet = -1;
						break;
					}
					KeyInfo.SetKeyToken( strToken );

					keyToken.CompTokenFromStr( strToken );
					KeyInfo.SetKeyCheckValue( keyToken.GetKCV() );

					iRet = WriteKeyIntoFile(KeyInfo);;
				}
				break;
			case 'o':
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYPARTMODE_LABELSTR, " 1-Two XOR components 2-Three XOR components 3-Clear component" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyPartMode!";
					break;
				}

				if( KeyInfo.GetKeyToken().length() < 32 ){
					iRet = -1;
					strErrMsg = "Token not generated!";
					break;
				}
				iRet = GenCmd( KMC_CMD_TYPE_OUTPUTKEYSTR );
				if( iRet != 0 ){
					break;
				}

				iRet = SendCmdToSSM();
				if( iRet == 0 ){
					iRet = DisplayKeyPartsStr();
				}
				break;
			case 'r':
				return;
				break;
			default:
				iRet = -1;
				strErrMsg = "Invalid Choice!";
				break;
		}

		if( iRet != 0 ){
			cout<<strErrMsg<<endl;
			getchar();
		}

	}

	return;
}

//------------------------------------------------------------------
// Function   : DisplayMenu_InputKmcKey
// Description: Display choice menu for InputKmcKey control
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::DisplayMenu_InputKmcKey()
{
	system("clear");
	cout<<"********************************************************************************"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**                              2. Input Key                                  **"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**     1. KeyType                       Input Value:"<<KeyInfo.GetKeyType();	cout.width((28-KeyInfo.GetKeyType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     2. SubType                       Input Value:"<<KeyInfo.GetSubType();	cout.width((28-KeyInfo.GetSubType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     3. KeyVersion                    Input Value:"<<KeyInfo.GetKeyVersion();	cout.width((28-KeyInfo.GetKeyVersion().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     4. KeyPartMode                   Input Value:"<<KeyInfo.GetKeyPartsMode();	cout.width((28-KeyInfo.GetKeyPartsMode().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     5. KeyPartValue                                                        **"<<endl;
	if( atoi(KeyInfo.GetKeyPartsMode().c_str()) == 1 ){
		cout<<"**        Input Part 1"<<":"<<KeyInfo.GetKeyParts(1);	cout.width((57-KeyInfo.GetKeyParts(1).length()));
		cout<<"**"<<endl;	cout.width();
		cout<<"**        Input Part 2"<<":"<<KeyInfo.GetKeyParts(2);	cout.width((57-KeyInfo.GetKeyParts(2).length()));
		cout<<"**"<<endl;	cout.width();
	}
	else if( atoi(KeyInfo.GetKeyPartsMode().c_str()) == 2 ){
		cout<<"**        Input Part 1"<<":"<<KeyInfo.GetKeyParts(1);	cout.width((57-KeyInfo.GetKeyParts(1).length()));
		cout<<"**"<<endl;	cout.width();
		cout<<"**        Input Part 2"<<":"<<KeyInfo.GetKeyParts(2);	cout.width((57-KeyInfo.GetKeyParts(2).length()));
		cout<<"**"<<endl;	cout.width();
		cout<<"**        Input Part 3"<<":"<<KeyInfo.GetKeyParts(3);	cout.width((57-KeyInfo.GetKeyParts(3).length()));
		cout<<"**"<<endl;	cout.width();
	}
	cout<<"**     6. Description                   Input Value:";
	if( KeyInfo.GetDesc().length() <= 26 ){
		cout<<KeyInfo.GetDesc();
		cout.width((28-KeyInfo.GetDesc().length()));
		cout<<"**"<<endl;	cout.width();
	}
	else{
		cout<<KeyInfo.GetDesc().substr(0,26); cout<<"**"<<endl;
		cout<<"**                                      ";
		cout<<KeyInfo.GetDesc().substr(26,KeyInfo.GetDesc().length()-26);
		cout.width((40+26-KeyInfo.GetDesc().length()));
		cout<<"**"<<endl;
	}
	cout<<"**     7. Check Value                   Input Value:"<<KeyInfo.GetKeyCheckValue();	cout.width((28-KeyInfo.GetKeyCheckValue().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**     r. Return                        i. Input Key                          **"<<endl;
	cout<<"********************************************************************************"<<endl;
}

//------------------------------------------------------------------
// Function   : InputKmcKey
// Description: Input KMC Key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::InputKmcKey()
{
	int    iRet=0;
	unsigned char caUnPackStr[1024+1];
	string inputStr;
	KeyInfo.ResetValue();

	while(1){
		iRet = 0;
		strErrMsg = "";
		DisplayMenu_InputKmcKey();
		cout<<"Please Input your Choice:"<<endl;
		cin>>inputStr;
		//ignore \n in cin input stream
		cin.ignore();

		if( inputStr.length() > 1 ){
			cout<<"Invalid Choice!"<<endl;
			getchar();
			continue;
		}

		switch (inputStr.c_str()[0]) {
			case '1':	//KeyType
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYTYPE_LABEL, "00-FF" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyType!";
				}
				break;
			case '2':	//SubType
				iRet = KeyInfo.SetStrFromScrInput( KMC_SUBTYPE_LABEL, strSubTypeRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid SubType!";
				}
				break;
			case '3':	//KeyVersion
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYVERSION_LABEL, strKeyVerRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyVersion!";
				}
				break;
			case '4':	//KeyPartMode
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYPARTMODE_LABELSTR, " 1-Two XOR components 2-Three XOR components" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyPartMode!";
				}
				break;
			case '5':	//KeyPartStr
				if( atoi(KeyInfo.GetKeyPartsMode().c_str()) == 1 ){
					iRet = KeyInfo.SetStrFromScrInput( KMC_KEYPART_1_LABELSTR, "" );
					iRet += KeyInfo.SetStrFromScrInput( KMC_KEYPART_2_LABELSTR, "" );
				}
				else if( atoi(KeyInfo.GetKeyPartsMode().c_str()) == 2 ){
					iRet = KeyInfo.SetStrFromScrInput( KMC_KEYPART_1_LABELSTR, "" );
					iRet += KeyInfo.SetStrFromScrInput( KMC_KEYPART_2_LABELSTR, "" );
					iRet += KeyInfo.SetStrFromScrInput( KMC_KEYPART_3_LABELSTR, "" );
				}
				else{
					iRet = -1;
					strErrMsg = "Please Input KeyPartMode(Choice 4) first!";
					break;
				}
				
				if( iRet == -1 ){
					strErrMsg = "Key Part String 1 is Invalid!";
				}
				else if( iRet == -3 ){
					strErrMsg = "Key Part String 1 and 2 are Invalid!";
				}
				else if( iRet == -7 ){
					strErrMsg = "Key Part String 1,2,3 are Invalid!";
				}
				
				break;
			case '6':	//Description
				KeyInfo.SetStrFromScrInput( KMC_DESC_LABELSTR, "" );
				break;
			case '7':	//Key Check Value
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYCHKVAL_LABELSTR, "Input length should equal 6" );
				if( iRet != 0 ){
					strErrMsg = "Invalid Key Check Value!";
				}
				break;
			case 'i':
				iRet = GenCmd( KMC_CMD_TYPE_INPUTKEY );
				if( iRet != 0 ){
					break;
				}

				iRet = SendCmdToSSM();
				if( iRet == 0 ){
					/* get token from SSM Response */
					string strToken = "";

					memset( caUnPackStr, 0, sizeof(caUnPackStr) );
					DataFmtConv::Unpack( caBNTngCmd_rcv, caUnPackStr, iBNTngCmdLen_rcv );
					strToken = KMCReq::FF33_InputKey_resp( string( (char *)caUnPackStr) );
					if( strToken.length() != 79*2 ) {
						strErrMsg = "Token Fmt Error!";
						iRet = -1;
						break;
					}
					
					KeyInfo.SetKeyToken( strToken );
					keyToken.CompTokenFromStr( strToken );
					if( KeyInfo.GetKeyCheckValue() != keyToken.GetKCV() ){
						strErrMsg = "Check value is not correct!";
						iRet = -1;
						break;
					}
					KeyInfo.SetKeyCheckValue( keyToken.GetKCV() );
					
					strErrMsg = "Input Kmc Key succ,Check Value is : "+keyToken.GetKCV();
					iRet = WriteKeyIntoFile(KeyInfo);
				}
				break;
			case 'r':
				return;
				break;
			default:
				iRet = -1;
				strErrMsg = "Invalid Choice!";
				break;
		}

		if( iRet != 0 ){
			cout<<strErrMsg<<endl;
			getchar();
		}
	}

	return;
}

//------------------------------------------------------------------
// Function   : DisplayMenu_ImportKey
// Description: Display choice menu for ImportKey control
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::DisplayMenu_ImportKey()
{
	system("clear");
	cout<<"********************************************************************************"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**                             3. Import Key File                             **"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**     1. KeyType(key encryption key)   Input Value:"<<KeyInfo.GetKeyType();	cout.width((28-KeyInfo.GetKeyType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     2. SubType(key encryption key)   Input Value:"<<KeyInfo.GetSubType();	cout.width((28-KeyInfo.GetSubType().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     3. KeyVer(key encryption key)    Input Value:"<<KeyInfo.GetKeyVersion();	cout.width((28-KeyInfo.GetKeyVersion().length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**     4. Key File Name                                                       **"<<endl;
	cout<<"**     FilePath:"<<strInputFileName;	cout.width((64-strInputFileName.length()));
	cout<<"**"<<endl;	cout.width();
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**                                                                            **"<<endl;
	cout<<"**     r. Return                        i. Import Key File                    **"<<endl;
	cout<<"********************************************************************************"<<endl;
}

//------------------------------------------------------------------
// Function   : ImportKeyFile
// Description: Import Key File
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::ImportKeyFile()
{
	int    iRet=0;
	string inputStr;
	string tmpStr;
	KeyInfo.ResetValue();
	strInputFileName = "";
	while(1){
		iRet = 0;
		strErrMsg = "";
		DisplayMenu_ImportKey();
		cout<<"Please Input your Choice:"<<endl;
		cin>>inputStr;
		//ignore \n in cin input stream
		cin.ignore();

		if( inputStr.length() > 1 ){
			cout<<"Invalid Choice!"<<endl;
			getchar();
			continue;
		}

		switch (inputStr.c_str()[0]) {
			case '1':	//KeyType
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYTYPE_LABEL, "00-FF" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyType!";
				}
				break;
			case '2':	//SubType
				iRet = KeyInfo.SetStrFromScrInput( KMC_SUBTYPE_LABEL, strSubTypeRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid SubType!";
				}
				break;
			case '3':	//KeyVersion
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYVERSION_LABEL, strKeyVerRange );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyVersion!";
				}
				break;
			case '4':	//Import File Path
				tmpStr = KeyInfo.SetStrFromScrInputNoChk( "Import File Path", "absolute dir like /home/monedia/runtime/kmcfiles/filename" );
				if( tmpStr.length() < 3 ){
					strErrMsg = "Invalid Import File Path!";
					iRet = 1;
					break;
				}
				strInputFileName = tmpStr;
				break;
			case 'i':
				iRet = KeyInfo.Check();
				if( iRet != 0 ){
					strErrMsg = "Check Encryption KeyInfo Error!";
					break;
				}
				
				iRet = ImpDESKeyFileData();
				if( iRet != 0 ){
					break;
				}
				iRet = 1;
				break;
			case 'r':
				return;
				break;
			default:
				iRet = -1;
				strErrMsg = "Invalid Choice!";
				break;
		}

		if( iRet != 0 ){
			cout<<strErrMsg<<endl;
			getchar();
		}
	}

	return;
}

//------------------------------------------------------------------
// Function   : DisplayMenu_QueryKey
// Description: Display choice menu for QueryKey control
// Input      : int iTolCnt		-- Total record counts
//            : kmcKeyInfo *pKeyInfo[]  -- array of KeyInfo
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::DisplayMenu_QueryKey( int iTolCnt, kmcKeyInfo *pKeyInfo )
{
	int   iOffSet = 0;
	int   i;
	char  caTolCnt[4+1];

	memset( caTolCnt, 0, sizeof(caTolCnt) );
	sprintf( caTolCnt, "%d", iTolCnt );
	iOffSet = strlen(caTolCnt);

	system("clear");
	cout<<"********************************************************************************"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**                           4. Query and OutPut Key                          **"<<endl;
	cout<<"**----------------------------------------------------------------------------**"<<endl;
	cout<<"**  No  KeyType  SubType  KeyVer  ChkValue  Description                       **"<<endl;
	for( i=0; i<iTolCnt; i++ ){
		cout<<"**  ";	cout.width(2); cout<<i+1;
		cout<<"  ";	cout.width(7); cout<<pKeyInfo[i].GetKeyType();
		cout<<"  ";	cout.width(7); cout<<pKeyInfo[i].GetSubType();
		cout<<"  ";	cout.width(6); cout<<pKeyInfo[i].GetKeyVersion();
		cout<<"  ";	cout.width(8); cout<<pKeyInfo[i].GetKeyCheckValue();
		cout<<"  ";	cout<<pKeyInfo[i].GetDesc(); cout.width(36-pKeyInfo[i].GetDesc().length());
		cout<<"**"<<endl;	cout.width();
	}
	cout<<"**                                                                            **"<<endl;
	cout<<"**  Total Count:"; cout<<iTolCnt;  cout.width(64-iOffSet);
	cout<<"**"<<endl;	cout.width();
	cout<<"**                                                                            **"<<endl;
	cout<<"**     r. Return              q. Query Key           d. Delete Key            **"<<endl;
	cout<<"**     o. Output Key                                 g. Generate Key File     **"<<endl;
	cout<<"********************************************************************************"<<endl;
}

//------------------------------------------------------------------
// Function   : QueryKey
// Description: Query Key
// Input      : none
// Output     : none
// Return     : none
//------------------------------------------------------------------
void kmcUI::QueryKey()
{
	int    iRet=0;
	int    iCmdType=1;
	int 	 keyIdx = 0;
	string inputStr;
	string lineNo;
	string strFileName;
	string KSKFileName;
	string KDKFileName;
	KeyInfo.ResetValue();
	int    iTolCnt=0;
	kmcKeyInfo *pKeyInfo = new kmcKeyInfo[KMC_KEYINFO_MAXITEM];

	while(1){
		iRet = 0;
		DisplayMenu_QueryKey( iTolCnt, pKeyInfo );
		cout<<"Please Input your Choice:"<<endl;
		cin>>inputStr;
		//ignore \n in cin input stream
		cin.ignore();

		if( inputStr.length() > 1 ){
			cout<<"Invalid Choice!"<<endl;
			getchar();
			continue;
		}

		switch (inputStr.c_str()[0]) {
			case 'q':
				iTolCnt = QueryKeyFromFile( pKeyInfo );
				if( iTolCnt < 0 ){
					iRet = -1;
					iTolCnt = 0;
					strErrMsg = "Read from file error!";
				}
				break;
			case 'd':
				lineNo = KeyInfo.SetStrFromScrInputNoChk( "No of the Key", "" );
				if( atoi(lineNo.c_str()) <= iTolCnt && atoi(lineNo.c_str()) > 0 
					&& lineNo.length() > 0 ){
					iRet = DelKeyFromFile( lineNo );
					iTolCnt = QueryKeyFromFile( pKeyInfo );
				}
				else{
					iRet = -1;
					strErrMsg = "Invalid No of Key!";
				}
				break;
			case 'o':
				lineNo = KeyInfo.SetStrFromScrInputNoChk( "No of the Key", "" );
				if( lineNo.length() == 0 ){
					iRet = -1;
					strErrMsg = "Invalid No of Key!";
					break;
				}
					
				keyIdx= atoi(lineNo.c_str());

				if( keyIdx > iTolCnt ||
						keyIdx < 0 ||
						DataFmtCheck::CheckIntStr( lineNo.c_str(), lineNo.length() ) != 0 )
				{
					iRet = -1;
					strErrMsg = "Invalid No of Key!";
					break;
				}
				KeyInfo = pKeyInfo[keyIdx-1];
				iRet = KeyInfo.SetStrFromScrInput( KMC_KEYPARTMODE_LABELSTR, " 1-Two XOR components 2-Three XOR components 3-Clear component" );
				if( iRet != 0 ){
					strErrMsg = "Invalid KeyPartMode!";
					break;
				}

				iRet = GenCmd( KMC_CMD_TYPE_OUTPUTKEYSTR );
				if( iRet != 0 ){
					break;
				}

				iRet = SendCmdToSSM();
				if( iRet == 0 ){
					iRet = DisplayKeyPartsStr();
				}
				break;
			case 'g':
				strFileName = KeyInfo.SetStrFromScrInputNoChk( "FileName", "" );
				if( strFileName.length() <= 0 ){
					iRet = -1;
					strErrMsg = "Invalid FileName!";
					break;
				}

				//generate KDK File
				{
					iRet = FindKeyFromFile( MD_SEC_BNT_TK_KDK, strSubType_DEFAULT, "0", &KeyInfo );
					if( iRet != 0 ){
						break;
					}

					//generate KDK File
					iRet = GenCmd( KMC_CMD_TYPE_GENKEYFILE_KDK );
					if( iRet != 0 ){
						break;
					}

					iRet = SendCmdToSSM();
					if( iRet != 0 ){
						break;
					}

					KDKFileName = strFileName;
					if( KDKFileName.find( ".kdk", 0 ) == string::npos ){
						KDKFileName.append( ".kdk" );
					}
					KDKFileName = strOutputFilePath+KDKFileName;
					iRet = GenKeyFile_KDK( KDKFileName );
					if( iRet != 0 ){
						break;
					}
					iRet = 0;
					cout<<"Generate KDK File succ,FilePath:"+KDKFileName<<endl;
				}

				//generate KSK File
				{
					KSKFileName = strFileName;
					if( KSKFileName.find( ".ksk", 0 ) == string::npos ){
						KSKFileName.append( ".ksk" );
					}
					KSKFileName = strOutputFilePath+KSKFileName;
					iRet = GenKeyFile_KSK( KSKFileName );
					if( iRet != 0 ){
						break;
					}
					iRet = 0;
					cout<<"Generate KSK File succ,FilePath:"+KSKFileName<<endl;
					getchar();
				}

				break;
			case 'r':
				delete []pKeyInfo;
				return;
				break;
			default:
				iRet = -1;
				strErrMsg = "Invalid Choice!";
				break;
		}

		if( iRet != 0 ){
			cout<<strErrMsg<<endl;
			getchar();
		}
	}

	delete []pKeyInfo;
	return;
}

//------------------------------------------------------------------
// Function   : SendCmdToSSM
// Description: Send BNTng Command to SSM
// Input      :
// Output     :
// Return     : 0  success
//							-1 fail
//------------------------------------------------------------------
int kmcUI::SendCmdToSSM( )
{
	int iRet;

	if( iBNTngCmdLen_snd<= 0 ){
		strErrMsg = "Send Msg Length invalid!";
		return -1;
	}

	errLog.ErrLog( LOG_LEVEL_DEBUG, "Send Cmd Req to SSM", 0, caBNTngCmd_snd, iBNTngCmdLen_snd );

	iRet = Comm_Cli.write( caBNTngCmd_snd, iBNTngCmdLen_snd );
	if( iRet != 0 ){
		strErrMsg = "Send Msg to SSM Server error!";
		return -1;
	}

	memset( caBNTngCmd_rcv, 0, sizeof(caBNTngCmd_rcv) );
	iRet = Comm_Cli.read( caBNTngCmd_rcv, &iBNTngCmdLen_rcv, KMC_SEND_TIMEOUT );
	if( iRet != 0 ){
		strErrMsg = "Receive Msg From SSM Server error!";
		return -1;
	}

	errLog.ErrLog( LOG_LEVEL_DEBUG, "Get Cmd Rsp from SSM", 0, caBNTngCmd_rcv, iBNTngCmdLen_rcv );

	return 0;
}

//------------------------------------------------------------------
// Function   : GenCmd
// Description: Generate BNTng Command
// Input      : iCmdType
//              0 -- E9 generate Key
//              1 --    generate Input Key
//              2 --    import key
//              3 --    generate key file
// Output     :
// Return     : 0  success
//							-1 fail
//------------------------------------------------------------------
int kmcUI::GenCmd( int iCmdType )
{
	int iRet=0;
	string tmpStr="";
	string keyId="";
	string stokeyId="";
	string strsendCmd="";
	string encKeyToken="";

	memset( caBNTngCmd_snd, 0, sizeof(caBNTngCmd_snd) );
	switch( iCmdType ){
		case KMC_CMD_TYPE_GENKEY:
		{
			iRet = KeyInfo.Check();
			if( iRet != 0 ){
				strErrMsg = "Check KeyInfo Error!";
				return iRet;
			}
			
			iRet = CheckExistOfKeyInfo( &KeyInfo );
			if( iRet != 0 ){
				iRet = -1;
				return iRet;
			}

			/* keyId */
			keyId = BNTFmt::GenTagStr_D2( strNKComponentType, KeyInfo.GetKeyType(), KeyInfo.GetSubType(), KeyInfo.GetKeyVersion() );
			/* stokeyId */
			stokeyId = BNTFmt::GenTagStr_D2( strNKComponentType, MD_SEC_BNT_TK_KDKM, strSubType_DEFAULT, "0" );

			strsendCmd = KMCReq::E9_GenKey_cmd( keyId, stokeyId );

			break;
		}
		case KMC_CMD_TYPE_INPUTKEY:
		{
			iRet = KeyInfo.Check();
			if( iRet != 0 ){
				strErrMsg = "Check KeyInfo Error!";
				return iRet;
			}
			
			iRet = CheckExistOfKeyInfo( &KeyInfo );
			if( iRet != 0 ){
				iRet = -1;
				return iRet;
			}
			
			/* keyId */
			keyId = BNTFmt::GenTagStr_D2( strNKComponentType, KeyInfo.GetKeyType(), KeyInfo.GetSubType(), KeyInfo.GetKeyVersion() );
			/* stokeyId */
			stokeyId = BNTFmt::GenTagStr_D2( strNKComponentType, MD_SEC_BNT_TK_KDKM, strSubType_DEFAULT, "0" );
				
			string kevParts="";
			tmpStr = "00000000000000000000000000000000";
			keyTokenTLV *ptoKen_D5 = new keyTokenTLV( "D5", 1, tmpStr );
			if( KeyInfo.GetKeyPartsMode() == "1" ){
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, KeyInfo.GetKeyParts(1) );
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, KeyInfo.GetKeyParts(2) );
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, tmpStr );
			}
			else if( KeyInfo.GetKeyPartsMode() == "2" ){
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, KeyInfo.GetKeyParts(1) );
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, KeyInfo.GetKeyParts(2) );
				kevParts += ptoKen_D5->GenTLVStr( "D5", 1, KeyInfo.GetKeyParts(3) );
			}
			else{
				iRet = -1;
				strErrMsg = "Invalid KeyPartMode!";
				delete ptoKen_D5;
				return iRet;
			}

			strsendCmd = KMCReq::FF23_KeyInput_cmd( keyId, stokeyId, KeyInfo.GetKeyPartsMode(), kevParts );

			delete ptoKen_D5;
			break;
		}
		case KMC_CMD_TYPE_GENKEYFILE_KDK:
			encKeyToken = keyToken.CompToken( MD_SEC_BNT_TK_KM2BNTX, "00", "00", "00", "00000000000000000000000000000000", "000000" );
			strsendCmd = KMCReq::FF21_TransToken_cmd( KeyInfo.GetKeyToken(), encKeyToken );
			break;
		case KMC_CMD_TYPE_OUTPUTKEYSTR:
			strsendCmd = KMCReq::FF22_OutPKey_cmd( KeyInfo.GetKeyToken(), KeyInfo.GetKeyPartsMode() );
			break;
		default:
			iRet = -1;
			strErrMsg = "Invalid Cmd Type!";
			break;
	}

	iBNTngCmdLen_snd = DataFmtConv::Pack( (unsigned char *)strsendCmd.c_str(), caBNTngCmd_snd+KMC_SENDBUFLEN_LEN, strsendCmd.length() );
	DataFmtConv::long_tab( caBNTngCmd_snd, KMC_SENDBUFLEN_LEN, &iBNTngCmdLen_snd );
	iBNTngCmdLen_snd += KMC_SENDBUFLEN_LEN;
	//iBNTngCmdLen_snd = DataFmtConv::Pack( (unsigned char *)strsendCmd.c_str(), caBNTngCmd_snd, strsendCmd.length() );

	errLog.ErrLog( LOG_LEVEL_DEBUG, "SendCmd in Hex", 0, caBNTngCmd_snd, iBNTngCmdLen_snd );

	return iRet;
}

//------------------------------------------------------------------
// Function   : QueryKeyFromFile
// Description: Query Kmc Key data from KMC key data file
// Input      :
// Output     : kmcKeyInfo *pKeyInfo
// Return     : >=0 Record counts
//							-1 fail
//------------------------------------------------------------------
int kmcUI::QueryKeyFromFile( kmcKeyInfo *pKeyInfo )
{
	int iCnt = 0,iRet = 0;
	kmcKeyInfo *pKeyInfo_1 = new kmcKeyInfo;
	kmcFileOp *pFileOpHandle = new kmcFileOp( strKmcKeyFileName, "R" );

	iRet = pFileOpHandle->OpenFile();
	if( iRet != 0 ){
		delete pKeyInfo_1;
		delete pFileOpHandle;
		return iRet;
	}

	while(1){
		iRet = pFileOpHandle->ReadKmcKeyDataFile( pKeyInfo_1 );
		if( iRet < 0 ){
			iCnt = -1;
			break;
		}
		else if( iRet == 1 ){
			pKeyInfo[iCnt] = *pKeyInfo_1;
			iCnt++;
			break;
		}
		else if( iRet == 2 ){
			break;
		}

		pKeyInfo[iCnt] = *pKeyInfo_1;
		iCnt++;

		/* only query count <= KMC_KEYINFO_MAXITEM */
		if( iCnt >= KMC_KEYINFO_MAXITEM ){
			break;
		}
	}

	pFileOpHandle->Close();
	delete pKeyInfo_1;
	delete pFileOpHandle;
	return iCnt;
}

int kmcUI::DelKeyFromFile( string lineNo )
{
	int iLineNo = 0,iCurrLine;
	int iRet;
	int iStrLen,iOffSet;
	char caTmpStr[1024+1];
	char caFileStr[10240*2+1];
	fstream	hInFile;
	fstream	hOutFile;

	hInFile.open( strKmcKeyFileName.c_str(), ios::in );

	/* read all data except special line */
	iLineNo = atoi(lineNo.c_str());
	iStrLen = 0;
	iOffSet = 0;
	iCurrLine = 0;
	memset( caFileStr, 0, sizeof(caFileStr) );
	while(1){
		memset( caTmpStr, 0, sizeof(caTmpStr) );
		hInFile.getline( caTmpStr, 1024 );
		if( hInFile.bad() )
		{
			iRet = -1;
			strErrMsg = "Read File Error!";
			break;
		}
		if( iCurrLine == iLineNo - 1){
			iLineNo = 0;
			continue;
		}
		iStrLen = hInFile.gcount();
		memcpy( caFileStr+iOffSet, caTmpStr, iStrLen );
		iOffSet += iStrLen;
		if( caFileStr[iOffSet-1] == 0x00 ){
			caFileStr[iOffSet-1] = '\n';
		}
/*
		else if( caFileStr[iOffSet-1] == '\n' ){
			caFileStr[iOffSet-1] = '|';
			caFileStr[iOffSet] = '\n';
			iOffSet += 1;
		}
*/
		iCurrLine++;

		if( hInFile.eof() ){
			iRet = 1;
			break;
		}
	}

	hInFile.close();

	if( iRet<0 ){
		return iRet;
	}

	hOutFile.open( strKmcKeyFileName.c_str(), ios::out );
	hOutFile.write( caFileStr, iOffSet );

	hOutFile.close();
	strErrMsg = "Delete key succ!";
	return 1;
}

//------------------------------------------------------------------
// Function   : WriteKeyIntoFile
// Description: Write Kmc Key data into KMC key data file
// Input      : inpKeyInfo
// Output     :
// Return     : 1 success
//							-1 fail
//------------------------------------------------------------------
int kmcUI::WriteKeyIntoFile( kmcKeyInfo inpKeyInfo )
{
	int iRet=-1,iBytes;
	unsigned char caTmpStr[1024+1];
	fstream	hWriteFile;

	iRet = inpKeyInfo.Check();
	if( iRet != 0 ){
		strErrMsg = "Check KeyInfo Error!";
		return iRet;
	}

	hWriteFile.open( strKmcKeyFileName.c_str(), ios::out|ios::app );
	if( hWriteFile.bad() ){
		strErrMsg = "Open File for write error!";
		return iRet;
	}
	memset( caTmpStr, 0, sizeof(caTmpStr) );

  /*
	 * Line Fmt:KeyType|SubType|KeyVer|ChkValue|Desc|KeyTokey|
	*/
  iBytes = sprintf( (char *)caTmpStr, "%s|%s|%s|%s|%s|%s|\n",
  									 inpKeyInfo.GetKeyType().c_str(),
  									 inpKeyInfo.GetSubType().c_str(),
  									 inpKeyInfo.GetKeyVersion().c_str(),
  									 inpKeyInfo.GetKeyCheckValue().c_str(),
  									 inpKeyInfo.GetDesc().c_str(),
  									 inpKeyInfo.GetKeyToken().c_str() );
	if( iBytes >= 12 ){
		hWriteFile.write( (char *)caTmpStr, iBytes );
		iRet = 1;
	}
	else{
		strErrMsg = "Error Format of Key Data!";
		iRet = -1;
	}

	if( hWriteFile.bad() ){
		strErrMsg = "Write file error!";
		iRet = -1;
	}

	hWriteFile.close();
	return iRet;
}

int kmcUI::DisplayKeyPartsStr()
{
	unsigned char caUnPackStr[1024+1];
	int iKeyLenth = 0;

	memset( caUnPackStr, 0, sizeof(caUnPackStr) );
	DataFmtConv::Unpack( caBNTngCmd_rcv, caUnPackStr, iBNTngCmdLen_rcv );
	string strRspMsg = (char *)caUnPackStr;

	errLog.ErrLog( LOG_LEVEL_DEBUG, "strRspMsg in Str", 0, (unsigned char *)strRspMsg.c_str(), strRspMsg.length() );
	
	int pos1 = strRspMsg.find( "C1" );
	if( pos1<0 ){
		strErrMsg = "Error Key Parts Format!";
		return -1;
	}

/*add by guo jy in 2010/01/27 */
	string strMsgLen = strRspMsg.substr( pos1+2, 2 );
	if( (strMsgLen == "17") || (strMsgLen == "22") || (strMsgLen == "0C"))
	{
		iKeyLenth = 16;
	}
	else
	{
		iKeyLenth = 32;
	}
	string outPutKey[3];
	string outPutKey_ChkValue[3];
	int iOffSet = 2+2+2;

	/* display the key parts */
	if( KeyInfo.GetKeyPartsMode() == "1" ){
		outPutKey[0] = strRspMsg.substr( pos1+iOffSet, iKeyLenth);
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[0] = strRspMsg.substr( pos1+iOffSet, 6 );
		iOffSet += 6;

		outPutKey[1] = strRspMsg.substr( pos1+iOffSet, iKeyLenth );
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[1] = strRspMsg.substr( pos1+iOffSet, 6 );
		iOffSet += 6;
		cout<<"Key Desc:"<<KeyInfo.GetDesc()<<endl;
		cout<<"Check Value For Key:"<<KeyInfo.GetKeyCheckValue()<<endl;
		cout<<"Key Part 1:"<<outPutKey[0]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[0]<<endl;
		cout<<"Key Part 2:"<<outPutKey[1]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[1]<<endl;
	}
	else if( KeyInfo.GetKeyPartsMode() == "2" ){
		outPutKey[0] = strRspMsg.substr( pos1+iOffSet, iKeyLenth );
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[0] = strRspMsg.substr( pos1+iOffSet, 6 );
		iOffSet += 6;

		outPutKey[1] = strRspMsg.substr( pos1+iOffSet, iKeyLenth );
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[1] = strRspMsg.substr( pos1+iOffSet, 6 );
		iOffSet += 6;

		outPutKey[2] = strRspMsg.substr( pos1+iOffSet, iKeyLenth );
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[2] = strRspMsg.substr( pos1+iOffSet, 6 );
		iOffSet += 6;

		cout<<"Key Desc:"<<KeyInfo.GetDesc()<<endl;
		cout<<"Check Value For Key:"<<KeyInfo.GetKeyCheckValue()<<endl;
		cout<<"Key Part 1:"<<outPutKey[0]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[0]<<endl;
		cout<<"Key Part 2:"<<outPutKey[1]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[1]<<endl;
		cout<<"Key Part 3:"<<outPutKey[2]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[2]<<endl;
	}
	else if( KeyInfo.GetKeyPartsMode() == "3" ){
		outPutKey[0] = strRspMsg.substr( pos1+iOffSet, iKeyLenth );
		iOffSet += iKeyLenth;
		outPutKey_ChkValue[0] = strRspMsg.substr( pos1+iOffSet, 6 );
		
		cout<<"Key Desc:"<<KeyInfo.GetDesc()<<endl;
		cout<<"Check Value For Key:"<<KeyInfo.GetKeyCheckValue()<<endl;
		cout<<"Clear component:"<<outPutKey[0]<<" ";
		cout<<"Check Value:"<<outPutKey_ChkValue[0]<<endl;
	}
	else{
		strErrMsg = "Error Key Parts Mode!";
		return -1;
	}
	strErrMsg = "Output key succ!";

	return 1;
}

//------------------------------------------------------------------
// Function   : CheckExistOfKeyInfo
// Description: check the existence of KeyInfo
// Input      : kmcKeyInfo
// Output     :
// Return     :  0 No Existence of the KeyInfo
//               1 KeyInfo Exist
//							-1 fail
//------------------------------------------------------------------
int kmcUI::CheckExistOfKeyInfo( kmcKeyInfo *pKeyInfo )
{
	int iRet = 0;
	kmcKeyInfo *pKeyInfo_1 = new kmcKeyInfo;
	kmcFileOp *pFileOpHandle = new kmcFileOp( strKmcKeyFileName, "R" );

	iRet = pFileOpHandle->OpenFile();
	if( iRet != 0 ){
		delete pKeyInfo_1;
		delete pFileOpHandle;
		return iRet;
	}

	while(1){
		iRet = pFileOpHandle->ReadKmcKeyDataFile( pKeyInfo_1 );
		if( iRet < 0 ){
			strErrMsg = "Read Kmc Key file error!";
			break;
		}
		else if( iRet == 2 ){   //read file over
			iRet = 0;
			break;
		}

		if( pKeyInfo->GetKeyType() == pKeyInfo_1->GetKeyType() &&
			  pKeyInfo->GetSubType() == pKeyInfo_1->GetSubType() &&
			  pKeyInfo->GetKeyVersion() == pKeyInfo_1->GetKeyVersion() ){
			iRet = 1;
			strErrMsg = "KeyInfo has existed!";
			break;
		}
	}

	pFileOpHandle->Close();
	delete pKeyInfo_1;
	delete pFileOpHandle;
	return iRet;
}
/*
//------------------------------------------------------------------
// Function   : CheckExistOfKeyInfo
// Description: check the existence of KeyInfo
// Input      : strTag_D2
// Output     :
// Return     :  0 No Existence of the KeyInfo
//               1 KeyInfo Exist
//							-1 fail
//------------------------------------------------------------------
*/
int kmcUI::CheckExistOfKeyInfo( string strTag_D2 )
{
	int iRet = 0;
	string keyType;
	string subType;
	string keyVersion;
	kmcKeyInfo *pKeyInfo_1 = new kmcKeyInfo;
	kmcFileOp *pFileOpHandle = new kmcFileOp( strKmcKeyFileName, "R" );

	iRet = pFileOpHandle->OpenFile();
	if( iRet != 0 ){
		delete pKeyInfo_1;
		delete pFileOpHandle;
		return iRet;
	}

	//D213KT000000KNKV
	keyType = strTag_D2.substr( 0+4, 2 );
	subType = strTag_D2.substr( strTag_D2.length()-2, 1 );
	keyVersion = strTag_D2.substr( strTag_D2.length()-1, 1 );
	
	while(1){
		iRet = pFileOpHandle->ReadKmcKeyDataFile( pKeyInfo_1 );
		if( iRet < 0 ){
			strErrMsg = "Read Kmc Key file error!";
			break;
		}
		else if( iRet == 2 ){   //read file over
			iRet = 0;
			break;
		}

		if( keyType == pKeyInfo_1->GetKeyType() &&
			  subType == pKeyInfo_1->GetSubType() &&
			  keyVersion == pKeyInfo_1->GetKeyVersion() ){
			iRet = 1;
			strErrMsg = "KeyInfo has existed!";
			break;
		}
	}

	pFileOpHandle->Close();
	delete pKeyInfo_1;
	delete pFileOpHandle;
	return iRet;
}

//------------------------------------------------------------------
// Function   : FindKeyFromFile
// Description: Find KeyInfo From kmc key file
// Input      : keyType
//              subType
//              keyVersion
// Output     : kmcKeyInfo
// Return     :  0 succ
//							-1 fail
//------------------------------------------------------------------
int kmcUI::FindKeyFromFile( string keyType, string subType, string keyVersion, kmcKeyInfo *pKeyInfo )
{
	int iRet = -1;
	kmcFileOp *pFileOpHandle = new kmcFileOp( strKmcKeyFileName, "R" );

	iRet = pFileOpHandle->OpenFile();
	if( iRet != 0 ){
		delete pFileOpHandle;
		return iRet;
	}

	while(1){
		iRet = pFileOpHandle->ReadKmcKeyDataFile( pKeyInfo );
		if( iRet < 0 ){
			strErrMsg = "Read Kmc Key file error!";
			break;
		}
		else if( iRet == 2 ){   //read file over
			strErrMsg = "No data found!";
			iRet = -1;
			break;
		}

		if( keyType == pKeyInfo->GetKeyType() &&
			  subType == pKeyInfo->GetSubType() &&
			  keyVersion == pKeyInfo->GetKeyVersion() ){
			iRet = 0;
			break;
		}
		pKeyInfo->ResetValue();
	}

	pFileOpHandle->Close();
	delete pFileOpHandle;
	return iRet;
}

//------------------------------------------------------------------
// Function   : GenKeyFile_KDK
// Description: generate KDK key file
// Input      : filename -- KDK file name
// Output     :
// Return     :  0 succ
//							-1 fail
//------------------------------------------------------------------
int kmcUI::GenKeyFile_KDK( string fileName )
{
	int iRet = -1;
	fstream	hWriteFile;
	string  strKDKFileData;
	unsigned char caTmpStr[1024+1];

	memset( caTmpStr, 0, sizeof(caTmpStr) );
	DataFmtConv::Unpack( caBNTngCmd_rcv, caTmpStr, iBNTngCmdLen_rcv );
	string strRspMsg = (char *)caTmpStr;
	int pos1 = strRspMsg.find( "E1" );
	if( pos1<0 ){
		strErrMsg = "Error TransFormat of Token!";
		return -1;
	}
	errLog.ErrLog( LOG_LEVEL_DEBUG, "KDK in Str", 0, (unsigned char *)strRspMsg.c_str(), strRspMsg.length() );

	strKDKFileData = "DF1401";
	strKDKFileData.append(MD_SEC_BNT_DF14_KDK);
	strKDKFileData.append("DF15020001");
	strKDKFileData.append( strRspMsg.substr( pos1, 2+2+154 ) );

	memset( caTmpStr, 0, sizeof(caTmpStr) );
	DataFmtConv::asc_hex( caTmpStr, strKDKFileData.length()/2, strKDKFileData.c_str(), strKDKFileData.length() );

	hWriteFile.open( fileName.c_str(), ios::out );
	if( hWriteFile.bad() ){
		strErrMsg = "Open File for write error!";
		return iRet;
	}
	hWriteFile.write( (char *)caTmpStr, strKDKFileData.length()/2 );

	iRet = 0;
	if( hWriteFile.bad() ){
		strErrMsg = "Write file error!";
		iRet = -1;
	}

	hWriteFile.close();
	return iRet;
}

//------------------------------------------------------------------
// Function   : GenKeyFile_KSK
// Description: generate KSK key file
// Input      : filename -- KSK file name
// Output     :
// Return     :  0 succ
//							-1 fail
//------------------------------------------------------------------
int kmcUI::GenKeyFile_KSK( string fileName )
{
	int iRet = -1;
	int iRecCnt = 0;
	int pos1;
	string 	strsendCmd;
	string  strFileData;
	string 	strRspMsg;
	unsigned char caTmpStr[1024+1];
	kmcKeyInfo *pKeyInfo = new kmcKeyInfo();
	fstream	hWriteFile;
	kmcFileOp *pFileOpHandle = new kmcFileOp( strKmcKeyFileName, "R" );

	iRet = pFileOpHandle->OpenFile();
	if( iRet != 0 ){
		delete pKeyInfo;
		delete pFileOpHandle;
		return iRet;
	}

	//get KDK key info
	iRet = FindKeyFromFile( MD_SEC_BNT_TK_KDK, strSubType_DEFAULT, "0", &KeyInfo );
	if( iRet != 0 ){
		strErrMsg = "There is no KDK in key file!";
		pFileOpHandle->Close();
		delete pKeyInfo;
		delete pFileOpHandle;
		return iRet;
	}

	hWriteFile.open( fileName.c_str(), ios::out );
	if( hWriteFile.bad() ){
		strErrMsg = "Open File for write error!";
		pFileOpHandle->Close();
		delete pKeyInfo;
		delete pFileOpHandle;
		return -1;
	}

	strFileData = "DF1401";
	strFileData.append(MD_SEC_BNT_DF14_KSK);
	strFileData.append("DF15020000");

	memset( caTmpStr, 0, sizeof(caTmpStr) );
	DataFmtConv::asc_hex( caTmpStr, strFileData.length()/2, strFileData.c_str(), strFileData.length() );
	hWriteFile.write( (char *)caTmpStr, strFileData.length()/2 );
	if( hWriteFile.bad() ){
		strErrMsg = "Write KSK file error!";
		hWriteFile.close();
		pFileOpHandle->Close();
		delete pKeyInfo;
		delete pFileOpHandle;
		return -1;
	}

	//read key info from kmc key file one by one
	while(1){
		iRet = pFileOpHandle->ReadKmcKeyDataFile( pKeyInfo );
		if( iRet < 0 ){
			strErrMsg = "Read Kmc Key file error!";
			break;
		}
		else if( iRet == 2 ){   //read file over
			break;
		}

		//KDK key is not in KSK file
		if( MD_SEC_BNT_TK_KDK == pKeyInfo->GetKeyType() &&
			  strSubType_DEFAULT == pKeyInfo->GetSubType() ){
			continue;
		}

		//generate TransToken cmd
		strsendCmd = "";
		strsendCmd = KMCReq::FF21_TransToken_cmd( pKeyInfo->GetKeyToken(), KeyInfo.GetKeyToken() );

		memset( caBNTngCmd_snd, 0, sizeof(caBNTngCmd_snd) );
		iBNTngCmdLen_snd = 0;
		//iBNTngCmdLen_snd = DataFmtConv::Pack( (unsigned char *)strsendCmd.c_str(), caBNTngCmd_snd, strsendCmd.length() );
		iBNTngCmdLen_snd = DataFmtConv::Pack( (unsigned char *)strsendCmd.c_str(), caBNTngCmd_snd+KMC_SENDBUFLEN_LEN, strsendCmd.length() );
		DataFmtConv::long_tab( caBNTngCmd_snd, KMC_SENDBUFLEN_LEN, &iBNTngCmdLen_snd );
		iBNTngCmdLen_snd += KMC_SENDBUFLEN_LEN;

		//send cmd to ssm
		iRet = SendCmdToSSM();
		if( iRet != 0 ){
			strErrMsg = "SendCmdToSSM error!";
			break;
		}

		//write the response into KSK file
		memset( caTmpStr, 0, sizeof(caTmpStr) );
		DataFmtConv::Unpack( caBNTngCmd_rcv, caTmpStr, iBNTngCmdLen_rcv );
		strRspMsg = (char *)caTmpStr;
		pos1 = strRspMsg.find( "E1" );
		if( pos1<0 ){
			strErrMsg = "Error TransFormat of Token!";
			iRet = -1;
			break;
		}
		strFileData = strRspMsg.substr( pos1, 2+2+154 );

		errLog.ErrLog( LOG_LEVEL_DEBUG, "KSK in Str", 0, (unsigned char *)strRspMsg.c_str(), strRspMsg.length() );

		memset( caTmpStr, 0, sizeof(caTmpStr) );
		DataFmtConv::asc_hex( caTmpStr, strFileData.length()/2, strFileData.c_str(), strFileData.length() );
		hWriteFile.write( (char *)caTmpStr, strFileData.length()/2 );

		if( hWriteFile.bad() ){
			strErrMsg = "Write KSK file error!";
			iRet = -1;
			break;
		}

		pKeyInfo->ResetValue();
		iRecCnt ++;
	}

	//re-write the count of records in KSK file
	hWriteFile.seekp( 0, ios::beg );
	strFileData = "DF1401";
	strFileData.append(MD_SEC_BNT_DF14_KSK);
	strFileData.append("DF1502");
	char caCnt[4+1];
	memset( caCnt, 0, sizeof(caCnt));
	sprintf( caCnt, "%04d", iRecCnt );
	strFileData.append(caCnt);

	memset( caTmpStr, 0, sizeof(caTmpStr) );
	DataFmtConv::asc_hex( caTmpStr, strFileData.length()/2, strFileData.c_str(), strFileData.length() );
	hWriteFile.write( (char *)caTmpStr, strFileData.length()/2 );

	hWriteFile.seekp( 0, ios::end );
	hWriteFile.close();
	pFileOpHandle->Close();
	delete pKeyInfo;
	delete pFileOpHandle;
	if( iRet > 0 )
		iRet = 0;
	return iRet;
}

//------------------------------------------------------------------
// Function   : ImpDESKeyFileData
// Description: Import DES Key File data
// Input      : 
// Output     : 
// Return     :  0 succ
//							-1 fail
//------------------------------------------------------------------
int kmcUI::ImpDESKeyFileData()
{
	int iRet;
	unsigned long fileSize=0;
	fstream	hReadDESKeyFile;
	
	//Open file to Read all data in DataBuffer
	hReadDESKeyFile.open( strInputFileName.c_str(), ios::in );
	if( !hReadDESKeyFile )
	{
		strErrMsg = "Error while open:"+strInputFileName;
		return -1;
	}	
	hReadDESKeyFile.seekg( 0, ios::end );
	fileSize = hReadDESKeyFile.tellg();
	hReadDESKeyFile.seekg( 0, ios::beg );

	unsigned char *pcaDESFileData = new unsigned char[fileSize+1];
	memset( pcaDESFileData, 0, fileSize+1 );

	hReadDESKeyFile.read( (char*)pcaDESFileData, fileSize );
	hReadDESKeyFile.close();

	long lOffSet = 0;
	//check file type
	if( memcmp( pcaDESFileData, "\xDF\x14", 2 ) != 0 ||
			memcmp( pcaDESFileData+2+1, "\x37", 1 ) != 0 ){
		strErrMsg = "Error File Format:Tag DF14!";
		delete pcaDESFileData;
		return -1;
	}
	lOffSet += 4;
	
	//check file records number
	if( memcmp( pcaDESFileData+lOffSet, "\xDF\x15", 2 ) != 0 ){
		strErrMsg = "Error File Format:Tag DF15!";
		delete pcaDESFileData;
		return -1;
	}
	lOffSet += 2;
	//Tag Length 
	lOffSet += 1;
	
	char tmpRecCnt[2+1];
	char caRecCnt[4+1];
	int  iRecCnt=0,iImpRecCnt=0;
	memset( tmpRecCnt, 0, sizeof(tmpRecCnt) );
	memcpy( tmpRecCnt, pcaDESFileData+lOffSet, 2 );
	lOffSet += 2;
	memset( caRecCnt, 0, sizeof(caRecCnt) );
	DataFmtConv::hex_asc( caRecCnt, (unsigned char*)tmpRecCnt, 2 * 2 );
	iRecCnt = atoi(caRecCnt);
	
	int findPos;
	int iFF41Len;
	char *pcaStrBegin;
	char caTagStr_FF41[200+1];
	char caErrMsg[128];
	unsigned char caTmpStr[1024+1];
	string strTagStr_FF41;
	string encKeyToken;
	string encKeyId;
	string chkKeyId;
	string impKeyId;
	string impKeyKCV;
	string impKeyKEV;
	string stoKeyId;
	string strsendCmd;
	string strRspMsg;
	string importedToken;
	string impKeyDesc;
	kmcKeyInfo encKeyInfo;
	kmcKeyInfo impKeyInfo;
	
	//get keyinfo of encrypted key
	iRet = FindKeyFromFile( KeyInfo.GetKeyType(), KeyInfo.GetSubType(), KeyInfo.GetKeyVersion(), &encKeyInfo );
	if( iRet != 0 ){
		strErrMsg = "Encrypted key is not existed!";
		delete pcaDESFileData;
		return -1;
	}
	encKeyToken = encKeyInfo.GetKeyToken();
	encKeyId = BNTFmt::GenTagStr_D2( strNKComponentType, KeyInfo.GetKeyType(), KeyInfo.GetSubType(), KeyInfo.GetKeyVersion() );
	stoKeyId = BNTFmt::GenTagStr_D2( strNKComponentType, MD_SEC_BNT_TK_KDKM, strSubType_DEFAULT, "0" );

	while(1){
		//get FF41
		pcaStrBegin = NULL;
		iFF41Len = 0;
		pcaStrBegin = strstr( (char *)pcaDESFileData+lOffSet, "\xFF\x41" );
		
		if( pcaStrBegin == NULL ){
			if( lOffSet != fileSize ){
				strErrMsg = "Error File Format:Tag FF41!";
				delete pcaDESFileData;
				return -1;
			}
			else{ //Read over file data
				break;
			}
		}
		iFF41Len = pcaStrBegin[2];
		lOffSet += 2+1+iFF41Len;
		
		strTagStr_FF41 = "";
		memset( caTagStr_FF41, 0, sizeof(caTagStr_FF41) );
		DataFmtConv::hex_asc( caTagStr_FF41, (unsigned char*)pcaStrBegin, (2+1+iFF41Len) * 2 );
		strTagStr_FF41 = caTagStr_FF41;
	
		findPos = strTagStr_FF41.find( "D213");
		chkKeyId = strTagStr_FF41.substr( findPos, 21*2 );
		findPos += 21*2;
		
		impKeyId = strTagStr_FF41.substr( findPos, 21*2 );
		findPos += 21*2;
		
		//D4 03 or D4 06
		findPos = strTagStr_FF41.find( "D403", findPos );
		if( findPos >0 ){
			impKeyKCV = strTagStr_FF41.substr( findPos, (1+1+3)*2 );
			findPos += 5*2;
		}
		else{
			findPos = strTagStr_FF41.find( "D406", findPos );
			if( findPos <= 0 )
				continue;

			impKeyKCV = strTagStr_FF41.substr( findPos, (1+1+6)*2 );
			findPos += 8*2;
		}
		
		//D5 10 or D5 08
		findPos = strTagStr_FF41.find( "D510", findPos );
		if( findPos >0 ){
			impKeyKEV = strTagStr_FF41.substr( findPos, (1+1+16)*2 );
			findPos += 18*2;
		}
		else{
			findPos = strTagStr_FF41.find( "D508", findPos );
			if( findPos <= 0 )
				continue;

			impKeyKEV = strTagStr_FF41.substr( findPos, (1+1+8)*2 );
			findPos += 10*2;
		}
		
		iRet = CheckExistOfKeyInfo( impKeyId );
		if( iRet == 1 ){
			strErrMsg = "Import key is existed in KMC!";
			continue;
		}
		else if( iRet < 0 ){
			strErrMsg = "CheckExistOfKeyInfo Error!";
			delete pcaDESFileData;
			return -1;
		}
		
		strsendCmd = "";
		strsendCmd = KMCReq::EB_ImpKey_cmd( encKeyToken, impKeyId, impKeyKCV, impKeyKEV, stoKeyId );
		
		memset( caBNTngCmd_snd, 0, sizeof(caBNTngCmd_snd) );
		iBNTngCmdLen_snd = 0;
		iBNTngCmdLen_snd = DataFmtConv::Pack( (unsigned char *)strsendCmd.c_str(), caBNTngCmd_snd+KMC_SENDBUFLEN_LEN, strsendCmd.length() );
		DataFmtConv::long_tab( caBNTngCmd_snd, KMC_SENDBUFLEN_LEN, &iBNTngCmdLen_snd );
		iBNTngCmdLen_snd += KMC_SENDBUFLEN_LEN;

		//send cmd to ssm
		iRet = SendCmdToSSM();
		if( iRet != 0 ){
			strErrMsg = "SendCmdToSSM error!";
			delete pcaDESFileData;
			return -1;
		}

		memset( caTmpStr, 0, sizeof(caTmpStr) );
		DataFmtConv::Unpack( caBNTngCmd_rcv, caTmpStr, iBNTngCmdLen_rcv );
		strRspMsg = (char *)caTmpStr;
		findPos = strRspMsg.find( "C0020000" );
		if( findPos<0 ){
			strErrMsg = "Import Key Error!";
			delete pcaDESFileData;
			return -1;
		}
		
		importedToken = strRspMsg.substr( findPos+8, 79*2 );
		keyToken.CompTokenFromStr( importedToken );
		
		impKeyInfo.SetKeyType(keyToken.GetKT());	//keyType
		impKeyInfo.SetNKComponentType(strNKComponentType);
		if( strNKComponentType == "62" ){
			unsigned char caKN[1+1];
			char caSubTypeStr[2+1];
			char caKeyVerStr[1+1];
			memset( caKN, 0, sizeof(caKN) );
			DataFmtConv::asc_hex( caKN, 1, keyToken.GetKN().substr(0,2).c_str(), 2 );
			
			//AAAAAABB:AAAAAA-SubType BB-KeyVer
			//11111110:111111         10
			memset( caSubTypeStr, 0, sizeof(caSubTypeStr) );
			sprintf( caSubTypeStr, "%02d", caKN[0]/4 );
			
			memset( caKeyVerStr, 0, sizeof(caKeyVerStr) );
			sprintf( caKeyVerStr, "%d", caKN[0]%4 );
			
			impKeyInfo.SetSubType(caSubTypeStr);
			impKeyInfo.SetKeyVersion(caKeyVerStr);
			//Import By:KT=02 SubType=00 KeyVer=0
			impKeyDesc = "Import by:KT="+KeyInfo.GetKeyType().substr(0,2)+" ST="+KeyInfo.GetSubType().substr(0,2)+" KV="+KeyInfo.GetKeyVersion().substr(0,1);
		}
		else{
			impKeyInfo.SetSubType(keyToken.GetKN().substr( 0, 1 ));
			impKeyInfo.SetKeyVersion(keyToken.GetKN().substr( 1, 1 ));
			//Import By:KT=02 SubType=0 KeyVer=0
			impKeyDesc = "Import by:KT="+KeyInfo.GetKeyType().substr(0,2)+" ST="+KeyInfo.GetSubType().substr(0,1)+" KV="+KeyInfo.GetKeyVersion().substr(0,1);
		}
		impKeyInfo.SetDesc( impKeyDesc );
		impKeyInfo.SetKeyCheckValue(keyToken.GetKCV());
		impKeyInfo.SetKeyToken(importedToken);
		
		iRet = WriteKeyIntoFile(impKeyInfo);
		if( iRet < 0 ){
			delete pcaDESFileData;
			return -1;
		}

		iImpRecCnt ++;
	} 
	
	sprintf( caErrMsg, "Import Key File finish,%d succ of %d Total!", iImpRecCnt, iRecCnt );
	strErrMsg = caErrMsg;
	
	delete pcaDESFileData;
	return 0;
}

void kmcUI::Test()
{
	int iRet;
	iRet = FindKeyFromFile( "A1", strSubType_DEFAULT, "0", &KeyInfo );
	if( iRet !=0 ){
		return;
	}
	
	string encToken = KeyInfo.GetKeyToken();
	KeyToken keToken;
	string clearEncKEV;
	keToken.CompTokenFromStr( encToken );
	clearEncKEV = SecBasic::Dec( KDKM, keToken.GetKEV().substr( 0, 16 ) );
	clearEncKEV.append( SecBasic::Dec( KDKM, keToken.GetKEV().substr( 16, 16 ) ) );

	KeyToken keToken_imp;
	string clearImpKey;
	string ImpKeyKEV;
	string tag_FF41[2];
	
	keToken_imp.CompTokenFromStr( "E14DDF100103D21390112233445530000032100000000000000000D213A2112233445530000032100000000000000000D302FFFFDF110102D40314FDB0D510CD971B9B335D9E946D60DFED15F68A89" );
	clearImpKey = SecBasic::Dec( KDKM, keToken_imp.GetKEV().substr( 0, 16 ) );
	clearImpKey.append( SecBasic::Dec( KDKM, keToken_imp.GetKEV().substr( 16, 16 ) ) );
	ImpKeyKEV = SecBasic::Enc( clearEncKEV, clearImpKey.substr( 0, 16 ) );
	ImpKeyKEV.append(SecBasic::Enc( clearEncKEV, clearImpKey.substr( 16, 16 ) ) );
	tag_FF41[0] = BNTFmt::GenTagStr_FF41( keToken.GetKid(), keToken_imp.GetKid(), keToken_imp.GetKCV(), ImpKeyKEV );
	
	keToken_imp.CompTokenFromStr( "E14DDF100103D21390112233445530000032100000000000000000D213A3112233445530000032100000000000000000D302FFFFDF110102D40368F03AD51042CA705628F089897F96746ADFFFDBD2" );
	clearImpKey = SecBasic::Dec( KDKM, keToken_imp.GetKEV().substr( 0, 16 ) );
	clearImpKey.append( SecBasic::Dec( KDKM, keToken_imp.GetKEV().substr( 16, 16 ) ) );
	ImpKeyKEV = SecBasic::Enc( clearEncKEV, clearImpKey.substr( 0, 16 ) );
	ImpKeyKEV.append(SecBasic::Enc( clearEncKEV, clearImpKey.substr( 16, 16 ) ) );
	tag_FF41[1] = BNTFmt::GenTagStr_FF41( keToken.GetKid(), keToken_imp.GetKid(), keToken_imp.GetKCV(), ImpKeyKEV );
	
	string fileData;
	fstream	hWriteFile;
	string fileName = strOutputFilePath+"testkey.imp";
	
	fileData = "DF140137";
	fileData += "DF15020002";
	fileData += tag_FF41[0];
	fileData += tag_FF41[1];
	
	unsigned char *pcaFileData = new unsigned char[fileData.length()/2+1];
	memset( pcaFileData, 0, fileData.length()/2+1 );
	DataFmtConv::asc_hex( pcaFileData, fileData.length()/2, fileData.c_str(), fileData.length() );
	
	hWriteFile.open( fileName.c_str(), ios::out );
	if( hWriteFile.bad() ){
		strErrMsg = "Open File for write error!";
		delete pcaFileData;
		return;
	}
	hWriteFile.write( (char *)pcaFileData, fileData.length()/2 );

	if( hWriteFile.bad() ){
		strErrMsg = "Write file error!";
	}

	hWriteFile.close();
	delete pcaFileData;
	return;
}
