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

	public boolean connect() {
		con = connect("", "");
		if (con == null)
			return false;

		return true;
	}

	public Connection connect(String username, String password) {
		try {
			String url = SSMView.prop.getProperty("ssm.jdbcURL");
			String driver = SSMView.prop.getProperty("ssm.jdbcDriver");
			String user = SSMView.prop.getProperty("ssm.dbUsername");
			String passwd = SSMView.prop.getProperty("ssm.dbPassword");

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

			Connection conn = DriverManager.getConnection(url, user, passwd);
			conn.setAutoCommit(false);

			return conn;
		} catch (SQLException ex) {
			log.debug("Connect DB:" + ex.getMessage());
			return null;
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
