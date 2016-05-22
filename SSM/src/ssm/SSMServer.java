package ssm;

import ssm.tools.LogUtil;
import ssm.ui.KeyModel;
import ssm.util.BadCmdException;
import ssm.util.CmdBean;
import ssm.util.KeyBean;
import ssm.util.KeyException;
import ssm.crypto.Des;
import ssm.jdbc.MvbOracleConnection;

import java.net.*;

import java.io.*;

public class SSMServer extends Thread {

	public static final String CONFIGFILENAME = "config/ssm.properties";

	Socket socket = null;
	int port;

	int iPort;
	ServerSocket lsn_sock = null;
	Socket sock;

	// LogUtil.setLogFile("log/ssm");
	public SSMServer(int port) {
		this.port = port;
	}

	MvbOracleConnection mvb = MvbOracleConnection.getInstance();

	/**
	 * A private thread to handle capitalization requests on a particular
	 * socket. The client terminates the dialogue by sending a single line
	 * containing only a period.
	 */
	private static class Capitalizer extends Thread {
		private Socket socket;
		private int clientNumber;

		public Capitalizer(Socket socket, int clientNumber) {
			this.socket = socket;
			this.clientNumber = clientNumber;
			log("New connection with client# " + clientNumber + " at " + socket);
		}

		public void run() {
			KeyModel keyModel = new KeyModel();

			try {
				final int MAX_LENGTH = 128;
				int iLength;

				byte[] byin = new byte[MAX_LENGTH];
				InputStream is = socket.getInputStream();
				OutputStream out = socket.getOutputStream();

				LogUtil.writeLog("[info] Server accept completd! ");
				LogUtil.writeLog("[info] Begin read! ");

				for (;;) {
					iLength = is.read(byin, 0, MAX_LENGTH);
					if (iLength <= 0) {
						LogUtil.writeLog("[fatal] read socket returns [" + iLength + "], close socket and return");
						socket.close();
						break;
					}
					LogUtil.writeLog("[info] Received message [" + iLength + "] bytes", new String(byin, 0, iLength));

					CmdBean cmdBean = new CmdBean(new String(byin, 0 , iLength));

					KeyBean kb = null;
					switch (cmdBean.getCmd()) {
					case CmdBean.INIT_TMK:
						keyModel.InitTMK(cmdBean.getATMID());
						KeyBean TMK = keyModel.getKey("99999999", "TMK");
						kb  = keyModel.getKey(cmdBean.getATMID(), "TMK");
						if (kb != null) {
							cmdBean.setData(Des.Enc2(TMK.getKev(), kb.getKev()) + kb.getKcv());
							cmdBean.setErrorCode("00");
						}
						break;
					case CmdBean.EXCH_TXK:
						KeyBean currentTMK = null;
						switch (cmdBean.getMode()) {
						case "0":
							currentTMK = keyModel.ExchTMK(cmdBean.getATMID());
							kb = keyModel.getKey(cmdBean.getATMID(), "TMK");
							break;
						case "1":
							currentTMK = keyModel.ExchTPK(cmdBean.getATMID());
							kb = keyModel.getKey(cmdBean.getATMID(), "TPK");
							break;
						}
						if (kb != null) {
							cmdBean.setData(Des.Enc2(currentTMK.getKev(), kb.getKev()) + kb.getKcv());
							cmdBean.setData(kb.getKev() + kb.getKcv());
							cmdBean.setErrorCode("00");
						}
						break;
					case CmdBean.TRNS_PIN:
						break;
					default:
						LogUtil.writeLog("[fail] Invalid cmd: [" + cmdBean.getCmd() + "]");
						cmdBean.setErrorCode(CmdBean.EC_UNKNOWN);
						break;
					}

					String rsp = cmdBean.getResponse();

					out.write(rsp.getBytes(), 0, rsp.length());
					out.flush();
					LogUtil.writeLog("SendOut message", rsp);
				}

			} catch (ArrayIndexOutOfBoundsException e) {
				LogUtil.writeLog("ArrayIndexOutOfBoundsException " + e.getMessage());
			} catch (BindException e) {
				LogUtil.writeLog("Server error: " + e.getMessage());
//				System.exit(0);
			} catch (IOException e) {
				LogUtil.writeLog("Server error1: " + e.getMessage());
				
			} catch (BadCmdException e) {
				LogUtil.writeLog("Bad cmd: " + e.getMessage());
			} catch (KeyException e) {
				LogUtil.writeLog("Bad key: " + e.getMessage());
			} finally {
				try {
					socket.close();
				} catch (IOException e) {
					LogUtil.writeLog("Server error: " + e.getMessage());
				}
				log("Connection with client# " + clientNumber + " closed");
			}
		}

		/**
		 * Logs a simple message. In this case we just write the message to the
		 * server applications standard output.
		 */
		private void log(String message) {
			System.out.println(message);
		}
	}

	public Socket getConn_sock() {
		return socket;
	}

	public void setSocket(Socket conn) {
		socket = conn;
	}

	@Override
	public void run() {
		LogUtil.writeLog("[info] Server Begin: ServerSocket on port:" + port);
		int clientNumber = 0;
		ServerSocket listener;
		while (running) {
			try {
				listener = new ServerSocket(port);
				try {
					while (true) {
						new Capitalizer(listener.accept(), clientNumber++).start();
					}
				} finally {
					listener.close();
				}
			} catch (IOException e) {
				LogUtil.d("", e.getMessage());
			}
		}

	}

	private volatile boolean running = true;

	public void terminate() {
		running = false;
	}

}
