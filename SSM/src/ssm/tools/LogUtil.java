package ssm.tools; 

import java.io.*;

public class LogUtil
{
	private static long maxFileSize	= 5000000;
	private static String logFile = "log/ics";	
	public	static String sHint	= "";
	
	
	public static void setMaxFileSize(long fileSize)
	{
		maxFileSize = fileSize;
	}

	public static void setLogFile(String fileName)
	{
		logFile	 = fileName;
	}
        
        public static boolean d(String x, String y) {
            return writeLog(x + y);
        }

	public static boolean writeLog( String sLog )
	{
		try
		{
			File fObj = new File( logFile + ".log");
			File fTarget;
			long lFileSize = fObj.length();
			PrintWriter pw;
			String sTime = "\nTime: " + PubTools.getTime(0);
			sLog = sTime + "\n" + sLog;
			//rewrite log file if file size > maxFileSize
			if ( lFileSize > maxFileSize ) 
			{ 
				fTarget = new File(logFile + PubTools.getTime(4) + ".log");
				fObj.renameTo(fTarget);
				pw = new PrintWriter(new FileWriter(logFile +".log")); 
				pw.println(sLog); 
				pw.flush(); 
				pw.close(); 
			} 
			else
			{ 
				pw = new PrintWriter(new FileWriter(logFile+".log", true)); 
				pw.println(sLog); 
				pw.flush(); 
				pw.close(); 
			} 

			return true;
		}catch (IOException ie)
		{
			System.out.println("Write log file failed: IOException. "+ie.toString());
			sHint="Write log file failed: "+ie.toString();
			return false;
		}catch (SecurityException se)
		{
			System.out.println("Log file access failed: SecurityException."+se.toString());
			sHint="Log file access failed: "+se.toString();
			return false;
		}
	}
	
	public static boolean writeLog( String sTitle,String sLog )
	{
		try
		{
			File fObj = new File( logFile + ".log");
			File fTarget;
			long lFileSize = fObj.length();
			PrintWriter pw;
			String sTime = "\nTime: " + PubTools.getTime(0) + "\n";
			sLog = sTime + sTitle + "\n" + sLog;
			//rewrite log file if file size > maxFileSize
			if ( lFileSize > maxFileSize ) 
			{ 
				fTarget = new File(logFile + PubTools.getTime(4) + ".log");
				fObj.renameTo(fTarget);
				pw = new PrintWriter(new FileWriter(logFile +".log")); 
				pw.println(sLog); 
				pw.flush(); 
				pw.close(); 
			} 
			else
			{ 
				pw = new PrintWriter(new FileWriter(logFile+".log", true)); 
				pw.println(sLog); 
				pw.flush(); 
				pw.close(); 
			} 

			return true;
		}catch (IOException ie)
		{
			System.out.println("Write log file failed: IOException. "+ie.toString());
			sHint="Write log file failed: "+ie.toString();
			return false;
		}catch (SecurityException se)
		{
			System.out.println("Log file access failed: SecurityException."+se.toString());
			sHint="Log file access failed: "+se.toString();
			return false;
		}
	}
	
	/*
		Method Name: String HexStringLog(byte[] String sMsg)
		Function Description:

		Author:			Fan BeiZhang
		Create Date:	2002.4.18

		Argument:
			byte[] bMsg;
		Return: String
		the format will be??
		Displacement -0--1--2--3--4--5--6-Hex-8--9--A--B--C--D--E--F  --ASCII Value--
		00000(00000) 31 30 31 30 31 31 30 32 30 30 30 30 30 31 30 30  1010110200000100
		00016(00010) 39 38 00 30 32 30 31 00 b6 a8 c6 da d5 cb bb a7  98*0201*????????

		??????????
		1. ???????????????????????? displacement ???????????? sLog
		2. ?????????????? sMsg ????16?????????????? byte[]-bMsg
		3. ?? bMsg16 ?????? 16????????????????,	?????? sLog
		4. ?? bMsg16 ?????????????????????? bMsgChar, ??????????????????????0x2A("*")
				???????????? <0x20 or (>0x7E and <91)
		5. ?? bMsgChar ??????sByte16, (sByte16 = new String(bMsgChar))
		6. ?? sByte16?????? sLog
		[??] ??????len????16????????????????????????Hex???? (16-len)*3-1????ASCII +16-len??

	 */
	public static String HexStringLog(String sMsg)
	{
		String sLog = "Displacement -0--1--2--3--4--5--6-Hex-8--9--A--B--C--D--E--F  --ASCII Value---";
		String space= "                                                            ";
		byte[] bMessage, bMsg={0x00}, bMsgChar;
		String sByte;
		int	   iStart=0, iLen=0, sLen=0;
		int    i=0;
		String sTmp1, sTmp2;

		try
		{
			bMessage = sMsg.getBytes("ISO-8859-1");
			sLen = bMessage.length;
			//sLog = "Message length(byte) = " + sLen + "\n" + sLog;
			while (true)
			{
				//1. ???????????????????????? displacement ???????????? sLog
				sLog += "\n";
				sTmp1 = "0000"+iStart;
				sTmp2 = "0000"+(iStart/16*10);
				sLog += sTmp1.substring(sTmp1.length()-5)+"("+sTmp2.substring(sTmp2.length()-5)+") ";

				//2. ?????????????? sMsg ????16?????????????? byte[]-bMsg
				try{
					if ( (iStart+16)<sLen )
					{
						bMsg   = new byte[16];
						for ( i=0; i<16; i++ ) 
						{
							bMsg[i] = bMessage[iStart + i];
						}
					}
					else
					{
						bMsg   = new byte[sLen - iStart];
						for ( i=0; i<(sLen-iStart); i++ ) 
						{
							bMsg[i] = bMessage[iStart + i];
						}
					}
				} catch(ArrayIndexOutOfBoundsException aie)
				{
					System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
					System.out.println("    Caught an ArrayIndexOutOfBoundsException-1!");
					System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
				}catch(	StringIndexOutOfBoundsException sie)
				{
					System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
					System.out.println("    Caught a StringIndexOutOfBoundsException!-1");
					System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
				}
				
				iLen   = bMsg.length;

				//3. ?? bMsg ?????? 16????????????????,	?????? sLog, ????????
				sLog  += Hex.toStringSpace(bMsg) + space.substring(0,(16-iLen)*3);

				//4. ?? bMsg ?????????????????????? bMsgChar, ??????????????????????0x2A("*")
				bMsgChar = new byte[bMsg.length];
				for( i=0; i<bMsg.length; i++ )
				{
					if ( bMsg[i]<0x20 || (bMsg[i]>0x7E && bMsg[i]<0x91) || bMsg[i]==0xFF )
						bMsgChar[i] = 0x2A;
					else
						bMsgChar[i] = bMsg[i];
				}
				
				//5. ?? bMsgChar ??????sByte16, (sByte16 = new String(bMsgChar))
				sByte = new String(bMsgChar,"ISO-8859-1");
				//6. ?? sByte?????? sLog
				sLog += " "+sByte + space.substring(0, 16-sByte.length() ) ;

				iStart = iStart + 16;
				if ( iStart >= sLen )
					break;
			}	// end of while()
		}catch(	StringIndexOutOfBoundsException sioe)
		{
			System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
			System.out.println("    Caught a StringIndexOutOfBoundsException!-2");
			System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
		} catch(ArrayIndexOutOfBoundsException aie)
		{
			System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
			System.out.println("    Caught a ArrayIndexOutOfBoundsException!-2");
			System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
		}
		catch( Exception e)
		{
			e.printStackTrace();
		}
		//System.out.println("Message String: \n"+ sLog);
		return sLog;
	}
	
