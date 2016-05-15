package ssm.ui;
// File: BranchModel.java

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import javax.swing.event.EventListenerList;

import ssm.jdbc.MvbOracleConnection;
import ssm.util.KeyBean;
import ssm.util.KeyTree;

public class KeyModel {
	protected PreparedStatement ps = null;
	protected EventListenerList listenerList = new EventListenerList();
	protected Connection con = null;

	protected String LMK = null;

	public KeyModel() {
		con = MvbOracleConnection.getInstance().getConnection();
	}

	public boolean insertKey(KeyBean key) {
		try {
			ps = con.prepareStatement("INSERT INTO `keys` VALUES (?,?,?,?)");

			ps.setString(1, key.getKid());
			ps.setString(2, key.getTyp());
			ps.setString(3, key.getKev());
			ps.setString(4, key.getKcv());

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

	public ResultSet showKeys() {
		try {
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);

			ResultSet rs = ps.executeQuery();

			return rs;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public KeyTree showKeys(int i) {
		try {
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);

			ResultSet rs = ps.executeQuery();

			KeyTree kt = BuildTree(rs);
			return kt;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public KeyTree initKeys() {
		try {
			ps = con.prepareStatement("SELECT k.* FROM `keys` k", ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);

			ResultSet rs = ps.executeQuery();

			KeyTree kt = BuildTree(rs);
			return kt;
		} catch (SQLException ex) {
			ExceptionEvent event = new ExceptionEvent(this, ex.getMessage());
			fireExceptionGenerated(event);
			return null;
		}
	}

	public KeyTree BuildTree(ResultSet rs) throws SQLException {
		KeyBean lmk = null, tmk = null;
		List<KeyBean> lvl1 = new ArrayList<KeyBean>();
		List<KeyBean> lvl2 = new ArrayList<KeyBean>();
		while (rs.next()) {
			KeyBean kb = new KeyBean(rs.getString(1), rs.getString(2), rs.getString(3), rs.getString(4));
			if (kb.isLMK())
				lmk = kb;
			else if (kb.isTMK())
				tmk = kb;
			else if (kb.isATM())
				lvl2.add(kb);
			else
				lvl1.add(kb);
		}
		if (lmk == null)
			return null;
		KeyTree kt = new KeyTree(lmk.Secure());
		if (tmk != null) {
			kt.addChild(tmk.Secure());
		}
		for (int i = 0; i < lvl1.size(); ++i) {
			kt.addChild(lvl1.get(i).Secure());
		}
		if (tmk != null)
			for (int i = 0; i < lvl2.size(); ++i) {
				kt.addChild(lvl2.get(i).Secure());
			}
		return kt;
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
