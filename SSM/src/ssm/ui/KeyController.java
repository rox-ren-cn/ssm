package ssm.ui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.tree.TreePath;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ssm.SSMServer;
import ssm.util.Des;
import ssm.util.KeyBean;
import ssm.util.KeyException;
import ssm.util.KeyTree;
import ssm.util.KeyTreeModel;

public class KeyController implements ActionListener, ExceptionListener {
	private SSMView view = null;
	private KeyModel model = null;
	private SSMServer server = null;

	private Log log = LogFactory.getLog(KeyController.class);

	public static final int OPERATIONSUCCESS = 0;
	public static final int OPERATIONFAILED = 1;
	public static final int VALIDATIONERROR = 2;

	public KeyController(SSMView view) {
		this.view = view;
		model = new KeyModel();

		model.addExceptionListener(this);
	}

	public void actionPerformed(ActionEvent e) {
		String actionCommand = e.getActionCommand();

		if (actionCommand.equals("Insert Key")) {
			KeyInsertDialog iDialog = new KeyInsertDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Init LMK")) {
			LMKInitDialog iDialog = new LMKInitDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Input ZPK")) {
			ZPKInputDialog iDialog = new ZPKInputDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Output ZPK")) {
			ZPKOutputDialog iDialog = new ZPKOutputDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Input ATM Key")) {
			ATMKeyInputDialog iDialog = new ATMKeyInputDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Init TMK")) {
			TMKInitDialog iDialog = new TMKInitDialog(view);
			iDialog.pack();
			view.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Update Key")) {
			KeyUpdateDialog uDialog = new KeyUpdateDialog(view);
			uDialog.pack();
			view.centerWindow(uDialog);
			uDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Delete Key")) {
			KeyDeleteDialog dDialog = new KeyDeleteDialog(view);
			dDialog.pack();
			view.centerWindow(dDialog);
			dDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Show Key")) {
			showAllKeys();
			return;
		}

		if (actionCommand.equals("Init Key")) {
			initKeys();
			return;
		}
		if (actionCommand.equals("Start Server")) {
			StartService();
			view.menuItemStartService.setEnabled(false);
			view.menuItemStopService.setEnabled(true);
			return;
		}
		if (actionCommand.equals("Stop Server")) {
			StopService();
			view.menuItemStartService.setEnabled(true);
			view.menuItemStopService.setEnabled(false);
			return;
		}
	}

	void StartService() {
		server = new SSMServer(Integer.parseInt(SSMView.prop.getProperty("ssm.port")));
		server.addExceptionListener(this);
		server.start();
	}

	void StopService() {
		if (server != null) {
			server.interrupt();
			server.terminate();
			try {
				server.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	public void exceptionGenerated(ExceptionEvent ex) {
		String message = ex.getMessage();

		Toolkit.getDefaultToolkit().beep();

		if (message != null) {
			view.updateStatusBar(ex.getMessage());
		} else {
			view.updateStatusBar("An exception occurred!");
		}
	}

	private void showAllKeys() {
		KeyTree kt2 = model.showKeys(1);
		kt2.addExceptionListener(this);
		JKeyTree kt = new JKeyTree(kt2.getTree());

		view.addTree(kt);
	}

	private void initKeys() {
		KeyTree v = model.loadKeys();
		JKeyTree kt = null;
		if (v == null) {
			kt = new JKeyTree();
			Toolkit.getDefaultToolkit().beep();
			JOptionPane.showMessageDialog(view, "LMK isn't initialized", "Warning", JOptionPane.WARNING_MESSAGE);
		} else {
			kt = new JKeyTree(v.getTree());

		}
		if (v == null || !v.isLMKInited)
			view.setMenuLevel(0);
		else if (!v.isTMKInited)
			view.setMenuLevel(1);
		else
			view.setMenuLevel(2);

		final JKeyTree tree = kt;
		tree.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent me) {
				if (me.getClickCount() > 1) {
					TreePath path = tree.getPathForLocation(me.getX(), me.getY());
					KeyTreeModel c = (KeyTreeModel) path.getLastPathComponent();
					KeyBean kb = c.getKey();
					JOptionPane.showMessageDialog(null, "KEV: " + kb.getKev() + "  KCV: " + kb.getKcv(),
							"KID: " + kb.getKid() + "  TYPE: " + kb.getTyp(), JOptionPane.INFORMATION_MESSAGE);
				}
			}
		});

		// Adds the table to the scrollpane.
		// By default, a JTable does not have scroll bars.
		view.addTree(kt);
	}

