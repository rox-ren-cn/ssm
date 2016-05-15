package ssm;

import ssm.tools.LogUtil;
import ssm.util.CmdBean;
import ssm.jdbc.JDBCAdapter;
import java.net.*;

import java.io.*;


public class MyServer extends Thread {

    private static final String CONFIGFILENAME = "config/ssm.properties";

    JDBCAdapter jdbcadapter;
    Socket conn_sock = null;

    public Socket getConn_sock() {
        return conn_sock;
    }

    public void setConn_sock(Socket connSock) {
        conn_sock = connSock;
    }

    @Override
    public void run() {
        final int MAX_LENGTH = 4096;
        byte[] byin = new byte[MAX_LENGTH];
        byte[] byout;
        int iLength;
        String tranLog;

        CmdBean cmdBean = new CmdBean();
        DataInputStream dis = null;
        OutputStream out = null;
        InputStream is = null;
        try {
            LogUtil.writeLog("[info] Server accept completd! ");
            is = conn_sock.getInputStream();
            out = conn_sock.getOutputStream();
            LogUtil.writeLog("[info] Begin read! ");

            dis = new DataInputStream(is);
            for (;;) {
                iLength = dis.read(byin, 0, MAX_LENGTH);
                if (iLength <= 0) {
                    LogUtil.writeLog("[info] read socket returns [" + iLength + "],close socket and return");
                    conn_sock.close();
                    break;
                }
                String sTitle = "[info] Received message [" + iLength + "] bytes";
                tranLog = LogUtil.HexStringLog(byin, iLength);
                LogUtil.writeLog(sTitle, tranLog);

                /*
				 * unpack message
                 */
                iLength = cmdBean.Unpack(byin, iLength);
                if (iLength < 0) {
                    LogUtil.writeLog("[fail] Unpack failed!");
                    break;
                }

                /*
				 * judge transaction type according to MTI and Bit 3
                 */
                String cmd = cmdBean.getCmd();

                /*
				 * Transaction business proces
                 */
                switch (cmd) {
                    case CmdBean.INIT_TMK:
                        break;
                    case CmdBean.EXCH_TXK:
                        break;
                    case CmdBean.TRNS_PIN:
                        break;
                    default:
                        LogUtil.writeLog("[fail] Invalid cmd: [" + cmd + "]");
                        cmdBean.setErrorCode(CmdBean.EC_UNKNOWN);
                        break;
                }

                /*
		* Pack and Send response message back
                 */
                cmdBean.Pack();
                byout = cmdBean.getPackedMsg().getBytes("ISO-8859-1");
                iLength = cmdBean.getPackedMsg().length();

                out.write(byout, 0, iLength);
                tranLog = LogUtil.HexStringLog(byout, iLength);
                LogUtil.writeLog("SendOut message", tranLog);
            }

        } catch (ArrayIndexOutOfBoundsException e) {
            LogUtil.writeLog("ArrayIndexOutOfBoundsException " + e.getMessage());
        } catch (BindException e) {
            LogUtil.writeLog("Server error: " + e.getMessage());
            System.exit(0);
        } catch (Exception e) {
            LogUtil.writeLog("Server error: " + e);
            try {
                conn_sock.close();
            } catch (IOException e1) {
                LogUtil.writeLog("Server error: " + e1.getMessage());
            }
        } finally {

            try {
                if (dis != null) {
                    dis.close();
                }
                if (out != null) {
                    out.close();
                }
                if (is != null) {
                    is.close();
                }
                if (conn_sock != null) {
                    conn_sock.close();
                }
                jdbcadapter.closeConn();
            } catch (IOException e) {
                LogUtil.writeLog("Server error: " + e.getMessage());
            }
        }
    }

    @SuppressWarnings("resource")
    public static void main(String[] args) throws IOException {
        int iPort;
        ServerSocket lsn_sock;
        Socket sock;

        LogUtil.setLogFile("log/ssm");

        ssm.file.readPropertyFile rpf = new ssm.file.readPropertyFile();
        String sPort = rpf.readProperty(CONFIGFILENAME, "ssm.port");
        iPort = Integer.parseInt(sPort);
        LogUtil.writeLog("[info] Server Begin: ServerSocket on port:" + iPort);
        lsn_sock = new ServerSocket(iPort);

        while (true) {
            try {
                sock = lsn_sock.accept();
                MyServer threadMyServer = new MyServer();
                threadMyServer.connectDB();
                threadMyServer.setConn_sock(sock);
                threadMyServer.start();
            } catch (Exception e) {
                LogUtil.writeLog("Server error: " + e.getMessage());
            }
        }
    }

    public int connectDB() {
        jdbcadapter = new JDBCAdapter();

        ssm.file.readPropertyFile rpf = new ssm.file.readPropertyFile();
        String dbDriver = rpf.readProperty(CONFIGFILENAME, "ssm.jdbcDriver");
        String connURL = rpf.readProperty(CONFIGFILENAME, "ssm.jdbcURL");
        String dbUser = rpf.readProperty(CONFIGFILENAME, "ssm.dbUsername");
        String dbPasswd = rpf.readProperty(CONFIGFILENAME, "ssm.dbPassword");

        LogUtil.writeLog("Connect DB, config file: [config/ssm.properties] dbUser/dbPasswd: [" + dbUser + "/"
                + dbPasswd + "]");
        LogUtil.writeLog("Connect DB, connURL = " + connURL);

        if (jdbcadapter.loadDriver(dbDriver, connURL, dbUser, dbPasswd) != true) {
            LogUtil.writeLog("loadDriver error,user:" + dbUser + " dbPasswd:" + dbPasswd);
            LogUtil.writeLog(jdbcadapter.getHint());
            return -1;
        }

        String sSql = "ALTER SESSION SET NLS_DATE_FORMAT='YYYYMMDDHH24MISS'";
        jdbcadapter.executeUpdate(sSql);

        return 0;
    }

}
