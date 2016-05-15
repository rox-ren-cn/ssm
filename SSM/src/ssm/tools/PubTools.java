package ssm.tools;

import java.util.*;
import java.io.*;

public class PubTools {

    Hashtable<String, String> errMsgSet = new Hashtable<String, String>();
    String sHint = "";

    public String getHint() {
        return sHint;
    }

    //change value null to ""
    public String initVar(String instr) {
        String outstr;
        if (instr == null) {
            outstr = "";
        } else {
            outstr = instr.trim();
        }
        return outstr;
    }
    //change value null to "&nbsp;"

    public String initHtmlSpace(String instr) {
        String outstr;
        if (instr == null || instr.equals("")) {
            outstr = "&nbsp;";
        } else {
            outstr = instr.trim();
        }
        return outstr;
    }

    //get the right length's value
    //if input longer value, cut it
    //if input shorter value, add space to the end
    public static String packVar(String val, int len) {
        int iVarLen = val.length();
        byte[] Hex00 = {0x00};

        if (iVarLen >= len) {
            val = val.substring(0, len);
        }

        if (iVarLen < len) {
            for (int i = 0; i < (len - iVarLen); i++) {
//				val+=" ";
                val += new String(Hex00);
            }
        }
        return val;
    }

    /*
	**if style="+0"; then add "0" behind string val (default);
	**if style="0+"; then add "0" before string val;
	**if string val length > len; then output string val from 0-len;
     */
    public static String packVar(String val, int len, String style) {
        int iVarLen = val.length();
        String sChar = "";

        if (style.length() != 2) {
            style = "+ ";
        }

        if (iVarLen >= len) {
            val = val.substring(0, len);
        }

        if (iVarLen < len) {
            for (int i = 0; i < (len - iVarLen); i++) {
                if (style.startsWith("+")) {
                    sChar = style.substring(1, 2);
                    val += sChar;
                } else {
                    sChar = style.substring(0, 1);
                    val = sChar + val;
                }
            }
        }
        return val;
    }

    //	get system time
    public static String getTime(int i) {
        Calendar calDate = Calendar.getInstance();
        String sWeekday = String.valueOf(calDate.get(Calendar.DAY_OF_WEEK));
        String sYear = String.valueOf(calDate.get(Calendar.YEAR));

        String sMonth = String.valueOf(calDate.get(Calendar.MONTH) + 1);
        if (sMonth.length() < 2) {
            sMonth = "0" + sMonth;
        }

        String sDay = String.valueOf(calDate.get(Calendar.DAY_OF_MONTH));
        if (sDay.length() < 2) {
            sDay = "0" + sDay;
        }

        String sHour = String.valueOf(calDate.get(Calendar.HOUR_OF_DAY));
        if (sHour.length() < 2) {
            sHour = "0" + sHour;
        }

        String sMinute = String.valueOf(calDate.get(Calendar.MINUTE));
        if (sMinute.length() < 2) {
            sMinute = "0" + sMinute;
        }

        String sSecond = String.valueOf(calDate.get(Calendar.SECOND));
        if (sSecond.length() < 2) {
            sSecond = "0" + sSecond;
        }

        String sDate = sYear + sMonth + sDay;
        String sMonthDay = sMonth + sDay;
        String sTime = sHour + sMinute + sSecond;
        String sDateTime = sYear + sMonth + sDay + sHour + sMinute + sSecond;
        String sNow = sYear + "-" + sMonth + "-" + sDay + " " + sHour + ":" + sMinute + ":" + sSecond;
        String sToday = sYear + "-" + sMonth + "-" + sDay;

        switch (i) {
            case 1:
                return sToday;
            case 2:
                return sWeekday;
            case 3:
                return sDateTime;
            case 4:
                return sDate;
            case 5:
                return sTime;
            case 6:
                return sYear;
            case 7:
                return sMonth;
            case 8:
                return sDay;
            case 9:
                return sMonthDay;
            /*case 10:
			return sDaysOfMonth;*/
            default:
                return sNow;
        }
    }

