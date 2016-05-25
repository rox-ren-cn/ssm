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
	public static final String CLR_PIN = "CP";

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

	public void setPan(String Pan) {
		this.Pan = Pan;
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

	public String getPan() {
		return Pan;
	}

	private String ATMID;
	private String EntityID;
	private String Mode;
	private String PINBlock;
	private String Pan;

	private String ErrorCode;
	private String Data;

	private final static int CMD_LEN = 2;
	private final static int ENTITYID_LEN = 8;
	private final static int MODE_LEN = 1;
	private final static int PINBLOCK_LEN = 32;
	private final static int PAN_LEN = 19;

	public CmdBean(String data) throws CmdException {
		if (data.length() < 5)
			throw new CmdException("Invalid length");
		int offset = 3; // Three bytes fixed header;
		Command = data.substring(offset, offset + CMD_LEN);
		offset += CMD_LEN;

		switch (Command) {
		case INIT_TMK:
			if (data.length() < offset + ENTITYID_LEN)
				throw new CmdException("Invalid ATM ID Length");
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;
			break;
		case EXCH_TXK:
			if (data.length() < offset + MODE_LEN)
				throw new CmdException("Invalid MODE Length");
			Mode = data.substring(offset, offset + MODE_LEN);
			offset += MODE_LEN;

			if (data.length() < offset + ENTITYID_LEN)
				throw new CmdException("Invalid ATM ID Length");
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;
			break;
		case TRNS_PIN:
			if (data.length() < offset + ENTITYID_LEN)
				throw new CmdException("Invalid ATM ID Length");
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;

			if (data.length() < offset + ENTITYID_LEN)
				throw new CmdException("Invalid ZPK Entity ID Length");
			EntityID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;

			if (data.length() < offset + PINBLOCK_LEN)
				throw new CmdException("Invalid PIN Block Length");
			PINBlock = data.substring(offset, offset + PINBLOCK_LEN);
			offset += PINBLOCK_LEN;
			break;
		case CLR_PIN:
			if (data.length() < offset + ENTITYID_LEN)
				throw new CmdException("Invalid ATM ID Length");
			ATMID = data.substring(offset, offset + ENTITYID_LEN);
			offset += ENTITYID_LEN;

			if (data.length() < offset + PAN_LEN)
				throw new CmdException("Invalid PAN Length");
			Pan = data.substring(offset, offset + PAN_LEN);
			offset += PAN_LEN;

			if (data.length() < offset + PINBLOCK_LEN)
				throw new CmdException("Invalid PIN Block Length");
			PINBlock = data.substring(offset, offset + PINBLOCK_LEN);
			offset += PINBLOCK_LEN;
			break;
		default:
			throw new CmdException("Invalid cmd" + Command);
		}

		CheckInput();

		if (data.length() != offset)
			throw new CmdException("Extra bytes following cmd:" + Command + " expecting length:" + offset
					+ ", actual length:" + data.length());
	}

	void CheckInput() throws CmdException {
		if (LOCALENTITY.equals(ATMID))
			throw new CmdException("Bad ATM ID, it can't be 99999999");

		if (LOCALENTITY.equals(EntityID))
			throw new CmdException("Bad Entity ID, it can't be 99999999");

		if (ATMID != null && ATMID.length() != ENTITYID_LEN)
			throw new CmdException("Bad ATM ID length, it shall be " + ENTITYID_LEN);

		if (EntityID != null && EntityID.length() != ENTITYID_LEN)
			throw new CmdException("Bad Entity ID length, it shall be " + ENTITYID_LEN);

		if (Mode != null && Mode.length() != MODE_LEN)
			throw new CmdException("Bad Mode length, it shall be " + MODE_LEN);

		if (PINBlock != null && PINBlock.length() != PINBLOCK_LEN)
			throw new CmdException("Bad PINBlock, it shall be " + PINBLOCK_LEN);

		if (Pan != null && Pan.length() != PAN_LEN)
			throw new CmdException("Bad PAN, it shall be " + PAN_LEN);
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
		case CLR_PIN:
			rsp = "SSM" + TRNS_PIN.toLowerCase() + ErrorCode + Data;
			break;
		}
		return rsp;
	}
}
