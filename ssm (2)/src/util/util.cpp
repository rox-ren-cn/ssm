//=============================
// util.cpp
// 
//=============================
#include "util.h"
#include <sys/timeb.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
using namespace std;
//-----------------------------


//------------------------------------------------------------------
// Function   : GetDateTimeStr
// Description: Get date time string in format YYYYMMDDHHMMSSMMM.
// Input      : none
// Output     : pcDateTime - date time string
// Return     : none
//------------------------------------------------------------------
void DateFmt::GetDateTimeStr( char *pcDateTime )
{
    struct tm *tm_p;
    struct timeb tmb_p;

    ftime( &tmb_p );
    
    tm_p = localtime(&tmb_p.time);
    sprintf( pcDateTime, "%04d%02d%02d%02d%02d%02d%03d",
            tm_p->tm_year+1900, tm_p->tm_mon+1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, tmb_p.millitm);
    
    return;
}

//------------------------------------------------------------------
// Function   : GetFmtDateTime
// Description: Get date time string "<YYYY MM DD HH:MM:SS MMM>"
// Input      : none
// Output     : pcDateTime - date time string
// Return     : none
//------------------------------------------------------------------
void DateFmt::GetFmtDateTime( char *pcDateTime )
{
    char caDateTimeStr[256];
	 memset( caDateTimeStr, 0, sizeof(caDateTimeStr));

	 GetDateTimeStr( caDateTimeStr );
	 strcpy( pcDateTime, "<");
	 strncat( pcDateTime, caDateTimeStr, 4 );
	 strcat( pcDateTime, "-" );
	 strncat( pcDateTime, caDateTimeStr + 4, 2 );
	 strcat( pcDateTime, "-" );
	 strncat( pcDateTime, caDateTimeStr + 6, 2 );
	 strcat( pcDateTime, " " );
	 strncat( pcDateTime, caDateTimeStr + 8, 2 );
	 strcat( pcDateTime, ":" );
	 strncat( pcDateTime, caDateTimeStr + 10, 2 );
	 strcat( pcDateTime, ":" );
	 strncat( pcDateTime, caDateTimeStr + 12, 2 );
	 strcat( pcDateTime, " " );
	 strncat( pcDateTime, caDateTimeStr + 14, 3 );
	 strcat( pcDateTime, ">");
    return;
}

//------------------------------------------------------------------
// Function   : Conv
// Description: See asc_hex().
// Input      : Oct - pointer to long
// Output     : none
// Return     : result data 
//------------------------------------------------------------------
unsigned char	DataFmtConv::Conv(unsigned char Oct)
{
    if (Oct >= 0x41) return ( (Oct - 7) & 0x0F );
    else             return ( Oct & 0x0F );
}

//------------------------------------------------------------------
// Function   : asc_bcd
// Description: Convert ASC array to BCD array.
//              "1234AB" -> "\x12\x34\xAB" if Ld = 3
//              "1234:;" -> "\x12\x34\xAB" if Ld = 3
//              "1234:;" -> "\x34\xAB" if Ld = 2              
// Input      : Pts - pointer to ASC array
//            : Ls - length of ASC array
//            : Ld - length of Hex array
// Output     : Ptd - pointer to Hex array
// Return     : none
//------------------------------------------------------------------
void	DataFmtConv::asc_hex( unsigned char *Ptd, const unsigned long Ld, const char *Pts, const unsigned long Ls)
{
	unsigned long I;

    (void) memset( Ptd, 0x00, Ld ) ;

    Ptd = Ptd + Ld - ((Ls + 1) / 2) ;
    if ( Ls % 2 ) *Ptd++ = Conv(*Pts++) & 0x0F ;
    for ( I = 0 ; I < (Ls / 2) ; I++)
    {
	    *Ptd = (Conv(*Pts++) << 4) & 0xF0 ;
	    *Ptd = *Ptd + (Conv(*Pts++) & 0x0F);
	    Ptd ++;
	}
}