    //write log file
    public boolean writeLog(String sFile, String sLog) {
        try {
            File fObj = new File(sFile);
            long lFileSize = fObj.length();
            PrintWriter pw;
            if (getTime(8).equals("1")) {
                if (lFileSize > 10000000) {
                    pw = new PrintWriter(new FileWriter(sFile));
                    pw.println(sLog);
                    pw.flush();
                    pw.close();
                } else {
                    pw = new PrintWriter(new FileWriter(sFile, true));
                    pw.println(sLog);
                    pw.flush();
                    pw.close();
                }
            } else {
                pw = new PrintWriter(new FileWriter(sFile, true));
                pw.println(sLog);
                pw.flush();
                pw.close();
            }

            return true;
        } catch (IOException ie) {
            sHint = "Write log file failed: " + ie.toString();
            return false;
        } catch (SecurityException se) {
            sHint = "Log file access failed: " + se.toString();
            return false;
        }
    }

    public boolean writeLog(String sPath, String sFile, String sLog, long maxFileSize) {
        try {
            File fObj = new File(sPath + sFile + ".log");
            File fTarget;
            long lFileSize = fObj.length();
            PrintWriter pw;
            //rewrite log file from the first day of a month and file size > maxFileSize
            if (getTime(8).equals("1") || lFileSize > maxFileSize) {
                fTarget = new File(sPath + sFile + getTime(4) + ".log");
                fObj.renameTo(fTarget);
                pw = new PrintWriter(new FileWriter(sPath + sFile + ".log"));
                pw.println(sLog);
                pw.flush();
                pw.close();
            } else {
                pw = new PrintWriter(new FileWriter(sPath + sFile + ".log", true));
                pw.println(sLog);
                pw.flush();
                pw.close();
            }

            return true;
        } catch (IOException ie) {
            System.out.println("Write log file failed: IOException. " + ie.toString());
            sHint = "Write log file failed: " + ie.toString();
            return false;
        } catch (SecurityException se) {
            System.out.println("Log file access failed: SecurityException." + se.toString());
            sHint = "Log file access failed: " + se.toString();
            return false;
        }
    }

    //output date(yyyy-mm-dd) to string yyyymmdd
    public String date2str(String date) {
        String sOut = "";
        try {
            if (date.length() > 9) {
                sOut = date.substring(0, 4) + date.substring(5, 7) + date.substring(8, 10);
            } else if (date.length() < 9) {
                sOut = date;
            }
            return sOut;
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Date to string failed: " + ae.toString();
            return "";
        }

    }

    //output date(yyyy-mm-dd hh:mm:ss) to string yyyymmddhhmmss
    public String dateTime2str(String date) {
        String sOut = "";
        try {
            if (date.length() > 18) {
                sOut = date.substring(0, 4) + date.substring(5, 7) + date.substring(8, 10) + date.substring(11, 13) + date.substring(14, 16) + date.substring(17, 19);
            } else if (date.length() < 15) {
                sOut = date;
            }
            return sOut;
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Date time to string failed: " + ae.toString();
            return "";
        }

    }

    /* seek str2 in str1, replace str2 with str3 */
    public String strReplace(String str1, String str2, String str3) {
        int iLength2 = 0;
        int iLength3 = 0;
        int iOffset = 0;
        int iPos = 0;
        String sNewStr = str1;

        iLength2 = str2.length();
        iLength3 = str3.length();

        while ((iPos = sNewStr.indexOf(str2, iOffset)) != -1) {
            try {
                sNewStr = sNewStr.substring(0, iPos) + str3 + sNewStr.substring(iPos + iLength2);
                iOffset = iPos + iLength3;
            } catch (StringIndexOutOfBoundsException oe) {
                sHint = "substring out of bounds: " + oe.toString();
            }
        }

        return sNewStr;

    }

