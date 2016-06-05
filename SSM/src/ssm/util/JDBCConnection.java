package ssm.util;

import java.sql.*;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ssm.SSMServer;
import ssm.ui.SSMView;

public class JDBCConnection {
	private static JDBCConnection _conn = null;
	
	private static final Log log = LogFactory.getLog(SSMServer.class);
	
	protected Connection con = null;
	protected boolean driverLoaded = false;

	protected JDBCConnection() {
	}

	public static JDBCConnection getInstance() {
		if (_conn == null) {
			_conn = new JDBCConnection();
		}

		return _conn;
	}

	public boolean connect(String username, char[] password) throws SQLException {
		con = connect(username, String.valueOf(password));
		if (con == null)
			return false;

		SSMView.prop.setProperty("ssm.dbUsername", username);
		SSMView.prop.setProperty("ssm.dbPassword", String.valueOf(password));
		return true;
	}

	public Connection connect() throws SQLException {
		String user = SSMView.prop.getProperty("ssm.dbUsername");
		String passwd = SSMView.prop.getProperty("ssm.dbPassword");
		return connect(user, passwd);
	}
	
	public Connection connect(String username, String password) throws SQLException {
		try {
			String url = SSMView.prop.getProperty("ssm.jdbcURL");
			String driver = SSMView.prop.getProperty("ssm.jdbcDriver");
//			String user = SSMView.prop.getProperty("ssm.dbUsername");
//			String passwd = SSMView.prop.getProperty("ssm.dbPassword");

			if (!driverLoaded) {
				try {
					Class.forName(driver);
				} catch (ClassNotFoundException e) {
					log.debug("Load db driver: " + driver + " fail. " + e.getMessage());
					return null;
				}
//				DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
//				driverLoaded = true;
//				DriverManager.registerDriver(new com.mysql.jdbc.Driver());
				driverLoaded = true;
			}

			Connection conn = DriverManager.getConnection(url, username, password);
			conn.setAutoCommit(false);

			return conn;
		} catch (SQLException ex) {
			log.debug("Connect DB:" + ex.getMessage());
			throw ex;
		}
	}

	public Connection getConnection() {
		return con;
	}


	protected void finalize() throws Throwable {
		if (con != null) {
			con.close();
		}

		super.finalize();
	}
}
