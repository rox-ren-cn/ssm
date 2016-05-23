package ssm.util;

public class KeyBean {
	private String kid;
	private String typ;
	private String kev;
	private String kcv;

	public KeyBean(KeyBean kb) {
		this(kb.getKid(), kb.getTyp(), kb.getKev(), kb.getKcv());
	}
	
	public KeyBean(String kid, String typ, String kev, String kcv) {
		setKid(kid);
		setTyp(typ);
		setKev(kev);
		setKcv(kcv);
	}

	public KeyBean Secure() {
		if (kev != null)
			if (isLMK())
				setKev(Des.BDKEnc(kev));
			else
				setKev(Des.LMKEnc(kev));
		return this;
	}

	public String getKid() {
		return kid;
	}

	public void setKid(String kid) {
		this.kid = kid;
	}

	public String getTyp() {
		return typ;
	}

	public void setTyp(String typ) {
		this.typ = typ;
	}

	public String getKev() {
		return kev;
	}

	public void setKev(String kev) {
		this.kev = kev;
	}

	public String getKcv() {
		return kcv;
	}

	public void setKcv(String kcv) {
		this.kcv = kcv;
	}

	public boolean isLMK() {
		return kid.equals("99999999") && typ.equals("LMK");
	}

	public boolean isTMK() {
		return kid.equals("99999999") && typ.equals("TMK");
	}

	public boolean isATM() {
		return /* typ.equals("TPK") || */ (!kid.equalsIgnoreCase("99999999") && typ.equals("TMK"));
	}

	public boolean isTPK() {
		return typ.equals("TPK");
	}

	public boolean isZPK() {
		return typ.equals("ZPK");
	}

	public String getKeyType() {
		if (isLMK())
			return "LMK";
		else if (isTMK())
			return "TMK";
		else if (isATM())
			return "ATM";
		else if (isZPK())
			return "ZPK";

		return "N/A";
	}

}
