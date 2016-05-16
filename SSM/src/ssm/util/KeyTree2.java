package ssm.util;

import java.util.ArrayList;
import java.util.List;

import javax.swing.event.EventListenerList;

import ssm.crypto.Des;
import ssm.ui.ExceptionListener;

public class KeyTree2 {

	KeyTree3 root, branchTMK;
	protected EventListenerList listenerList = new EventListenerList();

	public void addExceptionListener(ExceptionListener l) {
		listenerList.add(ExceptionListener.class, l);
	}

	private KeyBean LMK;
	private KeyBean TMK;
	private List<KeyBean> TMK_ATM;
	private List<KeyBean> TPK_ATM;
	private List<KeyBean> ZPK;
	
	private boolean isLMKInited = false;
	private boolean isTMKInited = false;

	public KeyTree2() {
		isLMKInited = false;
		isTMKInited = false;
		LMK = null;
		TMK = null;
		TMK_ATM = new ArrayList<KeyBean>();
		TPK_ATM = new ArrayList<KeyBean>();
		ZPK = new ArrayList<KeyBean>();
	}

	public KeyTree2(KeyBean lmk) {
		this();
		setLMK(lmk);
	}

	public void setLMK(KeyBean lmk) {
		LMK = lmk;
		isLMKInited = true;
	}

	public void setTMK(KeyBean tmk) {
		TMK = tmk;
		isTMKInited = true;
	}
	
	public void addChild(KeyBean kb) {
		if (kb.isLMK()) {
			if (!isLMKInited) {
				setLMK(kb);
			} else {
				// LMK can't be overwirtten!
			}
		} else if (kb.isTMK()) {
			setTMK(kb);
		} else if (kb.isATM()) {
			TMK_ATM.add(kb);
		} else if (kb.isTPK()) {
			TPK_ATM.add(kb);
		} else {
			ZPK.add(kb);
		}

		return;
	}

	public KeyBean getLMK() {
		return LMK;
	}

	public KeyTree3 getTree() {
		return root;
	}

	public void buildTree() throws KeyException {
		if (!isLMKInited)
			throw new KeyException("LMK isn't inited");
		root = new KeyTree3(LMK);
		
		if (!isTMKInited)
			throw new KeyException("TMK isn't inited");
		
		branchTMK = root.addChild(TMK);

		TMK_ATM.forEach((x) -> branchTMK.addChild(x));
		TPK_ATM.forEach((x) -> branchTMK.addChild(x));

		ZPK.forEach((x) -> root.addChild(x));

		return;
	}

	public void checkKcv(String key, String kcv) throws KeyException {
		String KCV = Des.Enc(key, "0000000000000000");
		if (!KCV.startsWith(kcv))
			throw new KeyException("KCV is not correct!");
	}
	public KeyBean insertKey(KeyBean kb) throws KeyException {
		// kb is clear key
		checkKcv(kb.getKev(), kb.getKcv());
		if (!isLMKInited && kb.isLMK())
			return new KeyBean(kb.getKid(), kb.getTyp(), Des.EncLMK(kb.getKev()), kb.getKcv());
		else if (!isLMKInited)
			throw new KeyException("LMK isn't inited");
		else
			return new KeyBean(kb.getKid(), kb.getTyp(), Des.Enc0(LMK.getKev(), kb.getKev()), kb.getKcv());
	}
}