    public boolean readErrMsg(String fileName, String separator) {
        String aLine;
        String errCode;
        String errMsg;
        int offset = 0;
        BufferedReader in;

        try {
            in = new BufferedReader(new FileReader(fileName));
            while ((aLine = in.readLine()) != null) {
                offset = aLine.indexOf(separator);
                if (offset != -1) {
                    errCode = aLine.substring(0, offset).trim();
                    errMsg = aLine.substring(offset + separator.length());
                    errMsgSet.put((String) errCode, (String) errMsg);
                }
            }
            in.close();
            return true;
        } catch (FileNotFoundException fe) {
            sHint = fe.toString();
            return false;
        } catch (IOException ie) {
            sHint = ie.toString();
            return false;
        } catch (StringIndexOutOfBoundsException se) {
            sHint = se.toString();
            return false;
        }
    }

    public String readErrMsg(String fileName, String separator, String code) {
        String aLine;
        String errCode;
        String errMsg = "";
        int offset = 0;
        BufferedReader in;

        try {
            if (code.equals("")) {
                return "";
            }
            in = new BufferedReader(new FileReader(fileName));
            if (in != null) {
                while ((aLine = in.readLine()) != null) {
                    offset = aLine.indexOf(separator);
                    if (offset != -1) {
                        errCode = aLine.substring(0, offset).trim();
                        errMsg = aLine.substring(offset + separator.length());
                        if (code.equals(errCode)) {
                            break;
                        }
                    }
                }
            }
            in.close();
            return errMsg;
        } catch (FileNotFoundException fe) {
            sHint = fe.toString();
            return "";
        } catch (IOException ie) {
            sHint = ie.toString();
            return "";
        } catch (StringIndexOutOfBoundsException se) {
            sHint = se.toString();
            return "";
        }
    }

    public String getErrMsg(String errCode) {
        String errMsg = null;
        if (errMsgSet != null) {
            errMsg = (String) errMsgSet.get((String) errCode);
        }
        return errMsg;
    }

    public Hashtable<String, String> getErrMsgSet() {
        return errMsgSet;
    }

