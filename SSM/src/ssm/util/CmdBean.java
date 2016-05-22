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

	public static final String LOCALENTITY = "99999999";

	private String Command;

	public void setCmd(String cmd) {
		this.Command = cmd;
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
		return Command;
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

	private final static int CMD_LEN = 2;
	private final static int ENTITYID_LEN = 8;
	private final static int MODE_LEN = 1;
	private final static int PINBLOCK_LEN = 32;

	public CmdBean(String data) throws BadCmdException {
		if (data.length() < 5)
			throw new BadCmdException("Invalid length");
		int offset = 3; // Three bytes fixed header;
		Command = data.substring(offset, offset + CMD_LEN);
		offset += CMD_LEN;

		switch (Command) {
		case INIT_TMK:
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;
			break;
		case EXCH_TXK:
			Mode = data.substring(offset, offset + MODE_LEN);
			offset += MODE_LEN;

			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;
			break;
		case TRNS_PIN:
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;

			EntityID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;

			PINBlock = data.substring(offset, offset + PINBLOCK_LEN);
			offset += ENTITYID_LEN;
			break;
		default:
			throw new BadCmdException("Invalid cmd" + Command);
		}

		CheckInput();

		if (data.length() != offset)
			throw new BadCmdException(
					"Extra bytes following cmd:" + Command + " expecting length:" + offset + ", actual length:" + data.length());
	}

	void CheckInput() throws BadCmdException {
		if (LOCALENTITY.equals(ATMID))
			throw new BadCmdException("Bad ATM ID, it can't be 99999999");

		if (LOCALENTITY.equals(EntityID))
			throw new BadCmdException("Bad Entity ID, it can't be 99999999");

		if (ATMID != null && ATMID.length() != ENTITYID_LEN)
			throw new BadCmdException("Bad ATM ID length, it shall be 8");
	
		if (EntityID != null && EntityID.length() != ENTITYID_LEN)
			throw new BadCmdException("Bad Entity ID length, it shall be 8");

		if (Mode != null && Mode.length() != ENTITYID_LEN)
			throw new BadCmdException("Bad Mode length, it shall be 8");

		if (PINBlock != null && PINBlock.length() != ENTITYID_LEN)
			throw new BadCmdException("Bad PINBlock, it shall be 8");
	}

	public int Unpack(byte[] buf, int len) {
		return 0;
	}

	public int Pack() {
		return 0;
	}

	public String getResponse() {
		String rsp = "";
		switch (Command) {
		case INIT_TMK:
			rsp = "SSM" + INIT_TMK.toLowerCase() + ErrorCode + Data;
			break;
		case EXCH_TXK:
			rsp = "SSM" + EXCH_TXK.toLowerCase() + ErrorCode + Data;
			break;
		case TRNS_PIN:
			rsp = "SSM" + TRNS_PIN.toLowerCase() + ErrorCode + Data;
			break;
		}
		return rsp;
	}
}
