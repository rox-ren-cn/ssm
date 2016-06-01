package ssm;

import ssm.ui.ExceptionEvent;
import ssm.ui.ExceptionListener;
import ssm.ui.KeyModel;
import ssm.util.CmdException;
import ssm.util.DataException;
import ssm.util.CmdBean;
import ssm.util.Des;
import ssm.util.KeyBean;
import ssm.util.KeyException;
import ssm.util.JDBCConnection;

import java.net.*;

import javax.swing.event.EventListenerList;

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

	JDBCConnection mvb = JDBCConnection.getInstance();

	private static class Capitalizer extends Thread {
		private SSMServer father;
		private Socket socket;
		private int clientNumber;

		private static final Log log = LogFactory.getLog(Capitalizer.class);

		public Capitalizer(SSMServer father, Socket socket, int clientNumber) {
			this.father = father;
			this.socket = socket;
			this.clientNumber = clientNumber;
			log.debug("New connection with client# " + clientNumber + " at " + socket);
			ExceptionEvent event = new ExceptionEvent(this,
					"New connection with client# " + clientNumber + " at " + socket);
			father.fireExceptionGenerated(event);
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
//							String s = keyModel.TransPIN(cmdBean.getATMID(), cmdBean.getEntityID(), cmdBean.getPINBlock());
//							cmdBean.setData(s);
//							cmdBean.setErrorCode("00");
							keyModel.TransPIN(cmdBean);
							break;
						case CmdBean.CLR_PIN:
//							String c = keyModel.GetClearPIN(cmdBean.getATMID(), cmdBean.getPINBlock());
//							cmdBean.setData(Des.GetPin(cmdBean.getPan(), c));
//							cmdBean.setErrorCode("00");
							keyModel.GetClearPIN(cmdBean);
							break;
						default:
							log.fatal("[fail] Invalid cmd: [" + cmdBean.getCmd() + "]");
							cmdBean.setErrorCode(CmdBean.EC_UNKNOWN);
							break;
						}
					} catch (CmdException e) {
						log.fatal("Bad cmd: " + e.getMessage());
						ExceptionEvent event = new ExceptionEvent(this,
								"Bad cmd with client# " + clientNumber + " " + e.getMessage());
						father.fireExceptionGenerated(event);

					} catch (KeyException e) {
						log.fatal("Bad key: " + e.getMessage());
						ExceptionEvent event = new ExceptionEvent(this,
								"Bad key with client# " + clientNumber + " " + e.getMessage());
						father.fireExceptionGenerated(event);

					} catch (DataException e) {
						log.fatal("Bad data: " + e.getMessage());
						ExceptionEvent event = new ExceptionEvent(this,
								"Bad data with client# " + clientNumber + " " + e.getMessage());
						father.fireExceptionGenerated(event);

					} catch (Exception e) {
						log.fatal("Other exception: " + e.getMessage());
						ExceptionEvent event = new ExceptionEvent(this,
								"Other exception with client# " + clientNumber + " : " + e.getMessage());
						father.fireExceptionGenerated(event);
					}

					String rsp = cmdBean.getResponse();

					out.write(rsp.getBytes(), 0, rsp.length());
					log.info("SendOut message:" + rsp);
					ExceptionEvent event = new ExceptionEvent(this,
							"Client# " + clientNumber + " SendOut message:" + rsp);
					father.fireExceptionGenerated(event);
				}

			} catch (IOException e) {
				log.fatal("IO error: " + e.getMessage());
				ExceptionEvent event = new ExceptionEvent(this,
						"IO error on client# " + clientNumber + " : " + e.getMessage());
				father.fireExceptionGenerated(event);
			} finally {
				try {
					socket.close();
				} catch (IOException e) {
					log.fatal("Server error: " + e.getMessage());
				}
				log.debug("Connection with client# " + clientNumber + " closed");
				ExceptionEvent event = new ExceptionEvent(this,
						"Connection with client# " + clientNumber + " closed");
				father.fireExceptionGenerated(event);
			}
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
						new Capitalizer(this, listener.accept(), clientNumber++).start();
					}
				} finally {
					listener.close();
				}
			} catch (IOException e) {
				log.info("Server IO error:" + e.getMessage());
				ExceptionEvent event = new ExceptionEvent(this, e.getMessage());
				fireExceptionGenerated(event);
			}
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

	protected EventListenerList listenerList = new EventListenerList();

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
