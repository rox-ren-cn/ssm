/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ssm.util;

/**
 *
 * @author Rox
 */
public class CmdBean {

    public static final String INIT_TMK = "IM";
    public static final String EXCH_TXK = "XK";
    public static final String TRNS_PIN = "TP";

    public static final String EC_OK = "00";
    public static final String EC_BADTMK = "01";
    public static final String EC_BADTMK2 = "02";
    public static final String EC_BADTPK = "03";
    public static final String EC_BADZPK = "04";
    public static final String EC_BADPIN = "05";
    public static final String EC_BADMOD = "06";
    public static final String EC_UNKNOWN = "99";

    private String cmd;

    public void setCmd(String cmd) {
        this.cmd = cmd;
    }

    public void setATMID(String ATMID) {
        this.ATMID = ATMID;
    }

    public void setEntityID(String EntityID) {
        this.EntityID = EntityID;
    }

    public void setMode(String Mode) {
        this.Mode = Mode;
    }

    public void setPINBlock(String PINBlock) {
        this.PINBlock = PINBlock;
    }

    public void setErrorCode(String ErrorCode) {
        this.ErrorCode = ErrorCode;
    }

    public void setData(String Data) {
        this.Data = Data;
    }

    public String getCmd() {
        return cmd;
    }

    public String getATMID() {
        return ATMID;
    }

    public String getEntityID() {
        return EntityID;
    }

    public String getMode() {
        return Mode;
    }

    public String getPINBlock() {
        return PINBlock;
    }

    public String getErrorCode() {
        return ErrorCode;
    }

    public String getData() {
        return Data;
    }
    private String ATMID;
    private String EntityID;
    private String Mode;
    private String PINBlock;

    private String ErrorCode;
    private String Data;

    public int Unpack(byte[] buf, int len) {
        return 0;
    }

    public int Pack() {
        return 0;
    }
    private String s;

    String getPackedMsg() {
        return s;
    }
}
