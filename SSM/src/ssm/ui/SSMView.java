package ssm.ui;

import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import javax.swing.*;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;


public class SSMView extends JFrame {
	private static final long serialVersionUID = 1L;

	private static final Log log = LogFactory.getLog(SSMView.class);
	
	// initial position of the main frame
	private int framePositionX;
	private int framePositionY;

	// initial size of main frame
	private Rectangle frameBounds = null;

	// the status text area for displaying error messages
	private JTextArea statusField = new JTextArea(5, 0);

	// the scrollpane that will hold the table of database data
	private JScrollPane tableScrPane = new JScrollPane();

	// the key admin menu
	private JMenu keyAdmin;
	private JMenu svcAdmin;

	public SSMView() {
		// should call the constructor of the superclass first
		super("SSM Administration");
		setSize(650, 450);

		// the content pane;
		// components will be spaced vertically 10 pixels apart
		JPanel contentPane = new JPanel(new BorderLayout(0, 10));
		setContentPane(contentPane);

		// leave some space around the content pane
		contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

		// setup the menubar
		JMenuBar menuBar = new JMenuBar();
		setJMenuBar(menuBar);

		// indent first menu
		menuBar.add(Box.createRigidArea(new Dimension(10, 0)));

		// sets up the branch administration menu and adds it to the
		// menu bar
		setupKeyAdminMenu(menuBar);

		// Setup some other menus. You may be asked to add functionality
		// to these menus in an assignment.
		setupServiceMenu(menuBar);

		// the scrollpane for the status text field
		JScrollPane statusScrPane = new JScrollPane(statusField);
		statusScrPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		statusScrPane.setBorder(BorderFactory.createLoweredBevelBorder());

		// set status field properties
		statusField.setEditable(false);
		statusField.setLineWrap(true);
		statusField.setWrapStyleWord(true);

		// add the panes to the content pane
		contentPane.add(tableScrPane, BorderLayout.CENTER);
		contentPane.add(statusScrPane, BorderLayout.SOUTH);

		// center the main window
		Dimension screenSize = getToolkit().getScreenSize();
		frameBounds = getBounds();
		framePositionX = (screenSize.width - frameBounds.width) / 2;
		framePositionY = (screenSize.height - frameBounds.height) / 2;
		setLocation(framePositionX, framePositionY);

		// anonymous inner class to terminate program
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				log.debug("SSM terminated");
				System.exit(0);
			}
		});
	}

	public JMenuItem miInitLMK = null;
	public JMenuItem miInitTMK = null;
	public JMenuItem miInputZPK = null;
	public JMenuItem miOutputZPK = null;

	public void setMenuLevel(int i) {
		switch (i) {
		case 0:
			miInitLMK.setEnabled(true);
			miInitTMK.setEnabled(false);
			miInputZPK.setEnabled(false);
			miOutputZPK.setEnabled(false);
			break;
		case 1:
			miInitLMK.setEnabled(false);
			miInitTMK.setEnabled(true);
			miInputZPK.setEnabled(true);
			miOutputZPK.setEnabled(true);
			break;
		case 2:
			miInitLMK.setEnabled(false);
			miInitTMK.setEnabled(false);
			miInputZPK.setEnabled(true);
			miOutputZPK.setEnabled(true);
			break;
		}
	}

	private void setupKeyAdminMenu(JMenuBar mb) {
		keyAdmin = new JMenu("Key Admin");

		// when alt-b is pressed on the keyboard, the menu will appear
		keyAdmin.setMnemonic(KeyEvent.VK_B);

//		createMenuItem(keyAdmin, "Insert Key...", KeyEvent.VK_I, "Insert Key");
//		createMenuItem(keyAdmin, "Update Key Data...", KeyEvent.VK_U, "Update Key");
//		createMenuItem(keyAdmin, "Delete Key...", KeyEvent.VK_D, "Delete Key");
		createMenuItem(keyAdmin, "Show All Keys", KeyEvent.VK_S, "Show Key")
				.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, ActionEvent.CTRL_MASK));

		// createMenuItem(keyAdmin, "Edit All Keys", KeyEvent.VK_E, "Edit Key");
		miInitLMK = createMenuItem(keyAdmin, "Init LMK", KeyEvent.VK_L, "Init LMK");
		miInitTMK = createMenuItem(keyAdmin, "Init TMK", KeyEvent.VK_T, "Init TMK");
		miInputZPK = createMenuItem(keyAdmin, "Input ZPK", KeyEvent.VK_Z, "Input ZPK");
		miOutputZPK = createMenuItem(keyAdmin, "Output ZPK", KeyEvent.VK_Y, "Output ZPK");
