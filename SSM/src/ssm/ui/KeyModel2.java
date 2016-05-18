package ssm.ui;
// File: BranchModel.java

import java.sql.*;
import javax.swing.event.EventListenerList;

import ssm.jdbc.MvbOracleConnection;
import ssm.util.KeyBean;
import ssm.util.KeyTree2;
import ssm.util.KeyException;

public class KeyModel2 {
	protected PreparedStatement ps = null;
	protected EventListenerList listenerList = new EventListenerList();
	protected Connection con = null;

	protected KeyTree2 keyTree;

	public KeyModel2() {
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

	public boolean updateKey(String entitiyid, String keytype, String kev, String kcv) {
		try {
			ps = con.prepareStatement("UPDATE `keys` SET `kev` = ?, `kcv` = ? WHERE `entityid` = ? and `keytype` = ?");

			if (kev != null && kcv != null) {
				ps.setString(1, kev);
				ps.setString(2, kcv);
			} else {
				return false;
			}

			ps.setString(3, entitiyid);
			ps.setString(4, keytype);

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

			ps.executeQuery();

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

	public KeyTree2 showKeys(int i) {
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

	public KeyTree2 loadKeys() {
		try {
			keyTree = KeyTree2.getInstance();
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