//------------------------------------------------------------------
// Function   : hex_asc
// Description: Convert Hex array to ASC array.
//              "\x12\x34\xAB" -> "1234AB" if Lg = 6
//              "\x12\x34\xAB" -> "1234AB00" if Lg = 8
//              "\x12\x34\xAB" -> "123" if Lg = 3  
//              "\x01\x23\xAB" -> "123" if Lg = 3
//              "\x01\x23\xAB" -> "0123AB00" if Lg = 8
// Input      : Pts - pointer to Hex array
//              Lg - length of ASC array
// Output     : Ptd - pointer to Hex array
// Return     : none
// ------------------------------------------------------------------
void	DataFmtConv::hex_asc( char *Ptd, const unsigned char *Pts, const unsigned long Lg )
{
	char *Pt0;
	unsigned long I;

    Pt0 = Ptd ;
    if ( Lg % 2 ) *Ptd++ = (*Pts++ & 0x0F) + 0x30 ;
    for ( I = 0 ; I < (Lg / 2) ; I++)
    {
        *Ptd++ = ( (*Pts & 0xF0) >> 4 ) + 0x30 ;
        *Ptd++ = (*Pts++ & 0x0F) + 0x30 ;
    }
    while (Pt0 != Ptd)
    {
        if (*Pt0 >= 0x3A) *Pt0 += 7 ;
        Pt0++;
    }

	return;
}

//------------------------------------------------------------------
// Function   : long_asc
// Description: Convert unsigned long data to ASC array.
//              12345L -> "12345" if Lgd = 5
//              12345L -> "0012345" if Lgd = 7
// Input      : Pts - pointer to long
//            : Lgd - length of ASC array
// Output     : Ptd - pointer to ASC array
// Return     : none
//------------------------------------------------------------------
void	DataFmtConv::long_asc( char *Ptd, const int Lgd, const unsigned long *Pts )
{
	char	I,Oct,*Pt0,Tb[10];
	unsigned long	Lg1,Lg2;

    Lg1 = *Pts;
    Lg2 = 100000000L ;
    for (I = 0; I< 5; I++)
    {
		Oct = (unsigned char)(Lg1 / Lg2) ;
		Tb[2*I] = Oct / 10 + 0x30 ;
		Tb[2*I+1] = Oct % 10 + 0x30;
        Lg1 = Lg1 % Lg2;
        Lg2 = Lg2 / 100;
    }

	memset( Ptd, 0x30, Lgd ) ;
	Ptd += Lgd ;
   Pt0 = Ptd ;

	int Lg = Lgd;
	if ( Lg > 10 ) Lg = 10 ;
	for ( I=0; I < Lg;I++) *--Ptd = Tb[9-I] ;

	return;
}

/*
------------------------------------------------------------------
 Function   : asc_long
 Description: Convert ASC array to unsigned long data.
              "12345" -> 12345L if Ls = 5
              "12345" -> 345L if Ls = 3
              "0012345" -> 12345L if Ls = 5
 Input      : Pts - pointer to ASC array
            : Ls - length of ASC array
 Output     : none
 Return     : result unsigned long data
 
------------------------------------------------------------------
*/
unsigned long	DataFmtConv::asc_long( const char *Pts, const int Ls)
{
	unsigned char	I;
	unsigned long	Lg1,Lg2;

    Lg1 = 0 ;
    Lg2 = 1 ;
    Pts += Ls ;
    for (I = 0; I< Ls ; I++)
    {
        Lg1 += (Lg2 * (*--Pts & 0x0F) ) ;
        Lg2 *= 10 ;
    }

    return (Lg1) ;
}

//------------------------------------------------------------------
// Function   : long_tab
// Description: Convert unsigned long data to Hex array.
//              12345L -> "\x30\x39" if Lgd = 2
//              12345L -> "\x00\x30\x39" if Lgd = 3
// Input      : Pts - pointer to long
//            : Lgd - length of Hex array
// Output     : Ptd - Pointer to hex array
// Return     : none
//------------------------------------------------------------------
void DataFmtConv::long_tab( unsigned char *Ptd , const int Lgd, const unsigned long *Pts)
{
	unsigned char	I;

    (void) memset( Ptd, 0x00, Lgd ) ;
	 int Lg = Lgd;
    if (Lg > 4)
    {
        Ptd += ( Lg-4) ;
        Lg = 4;
    }
    for (I = (4-Lg); I < 4 ; I++)
    {
        *Ptd++ = (unsigned char) (*Pts >> (24 - 8*I) );
    }

	return;
}

//------------------------------------------------------------------
// Function   : tab_long
// Description: Convert Hex array to unsigned long data.
//              "\x30\x39" -> 12345L if Ls = 2
//              "\x30\x39" -> 57L if Ls = 1
// Input      : Pts - pointer to Hex array
//            : Ls - length of Hex array
// Output     : none
// Return     : result unsigned long data
// 
//------------------------------------------------------------------
unsigned long	DataFmtConv::tab_long( const unsigned char *Pts, const unsigned int Ls )
{
	unsigned char	I;
	unsigned long	Lg1,Lg2;

	Lg1 = 0 ;
	Lg2 = 1 ;
    Pts += Ls ;
    for (I = 0; I< Ls ; I++)
    {
        Lg1 += Lg2 * (*--Pts) ;
        if (Lg2 == 0x01000000L ) Lg2 = 0 ;
        else Lg2 *= 256 ;
    }
    return (Lg1) ;
}

