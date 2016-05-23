package ssm.util;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.event.EventListenerList;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ssm.ui.ExceptionListener;

public class KeyTree {
	private static KeyTree instance = null;

	private Log log = LogFactory.getLog(KeyTree.class);

	public static KeyTree getInstance() {
		if (instance == null) {
			instance = new KeyTree();
		}
		return instance;
	}

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

	protected KeyTree() {
		ResetTree();
	}

	public void ResetTree() {
		isLMKInited = false;
		isTMKInited = false;
		LMK = null;
		TMK = null;
		TMK_ATM = new ArrayList<KeyBean>();
		TPK_ATM = new ArrayList<KeyBean>();
		ZPK = new ArrayList<KeyBean>();
	}

	public void BuildTree(ResultSet rs) throws SQLException, KeyException {
		// clear all before loading
		ResetTree();

		while (rs.next()) {
			KeyBean kb = new KeyBean(rs.getString(1), rs.getString(2), rs.getString(3), rs.getString(4));
			addChild(kb);
		}

		buildTree();
	}

	public KeyTree(KeyBean lmk) {
		this();
		setLMK(lmk);
	}

	public void setLMK(KeyBean lmk) {
		LMK = lmk;
		Des.setLMK(lmk.getKev());
		isLMKInited = true;
	}

	public void setTMK(KeyBean tmk) {
		TMK = tmk;
		isTMKInited = true;
	}

	public void addChild(KeyBean kb) throws KeyException {
		if (kb.isLMK()) {
			if (!isLMKInited) {
				setLMK(kb);
				log.info("KeyTree2: LMK is loaded. " + kb.getKcv());
			} else {
				throw new KeyException("LMK can't be overwritten!");
			}
		} else if (kb.isTMK()) {
			if (!isTMKInited) {
				setTMK(kb);
				log.info("KeyTree2: TMK is loaded. " + kb.getKcv());
			} else {
				throw new KeyException("TMK can be init only once!");
			}
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

		if (!Des.Enc(Des.BDKDec(LMK.getKev()), "0000000000000000").startsWith(LMK.getKcv()))
			throw new KeyException("LMK KCV Error!");

		root = new KeyTree3(LMK);

		if (isTMKInited) {
			if (!Des.Enc(Des.LMKDec(TMK.getKev()), "0000000000000000").startsWith(TMK.getKcv()))
				throw new KeyException("TMK KCV Error!");

			branchTMK = root.addChild(TMK);

			TMK_ATM.forEach((x) -> branchTMK.addChild(x));
			TPK_ATM.forEach((x) -> branchTMK.addChild(x));
		}

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
		KeyBean encryptedKey = null;
		if (!isLMKInited) {
			if (kb.isLMK())
				encryptedKey = new KeyBean(kb.getKid(), kb.getTyp(), Des.BDKEnc(kb.getKev()), kb.getKcv());
			else
				throw new KeyException("LMK isn't inited");
		} else {
			if (isLMKInited && kb.isTMK())
				throw new KeyException("TMK can't be overwritten!");
			if (!isLMKInited && kb.isATM())
				throw new KeyException("TMK isn't inited!");
			encryptedKey = new KeyBean(kb.getKid(), kb.getTyp(), Des.LMKEnc(kb.getKev()), kb.getKcv());
		}

		addChild(encryptedKey);
		buildTree();
		return encryptedKey;
	}

	public KeyBean updateKey(KeyBean kb) throws KeyException {
		// kb is clear key
		checkKcv(kb.getKev(), kb.getKcv());
		KeyBean encryptedKey = null;
		if (!isLMKInited) {
			if (kb.isLMK())
				encryptedKey = new KeyBean(kb.getKid(), kb.getTyp(), Des.BDKEnc(kb.getKev()), kb.getKcv());
			else
				throw new KeyException("LMK isn't inited");
		} else {
			if (isLMKInited && kb.isTMK())
				throw new KeyException("TMK can't be overwritten!");
			if (!isLMKInited && kb.isATM())
				throw new KeyException("TMK isn't inited!");
			encryptedKey = new KeyBean(kb.getKid(), kb.getTyp(), Des.LMKEnc(kb.getKev()), kb.getKcv());
		}

		addChild(encryptedKey);
		buildTree();
		return encryptedKey;
	}

	public void deleteKey() {

	}

	public void InitTMK(String atmID) {
		// Generate New Key, encrypted by TMK.INIT
		// If ATM is already has a TMK, then delete it.
	}
}
