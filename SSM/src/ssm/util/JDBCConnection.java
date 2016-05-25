package ssm.util;

// File: MvbOracleConnection.java

import java.sql.*;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ssm.SSMServer;
import ssm.ui.SSMView;

/*
 * This class is a singleton class that provides methods 
 * to connect to an Oracle database, return the connection, 
 * set the connection, and determine whether or not the Oracle
 * JDBC driver has been loaded. To obtain a reference to an
 * instance of this class, use the getInstance() method.
 */
public class JDBCConnection {
	private static JDBCConnection _mvb = null;
	
	private static final Log log = LogFactory.getLog(SSMServer.class);
	
	protected Connection con = null;
	protected boolean driverLoaded = false;

	/*
	 * The constructor is declared protected so that only subclasses can access
	 * it.
	 */
	protected JDBCConnection() {
		// empty
	}

	/*
	 * Returns an instance of MvbOracleConnection
	 */
	public static JDBCConnection getInstance() {
		if (_mvb == null) {
			_mvb = new JDBCConnection();
		}

		return _mvb;
	}

	public boolean connect() {
		con = connect("", "");
		if (con == null)
			return false;

		return true;
	}

	/*
	 * Loads the Oracle JDBC driver and connects to the database named ug using
	 * the given username and password. Returns true if the connection is
	 * successful; false otherwise.
	 */
	public Connection connect(String username, String password) {
		try {
			// change the url if the branch table is located somewhere else
			String url = SSMView.prop.getProperty("ssm.jdbsURL");
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

	/*
	 * Returns the connection
	 */
	public Connection getConnection() {
		return con;
	}

	/*
	 * This method allows members of this class to clean up after itself before
	 * it is garbage collected. It is called by the garbage collector.
	 */
	protected void finalize() throws Throwable {
		if (con != null) {
			con.close();
		}

		// finalize() must call super.finalize() as the last thing it does
		super.finalize();
	}
}