//------------------------------------------------------------------
// Function   : long_bcd
// Description: Convert unsigned long data to BCD array.
//              12345L -> "\x01\x23\x45" if Lgd = 3
//              12345L -> "\x23\x45" if Lgd = 2
// Input      : Pts - pointer to long
//            : Lgd - length of BCD
// Output     : Ptd - Pointer to BCD
// Return     : none
//------------------------------------------------------------------
void	DataFmtConv::long_bcd( char *Ptd , const int Lgd, const unsigned long *Pts)
{
	char *Pt0;
	unsigned char	I,Tb[5];
	unsigned long	Lg1,Lg2;

    Lg1 = *Pts;
    Lg2 = 100000000L ;
    for (I = 0; I< 5; I++)
    {
        Tb[I] =(unsigned char)( Lg1 / Lg2) ;
        Tb[I] = ((Tb[I] / 10 ) << 4 ) + (Tb[I] % 10);
        Lg1 = Lg1 % Lg2;
        Lg2 = Lg2 / 100;
    }

    (void) memset( Ptd, 0x00, Lgd ) ;
    Ptd += Lgd ;
    Pt0 = Ptd ;
	 int Lg = Lgd;
    if ( Lg > 5) Lg = 5 ;
    for ( I=0; I < Lg;I++) *--Ptd = Tb[4-I] ;

	return;
}

//------------------------------------------------------------------
// Function   : bcd_long
// Description: Convert BCD array to unsigned long data.
//              "\x01\x23\x45" -> 12345L if Ls = 5
//              "\x01\x23\x45" -> 45L if Ls = 2
// Input      : Pts - pointer to BCD array
//           : Ls - length of BCD array
// Output     : none
// Return     : result unsigned long data
//-------------------------------------------------------------------
unsigned long	DataFmtConv::bcd_long( const char *Pts, const int Ls )
{
	unsigned char	I,Oct;
	unsigned long	Lg1,Lg2;

    Lg1 = 0 ;
    Lg2 = 1 ;
    Pts += (Ls+1)/2;
    for (I = 0; I< Ls ; I++)
    {
        if ( I % 2) Oct = (*Pts >> 4 ) & 0x0F;
        else Oct = *--Pts & 0x0F;
        Lg1 += Lg2 * Oct ;
        if (Lg2 == 1000000000L ) Lg2 = 0 ;
        else Lg2 *= 10 ;
    }
    return (Lg1) ;
}

//------------------------------------------------------------------
// Function   : asc_bcd
// Description: Convert ASC array to BCD array.
//              "1234AB" -> "\x12\x34\x12" if Lgd = 3
//              "1234:;" -> "\x12\x34\xAB" if Lgd = 3
//              "1234:;" -> "\x34\xAB" if Lgd = 2    
//              "1234:;" -> "\x00\x12\x34\xAB" if Lgd = 4
// Input      : Pts - pointer to ASC array
//            : Ls - length of ASC array
//            : Lgd - length of BCD array
// Output     : Ptd - pointer to BCD array
// Return     : none
//------------------------------------------------------------------
void	DataFmtConv::asc_bcd(unsigned char *Ptd, const unsigned long Lgd, const char *Pts, const unsigned long Lgs)
{
	unsigned long I;

	if( Lgd > Lgs/2)
    {
        (void) memset( Ptd, 0x00, Lgd ) ;
        Ptd = Ptd + Lgd - ((Lgs + 1) / 2) ;
    }
    for ( I = 0 ; I < ((Lgs+1) / 2) ; I++)
    {
		if ( (!(Lgs % 2) && !I) || I ) *Ptd =  (*Pts++ << 4) & 0xF0 ;
	    *Ptd = *Ptd + (*Pts++ & 0x0F)  ;
	    Ptd ++;
    }

	return;
}

//------------------------------------------------------------------
// Function   : bcd_asc
// Description: Convert BCD array to ASC array.
//              "\x12\x34\xAB" -> "1234:;" if Lg = 6
//              "\x12\x34\xAB" -> "1234:;00" if Lg = 8
//              "\x12\x34\xAB" -> "123" if Lg = 3  
// Input      : Pts - pointer to BCD array
//            : Lg - length of ASC array
// Output     : Ptd - pointer to ASC array
// Return     : none
// 
//------------------------------------------------------------------
void	DataFmtConv::bcd_asc( char *Ptd , const unsigned char *Pts , const unsigned long Lg)
{
	unsigned long	I;
	
	if ( Lg % 2 ) 
	*Ptd++ = (*Pts++ & 0x0F) + 0x30 ;
	for ( I = 0 ; I < (Lg / 2) ; I++)
	{
	  *Ptd++ = ( (*Pts & 0xF0) >> 4 ) + 0x30 ;
	  *Ptd++ = (*Pts++ & 0x0F) + 0x30 ;
	}

	return;
}