//		createMenuItem(keyAdmin, "Input ATM Key", KeyEvent.VK_Z, "Input ATM Key");
		
		mb.add(keyAdmin);
	}

	public JMenuItem menuItemStartService = null;
	public JMenuItem menuItemStopService = null;
	private void setupServiceMenu(JMenuBar mb) {
		
		// Setup some other menus. You may be asked to add functionality
		// to these menus in an assignment.
		svcAdmin = new JMenu("Service");
		svcAdmin.setMnemonic(KeyEvent.VK_D);
		svcAdmin.setActionCommand("Start Service");

		// when alt-b is pressed on the keyboard, the menu will appear
		svcAdmin.setMnemonic(KeyEvent.VK_S);

		menuItemStartService = createMenuItem(svcAdmin, "Start Server", KeyEvent.VK_I, "Start Server");
		menuItemStopService = createMenuItem(svcAdmin, "Stop Server", KeyEvent.VK_U, "Stop Server");

		menuItemStopService.setEnabled(false);
		
		mb.add(svcAdmin);
	}

	private JMenuItem createMenuItem(JMenu menu, String label, int mnemonicKey, String actionCommand) {
		JMenuItem menuItem = new JMenuItem(label);

		if (mnemonicKey > 0)
			menuItem.setMnemonic(mnemonicKey);

		if (actionCommand.length() > 0)
			menuItem.setActionCommand(actionCommand);

		menu.add(menuItem);

		return menuItem;
	}

	public void centerWindow(Window w) {
		Rectangle winBounds = w.getBounds();
		w.setLocation(framePositionX + (frameBounds.width - winBounds.width) / 2,
				framePositionY + (frameBounds.height - winBounds.height) / 2);
	}

	public void updateStatusBar(String s) {
		statusField.append(s.trim() + "\n");

		statusField.revalidate();
	}

	public void addTable(JTable data) {
		tableScrPane.setViewportView(data);
	}

	public void addTree(JKeyTree data) {
		tableScrPane.setViewportView(data);
	}

	public void registerControllers() {
		JMenuItem menuItem;

		KeyController kc = new KeyController(this);

		for (int i = 0; i < keyAdmin.getItemCount(); i++) {
			menuItem = keyAdmin.getItem(i);
			menuItem.addActionListener(kc);
		}
		for (int i = 0; i < svcAdmin.getItemCount(); i++) {
			menuItem = svcAdmin.getItem(i);
			menuItem.addActionListener(kc);
		}

	}

	public void showAllKeys() {
		ActionListener[] aa = keyAdmin.getItem(3).getActionListeners();
		for(ActionListener a: aa) {
		    a.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Init Key"));
		}
	}

	public static Properties prop = new Properties();
	public static void main(String[] args) {

		ClassLoader loader = Thread.currentThread().getContextClassLoader();           
		InputStream stream = loader.getResourceAsStream("config/ssm.properties");
		try {
			prop.load(stream);
		} catch (IOException e) {
			log.debug("ssm.properties doesn't exist");
		}

		SSMView mvb = new SSMView();
	
		mvb.setVisible(true);
		LoginWindow lw = new LoginWindow(mvb);
		lw.addWindowListener(new ControllerRegister(mvb));
		lw.pack();
		mvb.centerWindow(lw);
		lw.setVisible(true);
	}
}

class ControllerRegister extends WindowAdapter {
	private SSMView view;

	public ControllerRegister(SSMView view) {
		this.view = view;
	}

	public void windowClosed(WindowEvent e) {
		view.registerControllers();
		view.showAllKeys();
	}
}
