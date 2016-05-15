/*
 *Source:       JDBCAdapter.java
 *Description:  Establish JDBC database connection;
 *              Access table by SQL;
 */
package ssm.jdbc;

import java.sql.*;
import ssm.tools.PubTools;

public class JDBCAdapter {

    String sDBDriver = "";
    String sConnStr = "";
    String sUser = "";
    String sPswd = "";
    String sHint = "";
    ResultSet rs = null;
    Connection conn = null;
    Statement stmt = null;
    PubTools tool = new PubTools();

    public String getHint() {
        return sHint;
    }

    public void setDBDriver(String s) {
        sDBDriver = s;
    }

    public String getDBDriver() {
        return sDBDriver;
    }

    public String getConnStr() {
        return sConnStr;
    }

    public void setConnStr(String s) {
        sConnStr = s;
    }

    public ResultSet getResultSet() {
        return rs;
    }

    //Initialize the database driver
    public boolean loadDriver(String dbDriver, String dbConn, String dbUser, String dbPswd) {
        try {
            Class.forName(dbDriver);
            sHint = "Database driver load succeed!";
        } catch (java.lang.ClassNotFoundException e) {
            sHint = "Database driver load failed: ClassNotFound " + e.getMessage();
            return false;
        }
        try {
            conn = DriverManager.getConnection(dbConn, dbUser, dbPswd);
        } catch (java.sql.SQLException e) {
            sHint = "Database driver load failed: (" + sDBDriver + "," + sConnStr + "," + sUser + "," + sPswd + ") " + e.getMessage();
            return false;
        }
        return true;
    }

//execute a SQL statement with recordset returned 
    public ResultSet executeQuery(String sql) {
        try {
            if (stmt != null) {
                stmt.close();
            }
            if (rs != null) {
                rs.close();
            }
            rs = null;
            stmt = conn.createStatement();
            rs = stmt.executeQuery(sql);
        } catch (SQLException ex) {
            sHint = "Execute query failed: [" + sql + "] " + ex.getMessage();
        }
        return rs;
    }

    //execute a SQL statement without returning recordset 
    public boolean executeUpdate(String sql) {
        try {
            if (stmt != null) {
                stmt.close();
            }
            stmt = conn.createStatement();
            if (stmt.executeUpdate(sql) > 0) {
                return true;
            } else {
                sHint = "EexecuteUpdate: [" + sql + "] 0 row updated";
                return false;
            }
        } catch (SQLException ex) {
            sHint = "Execute update failed: [" + sql + "] " + ex.getMessage();
            return false;
        } finally {
            if (stmt != null) {
                try {
                    stmt.close();
                } catch (SQLException e) {
                    return false;
                }
            }
        }
    }

    //Close connections 
    public boolean closeConn() {
        try {
            if (rs != null) {
                rs.close();
            }
            if (stmt != null) {
                stmt.close();
            }
            if (conn != null) {
                conn.close();
            }
            return true;
        } catch (SQLException ex) {
            sHint = "Close connection failed: " + ex.getMessage();
            return false;
        }
    }
}