//=============================

/*
**   This Function only pack HEX number.For example: '0','A'.
**   input:pbyInBuffer is "1234567FE" and  iInBuffLen=9;
**   out: pbyOutBuffer is 0x12 0x34 0x56 0x7F 0xE0
*/
int DataFmtConv::Pack( unsigned char *pbyInBuffer, unsigned char *pbyOutBuffer, int iInBuffLen )
{
   unsigned char n;
   int iLen,i;
 
   /*
   **  whether InBuffer data's len can be divided by 2
   */
   if(iInBuffLen%2)
      iLen=iInBuffLen+1;
   else
      iLen=iInBuffLen;

   for (i=0; i<iInBuffLen; i++)
   {
      pbyInBuffer[i]=(unsigned char) toupper(pbyInBuffer[i]);

      if (i%2!=0)    /* if odd number */
      {            
         n =(unsigned char) (pbyInBuffer[i] - 0x30);
         /*
         **  Note: 'A' = 65, 'F'= 70.  65-48 = 17, 17-7=10.
         **  For example, this will convert 'A' to value 10.
         */
         if (n>9) 
            n = n-7;
         pbyOutBuffer[i/2] = pbyOutBuffer[i/2] | n;
      }
      else
      {
         pbyOutBuffer[i/2] = ( (n=pbyInBuffer[i]-0x30)>9 ? n-7:n ) << 4;
      }
   }

   return(iLen/2);
}

/*
**   This function unpack hex to ascii . for example,
**   input pbyInBuffer is 0x12 0x34 0x56 0x7F 0xE0 ,iInBufLen=5;
**   pbyOutBuffer is "1234567fe0".
*/
int DataFmtConv::Unpack( unsigned char *pbyInBuffer,
        unsigned char *pbyOutBuffer,
        int iInBufLen )
{
   int i,j;

   for (i=0,j=0; j<iInBufLen; j++, i=i+2)
   {
      if ( (pbyInBuffer[j]/16)>9 )
         pbyOutBuffer[i] = (pbyInBuffer[j]/16)+'0'+7;
      else
         pbyOutBuffer[i] = (pbyInBuffer[j]/16)+'0';

      if ( (pbyInBuffer[j]%16)>9 )
         pbyOutBuffer[i+1] = (pbyInBuffer[j]%16)+'0'+7;
      else
         pbyOutBuffer[i+1] = (pbyInBuffer[j]%16)+'0';
   }

   return(0);

}

/*
------------------------------------------------------------------
 Function   : CvtLetterCase
 Description: convert letter case by convert flag
 Input      : string inputStr
 	            int cvtFlag   -- convert flag
 	                0 - tolower
 	                1 - toupper
 Output     : 
 Return     : formatted string
------------------------------------------------------------------
*/
string DataFmtConv::CvtLetterCase( string inputStr, int cvtFlag )
{
	int iAddNum = 0;
	
	if( cvtFlag == 0 ){
		iAddNum = 32;
	}
	else if( cvtFlag == 1 ){
		iAddNum = -32;
	}
	else{
		return inputStr;
	}
	
	char *pcaInputStr = new char[inputStr.length()+1];
	
	memset( pcaInputStr, 0, inputStr.length()+1 );
	memcpy( pcaInputStr, inputStr.c_str(), inputStr.length() );
	
	for( int i=0; i<inputStr.length(); i++ ){
		if( pcaInputStr[i] >= 'A' && pcaInputStr[i] <= 'Z' ){
			if( cvtFlag == 0 ){
				pcaInputStr[i] += iAddNum;
			}
			else{
				continue;
			}
		}
		else if( pcaInputStr[i] >= 'a' && pcaInputStr[i] <= 'z' ){
			if( cvtFlag == 1 ){
				pcaInputStr[i] += iAddNum;
			}
			else{
				continue;
			}
		}
	}
	
	string TmpStr = pcaInputStr;
	delete pcaInputStr;
	return TmpStr;
}