	public static String HexStringLog(byte[] bMessage, int sLen )
	{
		String sLog = "Displacement -0--1--2--3--4--5--6-Hex-8--9--A--B--C--D--E--F  --ASCII Value---";
		String space= "                                                            ";
		byte[] bMsg={0x00}, bMsgChar;
		String sByte;
		int	   iStart=0, iLen=0;
		int    i=0;
		String sTmp1, sTmp2;

		try
		{
			//sLog = "Message length(byte) = " + sLen + "\n" + sLog;
			while (true)
			{
				//1. ???????????????????????? displacement ???????????? sLog
				sLog += "\n";
				sTmp1 = "0000"+iStart;
				sTmp2 = "0000"+(iStart/16*10);
				sLog += sTmp1.substring(sTmp1.length()-5)+"("+sTmp2.substring(sTmp2.length()-5)+") ";

				//2. ?????????????? sMsg ????16?????????????? byte[]-bMsg
				try{
					if ( (iStart+16)<sLen )
					{
						bMsg   = new byte[16];
						for ( i=0; i<16; i++ ) 
						{
							bMsg[i] = bMessage[iStart + i];
						}
					}
					else
					{
						bMsg   = new byte[sLen - iStart];
						for ( i=0; i<(sLen-iStart); i++ ) 
						{
							bMsg[i] = bMessage[iStart + i];
						}
					}
				} catch(ArrayIndexOutOfBoundsException aie)
				{
					System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
					System.out.println("    Caught an ArrayIndexOutOfBoundsException-1!");
					System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
				}catch(	StringIndexOutOfBoundsException sie)
				{
					System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
					System.out.println("    Caught a StringIndexOutOfBoundsException!-1");
					System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
				}

				iLen   = bMsg.length;

				//3. ?? bMsg ?????? 16????????????????,	?????? sLog, ????????
				sLog  += Hex.toStringSpace(bMsg) + space.substring(0,(16-iLen)*3);

				//4. ?? bMsg ?????????????????????? bMsgChar, ??????????????????????0x2A("*")
				bMsgChar = new byte[bMsg.length];
				for( i=0; i<bMsg.length; i++ )
				{
					if ( bMsg[i]<0x20 || (bMsg[i]>0x7E && bMsg[i]<0x91) || bMsg[i]==0xFF )
						bMsgChar[i] = 0x2A;
					else
						bMsgChar[i] = bMsg[i];
				}
				
				//5. ?? bMsgChar ??????sByte16, (sByte16 = new String(bMsgChar))
				sByte = new String(bMsgChar);
				//6. ?? sByte?????? sLog
				sLog += " "+sByte + space.substring(0, 16-sByte.length() ) ;

				iStart = iStart + 16;
				if ( iStart >= sLen )
					break;
			}	// end of while()
		}catch(	StringIndexOutOfBoundsException sioe)
		{
			System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
			System.out.println("    Caught a StringIndexOutOfBoundsException!-2");
			System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
		} catch(ArrayIndexOutOfBoundsException aie)
		{
			System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
			System.out.println("    Caught a ArrayIndexOutOfBoundsException!-2");
			System.out.println("sLen="+ sLen +"    "+ "iStart="+iStart);
		}
		//System.out.println("Message String: \n"+ sLog);
		return sLog;
	}
}