	class LMKInitDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev_p1 = makeTextField(32);
		private JTextField kev_p2 = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		public LMKInitDialog(JFrame parent) {
			super(parent, "Init LMK", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			kid.setEditable(false);
			kid.setText("99999999");
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			typ.setText("LMK");
			typ.setEditable(false);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Part1: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p1, c);
			inputPane.add(kev_p1);

			// create and place key value label
			label = new JLabel("Key Part2: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p2, c);
			inputPane.add(kev_p2);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// when the return key is pressed in the last field of this form,
			// the action performed by the ok button is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");

			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				switch (validateInsert()) {
				case OPERATIONSUCCESS:
					dispose();
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		/*
		 * Validates the text fields in KeyInsertDialog and then calls
		 * key.insertKey() if the fields are valid. Returns the operation
		 * status, which is one of OPERATIONSUCCESS, OPERATIONFAILED,
		 * VALIDATIONERROR.
		 */
		private int validateInsert() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = Des.logic(Des.logic_op.xor, kev_p1.getText(), kev_p2.getText());
				String skcv = kcv.getText();

				if (skid.length() == 8 && styp.length() == 3 && skev.length() == 32 && skcv.length() == 6) {
					// check for duplicates
					if (model.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Inserting key...");

				if (model.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					view.setMenuLevel(1);
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				// this exception is thrown when a string
				// cannot be converted to a number
				return VALIDATIONERROR;
			}
		}
	}

	class TMKInitDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev_p1 = makeTextField(32);
		private JTextField kev_p2 = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public TMKInitDialog(JFrame parent) {
			super(parent, "Init TMK", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel will contain the text field labels and the text
			// fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			kid.setEditable(false);
			kid.setText("99999999");
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			typ.setText("TMK");
			typ.setEditable(false);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Part1: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p1, c);
			inputPane.add(kev_p1);

			// create and place key value label
			label = new JLabel("Key Part2: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p2, c);
			inputPane.add(kev_p2);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// when the return key is pressed in the last field
			// of this form, the action performed by the ok button
			// is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				switch (validateInsert()) {
				case OPERATIONSUCCESS:
					dispose();
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		/*
		 * Validates the text fields in KeyInsertDialog and then calls
		 * key.insertKey() if the fields are valid. Returns the operation
		 * status, which is one of OPERATIONSUCCESS, OPERATIONFAILED,
		 * VALIDATIONERROR.
		 */
		private int validateInsert() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = Des.logic(Des.logic_op.xor, kev_p1.getText(), kev_p2.getText());
				String skcv = kcv.getText();

				if (skid.length() == 8 && styp.length() == 3 && skev.length() == 32 && skcv.length() == 6) {
					// check for duplicates
					if (model.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Inserting key...");

				if (model.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					view.setMenuLevel(2);
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				return VALIDATIONERROR;
			}
		}
	}

	class ZPKInputDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev_p1 = makeTextField(32);
		private JTextField kev_p2 = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		public ZPKInputDialog(JFrame parent) {
			super(parent, "Input ZPK", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel will contain the text field labels and the text
			// fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			typ.setText("ZPK");
			typ.setEditable(false);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Value Part1: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p1, c);
			inputPane.add(kev_p1);

			// create and place key value label
			label = new JLabel("Key Value Part2: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p2, c);
			inputPane.add(kev_p2);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// when the return key is pressed in the last field
			// of this form, the action performed by the ok button
			// is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				switch (validateInsert()) {
				case OPERATIONSUCCESS:
					dispose();
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		/*
		 * Validates the text fields in KeyInsertDialog and then calls
		 * key.insertKey() if the fields are valid. Returns the operation
		 * status, which is one of OPERATIONSUCCESS, OPERATIONFAILED,
		 * VALIDATIONERROR.
		 */
		private int validateInsert() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = Des.logic(Des.logic_op.xor, kev_p1.getText(), kev_p2.getText());
				String skcv = kcv.getText();

				if (skid.length() == 8 && styp.length() == 3 && skev.length() == 32 && skcv.length() == 6) {
					// check for duplicates
					if (model.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Inserting key...");

				if (model.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				// this exception is thrown when a string
				// cannot be converted to a number
				return VALIDATIONERROR;
			}
		}
	}

	class ZPKOutputDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev_p1 = makeTextField(32);
		private JTextField kev_p2 = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public ZPKOutputDialog(JFrame parent) {
			super(parent, "Output ZPK", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel will contain the text field labels and the text
			// fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			inputPane.add(kid);

			// when the return key is pressed in the kid field
			// the query action is performed
			kid.addActionListener(this);
			kid.setActionCommand("QUERY");

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			typ.setText("ZPK");
			typ.setEditable(false);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Value Part1: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p1, c);
			inputPane.add(kev_p1);

			// create and place key value label
			label = new JLabel("Key Value Part2: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev_p2, c);
			inputPane.add(kev_p2);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
//			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
//			OKButton.setActionCommand("OK");
			OKButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
//			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
//			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("QUERY")) {
				switch (validateQuery()) {
				case OPERATIONSUCCESS:
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		private int validateQuery() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();

				if (skid.length() == 8) {
					// check for duplicates
					KeyBean kb = model.getKey(skid, styp);
					if (kb == null) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
					kev_p1.setText(Des.LMKDec(kb.getKev()));
					kev_p2.setText("00000000000000000000000000000000");
					kcv.setText(kb.getKcv());
					view.updateStatusBar("Key retrieved");
					return OPERATIONSUCCESS;
				} else {
					return VALIDATIONERROR;
				}

			} catch (NumberFormatException ex) {
				return VALIDATIONERROR;
			}
		}
	}

	class ATMKeyInputDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		String[] comboList = { "TMK", "TPK" };
		private JComboBox<String> typ = new JComboBox<String>(comboList);
		private JTextField kev = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public ATMKeyInputDialog(JFrame parent) {
			super(parent, "Input ZPK", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel will contain the text field labels and the text
			// fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev, c);
			inputPane.add(kev);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// when the return key is pressed in the last field
			// of this form, the action performed by the ok button
			// is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				switch (validateInsert()) {
				case OPERATIONSUCCESS:
					dispose();
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		/*
		 * Validates the text fields in KeyInsertDialog and then calls
		 * key.insertKey() if the fields are valid. Returns the operation
		 * status, which is one of OPERATIONSUCCESS, OPERATIONFAILED,
		 * VALIDATIONERROR.
		 */
		private int validateInsert() {
			try {
				String skid = kid.getText();
				String styp = typ.getSelectedItem().toString();
				String skev = kev.getText();
				String skcv = kcv.getText();

				if (skid.length() == 8 && styp.length() == 3 && skev.length() == 32 && skcv.length() == 6) {
					// check for duplicates
					if (model.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Inserting key...");

				if (model.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				// this exception is thrown when a string
				// cannot be converted to a number
				return VALIDATIONERROR;
			}
		}
	}

	/*
	 * This class creates a dialog box for inserting a key.
	 */
	class KeyInsertDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public KeyInsertDialog(JFrame parent) {
			super(parent, "Insert Key", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel will contain the text field labels and the text
			// fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			kid.setDocument(new LengthRestrictedDocument(8, 0));
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			inputPane.add(typ);

			// create and place key value label
			label = new JLabel("Key Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev, c);
			inputPane.add(kev);

			// create and place key check value label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key check value field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kcv, c);
			inputPane.add(kcv);

			// when the return key is pressed in the last field
			// of this form, the action performed by the ok button
			// is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add the buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyInsertDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				switch (validateInsert()) {
				case OPERATIONSUCCESS:
					dispose();
					break;
				case VALIDATIONERROR:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				case OPERATIONFAILED:
					Toolkit.getDefaultToolkit().beep();
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
					break;
				}
			}
		}

		/*
		 * Validates the text fields in KeyInsertDialog and then calls
		 * key.insertKey() if the fields are valid. Returns the operation
		 * status, which is one of OPERATIONSUCCESS, OPERATIONFAILED,
		 * VALIDATIONERROR.
		 */
		private int validateInsert() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = kev.getText();
				String skcv = kcv.getText();

				if (skid.length() == 8 && styp.length() == 3 && skev.length() == 32 && skcv.length() == 6) {
					// check for duplicates
					if (model.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						view.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Inserting key...");

				if (model.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				// this exception is thrown when a string
				// cannot be converted to a number
				return VALIDATIONERROR;
			}
		}
	}

	/*
	 * This class creates a dialog box for updating a key.
	 */
	class KeyUpdateDialog extends JDialog implements ActionListener {
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);
		private JTextField kev = makeTextField(32);
		private JTextField kcv = makeTextField(6);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public KeyUpdateDialog(JFrame parent) {
			super(parent, "Update Key", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel contains the text field labels and
			// the text fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			inputPane.add(kid);

			// create and place key type label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key type field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			inputPane.add(typ);

			// when the return key is pressed in the last field
			// of this form, the action performed by the ok button
			// is executed
			kcv.addActionListener(this);
			kcv.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyUpdateDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				if (validateUpdate() != VALIDATIONERROR) {
					dispose();
				} else {
					Toolkit.getDefaultToolkit().beep();

					// display a popup to inform the user of the error
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
				}
			}
		}

		private int validateUpdate() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = kev.getText();
				String skcv = kcv.getText();

				if (skev.length() == 0 || skcv.length() == 0) {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Updating key...");

				if (model.updateKey(new KeyBean(skid, styp, skev, skcv))) {
					view.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				return VALIDATIONERROR;
			} catch (KeyException e) {
				log.debug("UpdateKey " + e.getMessage());
				return VALIDATIONERROR;
			}
		}
	}

	/*
	 * This class creates a dialog box for deleting a key.
	 */
	class KeyDeleteDialog extends JDialog implements ActionListener {
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private JTextField kid = makeTextField(8);
		private JTextField typ = makeTextField(3);

		/*
		 * Constructor. Creates the dialog's GUI.
		 */
		public KeyDeleteDialog(JFrame parent) {
			super(parent, "Delete Key", true);
			setResizable(false);

			JPanel contentPane = new JPanel(new BorderLayout());
			setContentPane(contentPane);
			contentPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

			// this panel contains the text field labels and the text fields.
			JPanel inputPane = new JPanel();
			inputPane.setBorder(BorderFactory.createCompoundBorder(new TitledBorder(new EtchedBorder(), "Key Fields"),
					new EmptyBorder(5, 5, 5, 5)));

			// add the text field labels and text fields to inputPane
			// using the GridBag layout manager

			GridBagLayout gb = new GridBagLayout();
			GridBagConstraints c = new GridBagConstraints();
			inputPane.setLayout(gb);

			// create and place key id label
			JLabel label = new JLabel("Key ID: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kid, c);
			inputPane.add(kid);

			// create and place key id label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(0, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key id field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(0, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			inputPane.add(typ);

			typ.addActionListener(this);
			typ.setActionCommand("OK");

			// panel for the OK and cancel buttons
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
			buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 0, 0, 2));

			JButton OKButton = new JButton("OK");
			JButton cancelButton = new JButton("Cancel");
			OKButton.addActionListener(this);
			OKButton.setActionCommand("OK");
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});

			// add buttons to buttonPane
			buttonPane.add(Box.createHorizontalGlue());
			buttonPane.add(OKButton);
			buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
			buttonPane.add(cancelButton);

			contentPane.add(inputPane, BorderLayout.CENTER);
			contentPane.add(buttonPane, BorderLayout.SOUTH);

			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent e) {
					dispose();
				}
			});
		}

		/*
		 * Event handler for the OK button in KeyDeleteDialog
		 */
		public void actionPerformed(ActionEvent e) {
			String actionCommand = e.getActionCommand();

			if (actionCommand.equals("OK")) {
				if (validateDelete() != VALIDATIONERROR) {
					dispose();
				} else {
					Toolkit.getDefaultToolkit().beep();

					// display a popup to inform the user of the error
					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
				}
			}
		}

		private int validateDelete() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();

				if (skid.length() == 0 || styp.length() == 0) {
					return VALIDATIONERROR;
				}

				view.updateStatusBar("Deleting key...");

				if (model.deleteKey(skid, styp)) {
					view.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					view.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
				return VALIDATIONERROR;
			}
		}
	}

	JTextField makeTextField(int len) {
		return makeTextField(len, 0);
	}

	JTextField makeTextField(int len, int mode) {
		JTextField tf = new JTextField(len);
		tf.setDocument(new LengthRestrictedDocument(len, mode));
		return tf;
	}

}