/*
------------------------------------------------------------------
 Function   : HexStr2Int
 Description: convert HexString to int
 Input      : string inputStr
 Example    : input  -- "3f0a"
 	            return -- 3*16*16*16+15*16*16+0*16+10
 Output     : 
 Return     : >=0 int value
 	            <0 fail
------------------------------------------------------------------
*/
int DataFmtConv::HexStr2Int( string inputStr )
{
	int iRetValue=0;
	int iHexStrLen = 0;
	unsigned char caHexStr[2+1];

	if( inputStr.length() > 4 ){
		return -1;
	}
	
	iHexStrLen = inputStr.length()/2+inputStr.length()%2;
	memset( caHexStr, 0, sizeof(caHexStr) );
	DataFmtConv::asc_hex( caHexStr, iHexStrLen, inputStr.c_str(), inputStr.length() );

	if( iHexStrLen == 2 ){
		iRetValue = caHexStr[0]*256+caHexStr[1];
	}
	else if( iHexStrLen == 1 ){
		iRetValue = caHexStr[0];
	}

	return iRetValue;
}

/*
------------------------------------------------------------------
 Function   : NumToWord
 Description: convert numStr to word format
 Input      : string numStr
 Example    : input  -- "1234"
 	            return -- "one two three four"
 Output     : 
 Return     : numStr in word format
------------------------------------------------------------------
*/
string DataFmtConv::NumToWord( string numStr )
{
	string numInWordFmt;
	string retStr;
	
	for( int i=0; i<numStr.length(); i++ ){
		if( numStr.substr( i, 1 ) == "0" ){
			numInWordFmt = "ZERO ";
		}
		else if( numStr.substr( i, 1 ) == "1" ){
			numInWordFmt = "ONE ";
		}
		else if( numStr.substr( i, 1 ) == "2" ){
			numInWordFmt = "TWO ";
		}
		else if( numStr.substr( i, 1 ) == "3" ){
			numInWordFmt = "THREE ";
		}
		else if( numStr.substr( i, 1 ) == "4" ){
			numInWordFmt = "FOUR ";
		}
		else if( numStr.substr( i, 1 ) == "5" ){
			numInWordFmt = "FIVE ";
		}
		else if( numStr.substr( i, 1 ) == "6" ){
			numInWordFmt = "SIX ";
		}
		else if( numStr.substr( i, 1 ) == "7" ){
			numInWordFmt = "SEVEN ";
		}
		else if( numStr.substr( i, 1 ) == "8" ){
			numInWordFmt = "EIGHT ";
		}
		else if( numStr.substr( i, 1 ) == "9" ){
			numInWordFmt = "NINE ";
		}
		else{
			numInWordFmt = " ";
		}
		retStr.append( numInWordFmt );
	}
	return retStr;
}

/*
------------------------------------------------------------------
 Function   : CheckIntStr
 Description: check pcaInputStr whether compose of int
 Input      : char *pcaInputStr
 	            int iStrLen 
 Output     : 
 Return     : 0  - True
              -1 - False
------------------------------------------------------------------
*/
int DataFmtCheck::CheckIntStr( const char *pcaInputStr, int iStrLen )
{
	for( int i=0; i<iStrLen; i++ ){
		if( pcaInputStr[i] < '0' || pcaInputStr[i] > '9' ){
			return -1;
		}
	}
	return 0;
}

/*
------------------------------------------------------------------
 Function   : CheckLetterStr
 Description: check pcaInputStr whether compose of letters
 Input      : char *pcaInputStr
 	            int iStrLen 
 Output     : 
 Return     : 0  - True
              -1 - False
------------------------------------------------------------------
*/
int DataFmtCheck::CheckLetterStr( const char *pcaInputStr, int iStrLen )
{
	for( int i=0; i<iStrLen; i++ ){
		if( pcaInputStr[i] < 'A' || pcaInputStr[i] > 'z' ){
			return -1;
		}
		else if( pcaInputStr[i] > 'Z' && pcaInputStr[i] < 'a' ){
			return -1;
		}
	}
	return 0;
}

/*
------------------------------------------------------------------
 Function   : CheckHexStr
 Description: check pcaInputStr whether compose of hex num
 Input      : char *pcaInputStr
 	            int iStrLen 
 Output     : 
 Return     : 0  - True
              -1 - False
------------------------------------------------------------------
*/
int DataFmtCheck::CheckHexStr( const char *pcaInputStr, int iStrLen )
{
	for( int i=0; i<iStrLen; i++ ){
		if( pcaInputStr[i] < '0' || pcaInputStr[i] > 'f' ){
			return -1;
		}
		else if( pcaInputStr[i] > '9' && pcaInputStr[i] < 'A' ){
			return -1;
		}
		else if( pcaInputStr[i] > 'F' && pcaInputStr[i] < 'a' ){
			return -1;
		}
	}
	return 0;
}
