package ssm.ui;

import java.sql.*;
import javax.swing.event.EventListenerList;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ssm.util.CmdBean;
import ssm.util.DataException;
import ssm.util.Des;
import ssm.util.KeyBean;
import ssm.util.KeyTree;
import ssm.util.JDBCConnection;
import ssm.util.KeyException;

public class KeyModel {
	private static final Log log = LogFactory.getLog(KeyModel.class);

	protected PreparedStatement ps = null;
	protected EventListenerList listenerList = new EventListenerList();
	protected Connection con = null;

	protected static KeyTree keyTree;

	public KeyModel() {
		con = JDBCConnection.getInstance().getConnection();
	}

	public KeyModel(int i) throws SQLException {
		con = JDBCConnection.getInstance().connect();
	}

	public boolean insertKey(KeyBean clearKey) {
		try {
			KeyBean encryptedKey = keyTree.insertKey(clearKey);
			ps = con.prepareStatement("INSERT INTO keys VALUES (?,?,?,?)");

			ps.setString(1, encryptedKey.getKid());
			ps.setString(2, encryptedKey.getTyp());
			ps.setString(3, encryptedKey.getKev());
			ps.setString(4, encryptedKey.getKcv());

			ps.executeUpdate();

			con.commit();

			return true;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);

			try {
				con.rollback();
				return false;
			} catch (SQLException ex2) {
				event = new ExceptionEvent(this, ex2.getMessage());
				fireExceptionGenerated(event);
				return false;
			}
		} catch (KeyException ex) {
			fireExceptionGenerated(new ExceptionEvent(this, ex.getMessage()));
			return false;
		}
	}

	public boolean updateKey(KeyBean clearKey) throws KeyException {
		try {
			KeyBean encryptedKey = keyTree.updateKey(clearKey);
			ps = con.prepareStatement("UPDATE keys SET kev = ?, kcv = ? WHERE entityid = ? and keytype = ?");

			ps.setString(1, encryptedKey.getKev());
			ps.setString(2, encryptedKey.getKcv());

			ps.setString(3, clearKey.getKid());
			ps.setString(4, clearKey.getTyp());

			ps.executeUpdate();

			con.commit();

			return true;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);

			try {
				con.rollback();
				return false;
			} catch (SQLException ex2) {
				event = new ExceptionEvent(this, ex2.getMessage());
				fireExceptionGenerated(event);
				return false;
			}
		}
	}

	public boolean deleteKey(String entitiyid, String keytype) {
		return deleteKey(entitiyid, keytype, true);
	}

	// No commit
	public boolean deleteKey(String entitiyid, String keytype, boolean commit) {
		try {
			ps = con.prepareStatement("DELETE FROM keys WHERE entityid = ? and keytype = ?");

			ps.setString(1, entitiyid);
			ps.setString(2, keytype);

			ps.execute();

			if (commit)
				con.commit();

			return true;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);

			try {
				if (commit)
					con.rollback();
				return false;
			} catch (SQLException ex2) {
				event = new ExceptionEvent(this, ex2.getMessage());
				fireExceptionGenerated(event);
				return false;
			}
		}
	}

	public KeyTree showKeys(int i) {
		try {
			ps = con.prepareStatement("SELECT k.* FROM keys k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);

			ResultSet rs = ps.executeQuery();

			keyTree.BuildTree(rs);
			return keyTree;
		} catch (SQLException | KeyException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public KeyTree loadKeys() {
		try {
			keyTree = KeyTree.getInstance();
			ps = con.prepareStatement("SELECT k.* FROM keys k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);

			ResultSet rs = ps.executeQuery();

			keyTree.BuildTree(rs);
			return keyTree;
		} catch (SQLException | KeyException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public ResultSet editKey() {
		try {
			ps = con.prepareStatement("SELECT k.* FROM keys k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_UPDATABLE);

			ResultSet rs = ps.executeQuery();

			return rs;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public boolean findKey(String entitiyid, String keytype) {
		try {
			ps = con.prepareStatement("SELECT kcv FROM keys where entityid = ? and keytype = ?");

			ps.setString(1, entitiyid);
			ps.setString(2, keytype);

			ResultSet rs = ps.executeQuery();

			if (rs.next()) {
				return true;
			} else {
				return false;
			}
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);

			return false;
		}
	}

	public KeyBean getKey(String entitiyid, String keytype) {
		try {
			ps = con.prepareStatement("SELECT * FROM keys where entityid = ? and keytype = ?");

			ps.setString(1, entitiyid);
			ps.setString(2, keytype);

			ResultSet rs = ps.executeQuery();

			if (rs.next()) {
				return new KeyBean(rs.getString(1), rs.getString(2), rs.getString(3), rs.getString(4));
			} else {
				return null;
			}
		} catch (SQLException ex) {
			fireExceptionGenerated(new ExceptionEvent(this, ex.getMessage()));

			return null;
		}
	}

	public void InitTMK(String atmID) {
		// Generate New Key, encrypted by TMK.INIT
		// If ATM is already has a TMK, then delete it.
		if (findKey(atmID, "TMK")) {
			if (!deleteKey(atmID, "TMK", false)) {
				log.debug("Delete existing TMK failed");
				fireExceptionGenerated(new ExceptionEvent(this, "Delete existing TMK failed"));
			}

		}
		if (findKey(atmID, "TPK")) {
			if (!deleteKey(atmID, "TPK", false)) {
				log.debug("Delete existing TPK failed");
				fireExceptionGenerated(new ExceptionEvent(this, "Delete existing TPK failed"));
			}
		}

		String newKey = Des.randomKey();
		KeyBean kb = new KeyBean(atmID, "TMK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		if (!insertKey(kb)) {
			log.debug("InitTMK failed");
		}
	}

	public KeyBean ExchTMK(String atmID) throws KeyException {
		// Generate New Key, encrypted by current TMK
		KeyBean currentTMK = getKey(atmID, "TMK");
		if (currentTMK == null) {
			log.debug("ATM TMK isn't inited");
			fireExceptionGenerated(new ExceptionEvent(this, "ATM TMK isn't inited"));
			throw new KeyException("ATM TMK isn't inited");
		}

		String newKey = Des.randomKey();
		KeyBean newTMK = new KeyBean(atmID, "TMK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		updateKey(newTMK);

		return currentTMK;
	}

	public KeyBean ExchTPK(String atmID) throws KeyException {
		// Generate New Key, encrypted by current TPK
		KeyBean currentTMK = getKey(atmID, "TMK");
		if (currentTMK == null)
			throw new KeyException("ATM TMK isn't inited");
		String newKey = Des.randomKey();
		KeyBean newTMK = new KeyBean(atmID, "TPK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		if (findKey(atmID, "TPK"))
			updateKey(newTMK);
		else
			insertKey(newTMK);

		return currentTMK;
	}

	public String TransPIN(String atmID, String entityID, String PINBlock) throws KeyException {
		// Generate New Key, encrypted by current TMK
		KeyBean TPK = getKey(atmID, "TPK");
		if (TPK == null)
			throw new KeyException("ATM TPK doesn't exist");
		KeyBean ZPK = getKey(entityID, "ZPK");
		if (ZPK == null)
			throw new KeyException("ZPK doesn't exist for " + entityID);

		String clearPB = Des.Dec(Des.LMKDec(TPK.getKev()), PINBlock);
		return Des.Enc(Des.LMKDec(ZPK.getKev()), clearPB);
	}

	public void TransPIN(CmdBean cb) throws KeyException {
		// Generate New Key, encrypted by current TMK
		KeyBean TPK = getKey(cb.getATMID(), "TPK");
		if (TPK == null) {
			cb.setErrorCode("01");
			throw new KeyException("ATM TPK doesn't exist");
		}
		KeyBean ZPK = getKey(cb.getEntityID(), "ZPK");
		if (ZPK == null) {
			cb.setErrorCode("02");
			throw new KeyException("ZPK doesn't exist for " + cb.getEntityID());
		}

		String clearPB = Des.Dec(Des.LMKDec(TPK.getKev()), cb.getPINBlock());
		cb.setData(Des.Enc(Des.LMKDec(ZPK.getKev()), clearPB));
		cb.setErrorCode("00");
	}

	public String GetClearPIN(String atmID, String PINBlock) throws KeyException {
		// Generate New Key, encrypted by current TMK
		KeyBean TPK = getKey(atmID, "TPK");
		if (TPK == null)
			throw new KeyException("ATM TPK doesn't exist");

		String clearPB = Des.Dec(Des.LMKDec(TPK.getKev()), PINBlock);
		return clearPB;
	}

	public void GetClearPIN(CmdBean cb) throws KeyException, DataException {
		// Generate New Key, encrypted by current TMK
		KeyBean TPK = getKey(cb.getATMID(), "TPK");
		if (TPK == null) {
			cb.setErrorCode("01");
			throw new KeyException("ATM TPK doesn't exist");
		}

		try {
			cb.setData(Des.GetPin(cb.getPan(), Des.Dec(Des.LMKDec(TPK.getKev()), cb.getPINBlock())));
		} catch (DataException e) {
			cb.setErrorCode("02");
			throw e;
		} catch (KeyException e) {
			cb.setErrorCode("01");
			throw e;
		}
		cb.setErrorCode("00");
		return;
	}

	public Connection getConnection() {
		return con;
	}

	protected void finalize() throws Throwable {
		if (ps != null) {
			ps.close();
		}

		super.finalize();
	}

	public void addExceptionListener(ExceptionListener l) {
		listenerList.add(ExceptionListener.class, l);
	}

	public void removeExceptionListener(ExceptionListener l) {
		listenerList.remove(ExceptionListener.class, l);
	}

	public void fireExceptionGenerated(ExceptionEvent ex) {
		Object[] listeners = listenerList.getListenerList();

		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == ExceptionListener.class) {
				((ExceptionListener) listeners[i + 1]).exceptionGenerated(ex);
			}
		}
	}
}