    public boolean isEmpty(String s) {
        if (s != null) {
            if (s.equals("")) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    public static String getSTAN(String fileName, int i) {
        char[] buf = new char[20];
        PrintWriter pw;
        BufferedReader in;
        String str;
        String fileContent = "";
        int stan;
        File f = new File(fileName);

        try {
            if (!f.exists()) {
                str = packVar("1", i, "0+");
                pw = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
                pw.println(str);
                pw.flush();
                pw.close();
            }
            in = new BufferedReader(new FileReader(fileName));
            fileContent = "";
            while ((in.read(buf, 0, buf.length)) != -1) {
                fileContent += String.valueOf(buf);
            }

            in.close();

            fileContent = fileContent.trim();
            if (fileContent.equals("")) {
                fileContent = packVar("1", i, "0+");
            }
            if (fileContent.length() < i) {
                fileContent = fileContent.substring(0, i);
            }
            stan = Integer.parseInt(fileContent);
            stan++;
            if (stan > 999999) {
                stan = 1;
            }
            str = packVar(String.valueOf(stan), i, "0+");
            pw = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
            pw.println(str);
            pw.flush();
            pw.close();
        } catch (FileNotFoundException fe) {
            //sHint = fe.toString();
            return "";
        } catch (IOException ie) {
            //sHint = ie.toString();
            return "";
        } catch (SecurityException se) {
            //sHint = "Write file failed: "+se.toString();
            return "";
        }
        return fileContent;

    }

    public String getColFullName(String[] cn, String[] cfn, String s) {
        try {
            for (int i = 0; i < cn.length; i++) {
                if (s.toUpperCase().equals(cn[i].toUpperCase())) {
                    return cfn[i];
                }
            }
            return "";
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Column name's number error: " + ae.toString();
            return "";
        }
    }

    public String getColCheck(String col, String s) {
        int len = s.length();
        int format = 0;
        String str = ",'" + col + "'";

        try {
            for (int i = 0; i < len; i++) {
                format = Integer.parseInt(s.substring(i, i + 1));
                switch (format) {
                    case 1:
                        str += ",'null'";
                        break;
                    case 2:
                        str += ",'date'";
                        break;
                    case 3:
                        str += ",'time'";
                        break;
                    case 4:
                        str += ",'datetime'";
                        break;
                    case 5:
                        str += ",'number'";
                        break;
                    case 6:
                        str += ",'digit'";
                        break;
                    case 7:
                        str += ",'full'";
                        break;
                }
            }

            return str;
        } catch (StringIndexOutOfBoundsException se) {
            sHint = se.toString();
            return "";
        }
    }

    public String writeReadonly(String s, String[] key, boolean b) {
        try {
            for (int i = 0; i < key.length; i++) {
                if (s.toUpperCase().equals(key[i]) && b) {
                    return "READONLY";
                }
            }
            return "";
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Key's number error: " + ae.toString();
            return "";
        }
    }

    public String writeDisabled(String s, String[] key, boolean b) {
        try {
            for (int i = 0; i < key.length; i++) {
                //if (s.toUpperCase().equals(key[i]) && b)	// by fanxs 20040419
                if (s.toUpperCase().equals(key[i].toUpperCase()) && b) {
                    return "DISABLED";
                }
            }
            return "";
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Key's number error: " + ae.toString();
            return "";
        }
    }

    public String writeSelected(String s, String value) {
        if (s.equals(value)) {
            return "SELECTED";
        }
        if (s.equals(value)) {
            return "SELECTED";
        }
        return "";
    }

    public String writeKeyMark(String s, String[] key) {
        try {
            for (int i = 0; i < key.length; i++) {
                if (s.toUpperCase().equals(key[i])) {
                    return "<font color=ff0000>*</font>";
                }
            }
            return "";
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = "Key's number error: " + ae.toString();
            return "";
        }
    }

    public String writeNotNullMark(String s) {
        try {
            if (s.indexOf("1") != -1) {
                return "<font color=ff0000>*</font>";
            } else {
                return "";
            }
        } catch (StringIndexOutOfBoundsException se) {
            sHint = se.toString();
            return "";
        }
    }

    public String getFieldSize(int size) {

        try {
            if (size <= 5) {
                return String.valueOf(size);
            }
            if (size <= 20) {
                return String.valueOf(size + 5);
            } else {
                return String.valueOf(25);
            }
        } catch (NumberFormatException ne) {
            sHint = ne.toString();
            return "";
        }
    }




    public String getListValue(String s, String[][] list) {
        try {
            for (int i = 0; i < list[0].length; i++) {
                if (s.equalsIgnoreCase(list[0][i])) {
                    return list[1][i];
                }
            }
            return "";
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = ae.toString();
            return "";
        }
    }

    //added by FAN 2003-6-3
    public String getListValue(String key, String rbBase, String path, Locale locale) {
        ResourceBundle rb = null;

        rbBase = path + "." + rbBase;
        rb = ResourceBundle.getBundle(rbBase, locale);
        try {
            return key + "-" + rb.getString(key);
        } catch (Exception e) {
            sHint = e.toString();
            return "";
        }
    }

    public boolean isKey(String s, String[] keyCols) {
        try {
            for (int i = 0; i < keyCols.length; i++) {
                if (s.equals(keyCols[i])) {
                    return true;
                }
            }
            return false;
        } catch (ArrayIndexOutOfBoundsException ae) {
            sHint = ae.toString();
            return false;
        }
    }

    /*
		锟斤拷锟杰ｏ拷取一锟斤拷锟街凤拷锟斤拷锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟叫碉拷锟铰憋拷
		锟斤拷锟斤拷String[] s - 锟街凤拷锟斤拷锟斤拷
			  String str - 要锟斤拷锟揭碉拷锟街凤拷
		锟斤拷锟斤拷值锟斤拷锟揭碉拷锟斤拷	锟斤拷锟斤拷锟铰憋拷值锟斤拷
				未锟揭碉拷锟斤拷锟斤拷锟斤拷 -1
     */
    public int getIndex(String[] s, String str) {
        int len = s.length;
        for (int i = 0; i < len; i++) {
            if (str.equals(s[i])) {
                return i;
            }
        }
        return -1;
    }

    /*
		锟斤拷锟杰ｏ拷锟斤拷锟斤拷锟斤拷值(String)锟斤拷YYYY-MM-DD锟斤拷式转锟斤拷为YYYYMMDD锟斤拷式
		锟斤拷锟斤拷String dt  - 锟斤拷锟斤拷锟街凤拷
		      int    mod - 锟斤拷锟斤拷锟绞�
					     1 - YYYYMMDD
						 2 - MMDDYYYY
						 3 - DDMMYYYY
		锟斤拷锟斤拷值锟斤拷锟斤拷锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷锟绞揭拷螅锟斤拷?锟斤拷锟斤拷 YYYYMMDD
				锟斤拷锟斤拷锟斤拷锟津：凤拷锟斤拷原锟斤拷
     */
    public String dateToChar(String dt, int mode) {
        // this functin is only for informix.
        int len = dt.length();
        if (len == 10) {
            switch (mode) {
                case 1:
                    if (dt.charAt(4) == '-' && dt.charAt(7) == '-') {
                        return (dt.substring(0, 4) + dt.substring(5, 7) + dt.substring(8));
                    }
                case 2:
                    if (dt.charAt(4) == '-' && dt.charAt(7) == '-') {
                        return (dt.substring(5, 7) + dt.substring(8) + dt.substring(0, 4));
                    }
                case 3:
                    if (dt.charAt(4) == '-' && dt.charAt(7) == '-') {
                        return (dt.substring(8) + dt.substring(5, 7) + dt.substring(0, 4));
                    }

                default:
                    return dt;
            }
        }
        return dt;
    }

    /*
		Method Name: String 8583MsgToLogString(byte[] String sMsg)
		Function Description:

		Author:			Fan Xuesong
		Create Date:	2002.4.18

		Argument:
			byte[] bMsg;
		Return: String
		the format will be锟斤拷
		Displacement -0--1--2--3--4--5--6-Hex-8--9--A--B--C--D--E--F  --ASCII Value--
		00000(00000) 31 30 31 30 31 31 30 32 30 30 30 30 30 31 30 30  1010110200000100
		00016(00010) 39 38 00 30 32 30 31 00 b6 a8 c6 da d5 cb bb a7  98*0201*锟斤拷锟斤拷锟剿伙拷

		锟斤拷锟�锟借：
		1. 确锟斤拷取锟斤拷锟斤拷位锟矫ｏ拷锟斤拷锟姐本锟斤拷 displacement 锟斤拷值锟斤拷追锟接碉拷 sLog
		2. 锟斤拷确锟斤拷锟斤拷位锟斤拷取 sMsg 锟叫碉拷16锟斤拷锟街节ｏ拷转锟斤拷为 byte[]-bMsg
		3. 锟斤拷 bMsg16 转锟斤拷为 16锟斤拷锟斤拷锟斤拷示锟斤拷锟街凤拷,	追锟接碉拷 sLog
		4. 锟斤拷 bMsg16 锟叫碉拷锟斤拷锟矫匡拷锟斤拷纸诟锟斤拷频锟�bMsgChar, 锟斤拷锟斤拷锟叫的诧拷锟缴硷拷锟街凤拷转为0x2A("*")
				锟斤拷锟缴硷拷锟街凤拷锟斤拷 <0x20 or (>0x7E and <91)
		5. 锟斤拷 bMsgChar 锟斤拷锟斤拷锟絪Byte16, (sByte16 = new String(bMsgChar))
		6. 锟斤拷 sByte16追锟接碉拷 sLog
		[注] 锟斤拷锟斤拷锟斤拷len锟斤拷锟斤拷16时锟斤拷注锟斤拷锟斤拷锟斤拷示时锟斤拷锟秸革拷Hex锟叫诧拷 (16-len)*3-1锟斤拷锟斤拷ASCII +16-len锟斤拷

     */
    public String Msg8583ToLogString(String sMsg) {
        String sLog = "Displacement -0--1--2--3--4--5--6-Hex-8--9--A--B--C--D--E--F  --ASCII Value---";
        String space = "                                                            ";
        byte[] bMessage, bMsg = {0x00}, bMsgChar;
        String sByte;
        int iStart = 0, iLen = 0, sLen = 0;
        int i = 0;
        String sTmp1, sTmp2;

        try {
            bMessage = sMsg.getBytes();
            sLen = bMessage.length;
            sLog = "Message length(byte) = " + sLen + "\n" + sLog;
            while (true) {
                //1. 确锟斤拷取锟斤拷锟斤拷位锟矫ｏ拷锟斤拷锟姐本锟斤拷 displacement 锟斤拷值锟斤拷追锟接碉拷 sLog
                sLog += "\n";
                sTmp1 = "0000" + iStart;
                sTmp2 = "0000" + (iStart / 16 * 10);
                sLog += sTmp1.substring(sTmp1.length() - 5) + "(" + sTmp2.substring(sTmp2.length() - 5) + ") ";

                //2. 锟斤拷确锟斤拷锟斤拷位锟斤拷取 sMsg 锟叫碉拷16锟斤拷锟街节ｏ拷转锟斤拷为 byte[]-bMsg
                try {
                    if ((iStart + 16) < sLen) {
                        bMsg = new byte[16];
                        for (i = 0; i < 16; i++) {
                            bMsg[i] = bMessage[iStart + i];
                        }
                    } else {
                        bMsg = new byte[sLen - iStart];
                        for (i = 0; i < (sLen - iStart); i++) {
                            bMsg[i] = bMessage[iStart + i];
                        }
                    }
                } catch (ArrayIndexOutOfBoundsException aie) {
                    System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
                    System.out.println("    Caught a ArrayIndexOutOfBoundsException-1!");
                    System.out.println("sLen=" + sLen + "    " + "iStart=" + iStart);
                } catch (StringIndexOutOfBoundsException sie) {
                    System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
                    System.out.println("    Caught a StringIndexOutOfBoundsException!-1");
                    System.out.println("sLen=" + sLen + "    " + "iStart=" + iStart);
                }

                iLen = bMsg.length;

                //3. 锟斤拷 bMsg 转锟斤拷为 16锟斤拷锟斤拷锟斤拷示锟斤拷锟街凤拷,	追锟接碉拷 sLog, 锟斤拷锟斤拷锟秸革拷
                sLog += Hex.toStringSpace(bMsg) + space.substring(0, (16 - iLen) * 3);

                //4. 锟斤拷 bMsg 锟叫碉拷锟斤拷锟矫匡拷锟斤拷纸诟锟斤拷频锟�bMsgChar, 锟斤拷锟斤拷锟叫的诧拷锟缴硷拷锟街凤拷转为0x2A("*")
                bMsgChar = new byte[bMsg.length];
                for (i = 0; i < bMsg.length; i++) {
                    if (bMsg[i] < 0x20 || (bMsg[i] > 0x7E && bMsg[i] < 0x91) || (bMsg[i] > 0x92 && bMsg[i] < 0xA1)) {
                        bMsgChar[i] = 0x2A;
                    } else {
                        bMsgChar[i] = bMsg[i];
                    }
                }

                //5. 锟斤拷 bMsgChar 锟斤拷锟斤拷锟絪Byte16, (sByte16 = new String(bMsgChar))
                sByte = new String(bMsgChar);
                //6. 锟斤拷 sByte追锟接碉拷 sLog
                sLog += " " + sByte + space.substring(0, 16 - sByte.length());

                iStart = iStart + 16;
                if (iStart >= sLen) {
                    break;
                }
            }
        } catch (StringIndexOutOfBoundsException sioe) {
            System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
            System.out.println("    Caught a StringIndexOutOfBoundsException!-2");
            System.out.println("sLen=" + sLen + "    " + "iStart=" + iStart);
        } catch (ArrayIndexOutOfBoundsException aie) {
            System.out.println("[bull.tools.Tools.java] Method <Msg8583ToLogString>: ");
            System.out.println("    Caught a ArrayIndexOutOfBoundsException!-2");
            System.out.println("sLen=" + sLen + "    " + "iStart=" + iStart);
        }
        //System.out.println("8583 Message String: \n"+ sLog);
        return sLog;
    }


    /*
	//锟斤拷锟街达拷锟斤拷GBK转锟斤拷默锟斤拷锟斤拷锟斤拷(锟斤拷锟斤拷转锟斤拷 Oracle锟斤拷 select 锟斤拷锟�
	public String strGBK_Default(String str)
	{
		try{
			return ( new String(str.getBytes("GBK") ) );
		}catch(Exception e){ }
		return "NULL";
	}

	//锟斤拷锟街达拷锟斤拷GBK转锟斤拷8859_1(锟斤拷锟斤拷转锟斤拷 Oracle锟斤拷 select 锟斤拷锟�
	public String strGBK_8859(String str)
	{
		try{
			return ( new String(str.getBytes("GBK"), "8859_1" ) );
		}catch(Exception e){ }
		return "NULL";
	}

    //锟斤拷8859_1锟斤拷锟街达拷转锟斤拷为 GBK 锟街达拷(锟斤拷锟斤拷转锟斤拷 Oracle锟斤拷 insert, update, delete锟斤拷锟�
	public String str8859_GBK(String str)
	{
		try{
			return ( new String(str.getBytes("8859_1"), "GBK") );
		}catch(Exception e){ }
		return "NULL";
	}
     */
    //锟斤拷转锟诫，直锟接凤拷锟斤拷源锟斤拷
    public String strGBK_Default(String str) {
        return str;
    }

    public String strGBK_8859(String str) {
        return str;
    }

    public String str8859_GBK(String str) {
        return str;
    }

    /*
		MethodName:		userLocale
		Description:	Return user's locale info follow the Language info.
		Input:
				String lang
							The value should be :
							CHINA
							ENGLISH
							FRENCH
							GERMAN
							JAPANESE
							KOREAN
		Output:
				Locale locale
     */
    public Locale userLocale(String lang) {
        Locale locale = null;

        if (lang.equalsIgnoreCase("ENGLISH") || lang.equalsIgnoreCase("EN")) {
            locale = Locale.ENGLISH;
        } else if (lang.equalsIgnoreCase("CHINA") || lang.equalsIgnoreCase("CN")) {
            locale = Locale.CHINA;
        } else if (lang.equalsIgnoreCase("TAIWAN") || lang.equalsIgnoreCase("TW")) {
            locale = Locale.TAIWAN;
        } else if (lang.equalsIgnoreCase("FRENCH") || lang.equalsIgnoreCase("FR")) {
            locale = Locale.FRENCH;
        } else if (lang.equalsIgnoreCase("ARABIC") || lang.equalsIgnoreCase("AR")) {
            locale = new Locale("ar", "SA");
        } else if (lang.equalsIgnoreCase("GERMAN") || lang.equalsIgnoreCase("DE")) {
            locale = Locale.GERMAN;
        } else if (lang.equalsIgnoreCase("JAPANESE") || lang.equalsIgnoreCase("JA")) {
            locale = Locale.JAPANESE;
        } else if (lang.equalsIgnoreCase("KOREAN") || lang.equalsIgnoreCase("KO")) {
            locale = Locale.KOREAN;
        } else {
            locale = Locale.ENGLISH;
        }

        System.out.println("Current Locale: [" + locale + "]");
        return locale;
    }

    /*
		MethodName:		multiLangPrint
		Description:	get Value according to the key from the ResourceBundle file.
		Input:
				String lang
							The value should be :
							CHINA
							ENGLISH
							FRENCH
							GERMAN
							JAPANESE
							KOREAN
				String rbBase
							ResourceBundle file's classpath and filename.
							example:
								resource.page.common
							it will read resouce from "resource.page.common_??.properties,
							?? base on your "lang".
				String key
							the key's name that the value you want to get.
     */
    public String multiLangPrint(String lang,
            String rbBase,
            String key) {
        Locale locale = userLocale(lang);
        ResourceBundle rb = ResourceBundle.getBundle(rbBase, locale);

        return initVar(rb.getString(key));
    }
}
