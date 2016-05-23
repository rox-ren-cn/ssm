package ssm;

import ssm.ui.KeyModel;
import ssm.util.BadCmdException;
import ssm.util.CmdBean;
import ssm.util.Des;
import ssm.util.KeyBean;
import ssm.util.KeyException;
import ssm.util.MvbOracleConnection;

import java.net.*;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import java.io.*;

public class SSMServer extends Thread {

	private static final Log log = LogFactory.getLog(SSMServer.class);

	int port;

	int iPort;
	ServerSocket lsn_sock = null;
	Socket sock;

	public SSMServer(int port) {
		this.port = port;
	}

	MvbOracleConnection mvb = MvbOracleConnection.getInstance();

	private static class Capitalizer extends Thread {
		private Socket socket;
		private int clientNumber;

		private static final Log log = LogFactory.getLog(Capitalizer.class);
		
		public Capitalizer(Socket socket, int clientNumber) {
			this.socket = socket;
			this.clientNumber = clientNumber;
			log("New connection with client# " + clientNumber + " at " + socket);
		}

		public void run() {
			KeyModel keyModel = new KeyModel(1);

			Thread.currentThread().setName("SSM Server Worker " + clientNumber);
			
			try {
				final int MAX_LENGTH = 128;
				int iLength;

				byte[] byin = new byte[MAX_LENGTH];
				InputStream is = socket.getInputStream();
				OutputStream out = socket.getOutputStream();

				log.info("[info] Server accept completd! ");
				log.info("[info] Begin read! ");

				for (;;) {
					iLength = is.read(byin, 0, MAX_LENGTH);
					if (iLength <= 0) {
						log.fatal("read socket returns [" + iLength + "], close socket and return");
						socket.close();
						break;
					}
					log.info("[info] Received message [" + iLength + "] bytes " + new String(byin, 0, iLength));

					CmdBean cmdBean = null;
					try {
						cmdBean = new CmdBean(new String(byin, 0, iLength));
						KeyBean kb = null;
						switch (cmdBean.getCmd()) {
						case CmdBean.INIT_TMK:
							keyModel.InitTMK(cmdBean.getATMID());
							KeyBean TMK = keyModel.getKey("99999999", "TMK");
							kb = keyModel.getKey(cmdBean.getATMID(), "TMK");
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
							String s = keyModel.TransPIN(cmdBean.getATMID(), cmdBean.getEntityID(),
									cmdBean.getPINBlock());
							cmdBean.setData(s);
							cmdBean.setErrorCode("00");
							break;
						case CmdBean.CLR_PIN:
							String c = keyModel.GetClearPIN(cmdBean.getATMID(), cmdBean.getPINBlock());
							cmdBean.setData(Des.GetPin(cmdBean.getPan(), c));
							cmdBean.setErrorCode("00");
							break;
						default:
							log.fatal("[fail] Invalid cmd: [" + cmdBean.getCmd() + "]");
							cmdBean.setErrorCode(CmdBean.EC_UNKNOWN);
							break;
						}
					} catch (BadCmdException e) {
						log.fatal("Bad cmd: " + e.getMessage());
					} catch (KeyException e) {
						log.fatal("Bad key: " + e.getMessage());
					} catch (Exception e) {
						log.fatal("Other exception: " + e.getMessage());
					}

					String rsp = cmdBean.getResponse();

					out.write(rsp.getBytes(), 0, rsp.length());
					log.info("SendOut message" + rsp);
				}

			} catch (BindException e) {
				log.fatal("Bind error: " + e.getMessage());
				// System.exit(0);
			} catch (IOException e) {
				log.fatal("IO error: " + e.getMessage());
			} finally {
				try {
					socket.close();
				} catch (IOException e) {
					log.fatal("Server error: " + e.getMessage());
				}
				log("Connection with client# " + clientNumber + " closed");
			}
		}

		private void log(String message) {
			System.out.println(message);
		}
	}

	ServerSocket listener;

	@Override
	public void run() {
		log.info("[info] Server Begin: ServerSocket on port:" + port);
		Thread.currentThread().setName("SSM Server Listener");
		int clientNumber = 0;
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
				log.info("Server IO error:" + e.getMessage());
			}
			// if (Thread.currentThread().isInterrupted()) {
			// cleanup
			// }

		}

	}

	private volatile boolean running = true;

	public void terminate() {
		try {
			listener.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		running = false;
	}

}
