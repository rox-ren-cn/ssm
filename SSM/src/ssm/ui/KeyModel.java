package ssm.ui;
// File: BranchModel.java

import java.sql.*;
import javax.swing.event.EventListenerList;

import ssm.crypto.Des;
import ssm.jdbc.MvbOracleConnection;
import ssm.util.KeyBean;
import ssm.util.KeyTree;
import ssm.util.KeyException;

public class KeyModel {
	protected PreparedStatement ps = null;
	protected EventListenerList listenerList = new EventListenerList();
	protected Connection con = null;
	// connection for server
	protected Connection con0 = null;

	protected static KeyTree keyTree;

	public KeyModel() {
		con = MvbOracleConnection.getInstance().getConnection();
	}

	public boolean insertKey(KeyBean clearKey) {
		try {
			KeyBean encryptedKey = keyTree.insertKey(clearKey);
			ps = con.prepareStatement("INSERT INTO `keys` VALUES (?,?,?,?)");

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

	public boolean updateKey(String kid, String typ, String kev, String kcv) throws KeyException {
		try {
			KeyBean encryptedKey = keyTree.updateKey(new KeyBean(kid, typ, kev, kcv));
			ps = con.prepareStatement("UPDATE `keys` SET `kev` = ?, `kcv` = ? WHERE `entityid` = ? and `keytype` = ?");

			if (kev != null && kcv != null) {
				ps.setString(1, encryptedKey.getKev());
				ps.setString(2, encryptedKey.getKcv());
			} else {
				return false;
			}

			ps.setString(3, kid);
			ps.setString(4, typ);

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
		try {
			ps = con.prepareStatement("DELETE FROM `keys` WHERE `entityid` = ? and `keytype` = ?");

			ps.setString(1, entitiyid);
			ps.setString(2, keytype);

			ps.execute();

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

	public KeyTree showKeys(int i) {
		try {
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
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
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
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
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_UPDATABLE);

			ResultSet rs = ps.executeQuery();

			return rs;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event); // no need to commit or rollback
											// since it is only a query

			return null;
		}
	}

	public boolean findKey(String entitiyid, String keytype) {
		try {
			ps = con.prepareStatement("SELECT kcv FROM `keys` where `entityid` = ? and `keytype` = ?");

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
			ps = con.prepareStatement("SELECT * FROM `keys` where `entityid` = ? and `keytype` = ?");

			ps.setString(1, entitiyid);
			ps.setString(2, keytype);

			ResultSet rs = ps.executeQuery();

			if (rs.next()) {
				return new KeyBean(rs.getString(1), rs.getString(2), rs.getString(3), rs.getString(4));
			} else {
				return null;
			}
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);

			return null;
		}
	}

	public void InitTMK(String atmID) {
		// Generate New Key, encrypted by TMK.INIT
		// If ATM is already has a TMK, then delete it.
		if (findKey(atmID, "TMK")) {
			deleteKey(atmID, "TMK");
		}
		if (findKey(atmID, "TPK")) {
			deleteKey(atmID, "TPK");
		}

		String newKey = Des.randomKey();
		KeyBean kb = new KeyBean(atmID, "TMK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		insertKey(kb);
	}

	public KeyBean ExchTMK(String atmID) throws KeyException {
		// Generate New Key, encrypted by current TMK
		// If ATM is already has a TMK, then delete it.
		KeyBean currentTMK = getKey(atmID, "TMK");
		if (findKey(atmID, "TMK")) {
			deleteKey(atmID, "TMK");
		} else {
			throw new KeyException("ATM TMK isn't inited");
		}

		String newKey = Des.randomKey();
		KeyBean newTMK = new KeyBean(atmID, "TMK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		updateKey(newTMK.getKcv(), newTMK.getTyp(), newTMK.getKev(), newTMK.getKcv());
		
		return currentTMK;
	}

	public KeyBean ExchTPK(String atmID) throws KeyException {
		// Generate New Key, encrypted by current TMK
		// If ATM is already has a TPK, then delete it.
		KeyBean currentTMK = getKey(atmID, "TPK");
		if (findKey(atmID, "TPK")) {
			deleteKey(atmID, "TPK");
		}

		String newKey = Des.randomKey();
		KeyBean newTMK = new KeyBean(atmID, "TPK", newKey, Des.Enc(newKey, "0000000000000000").substring(0, 6));
		updateKey(newTMK.getKcv(), newTMK.getTyp(), newTMK.getKev(), newTMK.getKcv());
		
		return currentTMK;
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
