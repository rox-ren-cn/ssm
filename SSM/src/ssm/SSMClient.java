package ssm;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import javax.swing.JFrame;
//import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class SSMClient {

	InputStream is;
	OutputStream os;
	private JFrame frame = new JFrame("Capitalize Client");
	private JTextField dataField = new JTextField(40);
	private JTextArea messageArea = new JTextArea(8, 60);

	public SSMClient() {
		messageArea.setEditable(false);
		frame.getContentPane().add(dataField, "North");
		frame.getContentPane().add(new JScrollPane(messageArea), "Center");

		dataField.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String msg = "";
				try {
					switch (dataField.getText()) {
					case "1":
						msg = "RPIMIM10001004";
						break;
					case "2":
						msg = "PRIMXK010001004";
						break;
					case "3":
						msg = "PRIMXK110001004";
						break;
					case "4":
						msg = "PRIMTP123456788888888800000000000000000000000000000000";
						break;
					case "5":
						msg = "PRIMCP123456784392260020773618   00000000000000000000000000000000";
						break;
					default:
						msg = "PRIMIM12345678";
					}
					os.write(msg.getBytes(), 0, msg.length());
				} catch (IOException e1) {
					e1.printStackTrace();
				}
				String response;
				try {
					byte[] b = new byte[100];
					int len = is.read(b, 0, 100);
					response = new String(b, 0, len);
					if (response == null || response.equals("")) {
						System.exit(0);
					}
				} catch (IOException ex) {
					response = "Error: " + ex;
				}
				messageArea.append(response + "\n");
				dataField.selectAll();
			}
		});
	}

	public void connectToServer() throws IOException {
		// String serverAddress = JOptionPane.showInputDialog(
		// frame,
		// "Enter IP Address of the Server:",
		// "Welcome to the Capitalization Program",
		// JOptionPane.QUESTION_MESSAGE);
		String serverAddress = "localhost";

		@SuppressWarnings("resource")
		Socket socket = new Socket(serverAddress, 12345);
		is = socket.getInputStream();
		os = socket.getOutputStream();
	}

	public static void main(String[] args) throws Exception {
		SSMClient client = new SSMClient();
		client.frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		client.frame.pack();
		client.frame.setVisible(true);
		client.connectToServer();
	}
}