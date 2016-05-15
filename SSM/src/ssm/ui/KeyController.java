package ssm.ui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

import ssm.util.KeyBean;
import ssm.util.KeyTree;

import java.sql.*;

public class KeyController implements ActionListener, ExceptionListener {
	private MvbView mvb = null;
	private KeyModel key = null;

	public static final int OPERATIONSUCCESS = 0;
	public static final int OPERATIONFAILED = 1;
	public static final int VALIDATIONERROR = 2;

	public KeyController(MvbView mvb) {
		this.mvb = mvb;
		key = new KeyModel();

		key.addExceptionListener(this);
	}

	public void actionPerformed(ActionEvent e) {
		String actionCommand = e.getActionCommand();

		if (actionCommand.equals("Insert Key")) {
			KeyInsertDialog iDialog = new KeyInsertDialog(mvb);
			iDialog.pack();
			mvb.centerWindow(iDialog);
			iDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Update Key")) {
			KeyUpdateDialog uDialog = new KeyUpdateDialog(mvb);
			uDialog.pack();
			mvb.centerWindow(uDialog);
			uDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Delete Key")) {
			KeyDeleteDialog dDialog = new KeyDeleteDialog(mvb);
			dDialog.pack();
			mvb.centerWindow(dDialog);
			dDialog.setVisible(true);
			return;
		}

		if (actionCommand.equals("Show Key")) {
			showAllKeys();
			return;
		}

		if (actionCommand.equals("Edit Key")) {
			editAllKeys();
			return;
		}
		if (actionCommand.equals("Init Key")) {
			initKeys();
			return;
		}
	}

	public void exceptionGenerated(ExceptionEvent ex) {
		String message = ex.getMessage();

		Toolkit.getDefaultToolkit().beep();

		if (message != null) {
			mvb.updateStatusBar(ex.getMessage());
		} else {
			mvb.updateStatusBar("An exception occurred!");
		}
	}

	private void showAllKeys() {
		ResultSet rs = key.showKeys();
		CustomTableModel model = new CustomTableModel(key.getConnection(), rs);
		CustomTable data = new CustomTable(model);

		KeyTree v = key.showKeys(1);

		JKeyTree kt = new JKeyTree(v);

		// register to be notified of any exceptions that occur in the model and
		// table
		model.addExceptionListener(this);
		data.addExceptionListener(this);

		// Adds the table to the scrollpane.
		// By default, a JTable does not have scroll bars.
		mvb.addTree(kt);
	}

	/*
	 * This method displays all branches in an editable JTable
	 */
	private void editAllKeys() {
		ResultSet rs = key.editKey();

		CustomTableModel model = new CustomTableModel(key.getConnection(), rs);
		CustomTable data = new CustomTable(model);

		model.addExceptionListener(this);
		data.addExceptionListener(this);

		mvb.addTable(data);
	}

	private void initKeys() {
//		ResultSet rs = key.showKeys();

		KeyTree v = key.showKeys(1);
		JKeyTree kt = new JKeyTree(v);

		// Adds the table to the scrollpane.
		// By default, a JTable does not have scroll bars.
		mvb.addTree(kt);
	}

	/*
	 * This class creates a dialog box for inserting a key.
	 */
	class KeyInsertDialog extends JDialog implements ActionListener {
		/**
		 * 
		 */
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

			// create and place key name label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key name field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(typ, c);
			inputPane.add(typ);

			// create and place key address label
			label = new JLabel("Key Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key address field
			c.gridwidth = GridBagConstraints.REMAINDER;
			c.insets = new Insets(5, 0, 0, 0);
			c.anchor = GridBagConstraints.WEST;
			gb.setConstraints(kev, c);
			inputPane.add(kev);

			// create and place key city label
			label = new JLabel("Key Check Value: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key city field
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
				if (validateInsert() != VALIDATIONERROR) {
					dispose();
				} else {
					Toolkit.getDefaultToolkit().beep();

					JOptionPane.showMessageDialog(this, "Invalid Input", "Error", JOptionPane.ERROR_MESSAGE);
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

				if (kid.getText().trim().length() > 0 && typ.getText().trim().length() > 0) {
					// check for duplicates
					if (key.findKey(skid, styp)) {
						Toolkit.getDefaultToolkit().beep();
						mvb.updateStatusBar("Key " + skid + "/" + styp + " already exists!");
						return OPERATIONFAILED;
					}
				} else {
					return VALIDATIONERROR;
				}

				mvb.updateStatusBar("Inserting key...");

				if (key.insertKey(new KeyBean(skid, styp, skev, skcv))) {
					mvb.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					mvb.updateStatusBar("Operation failed.");
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
		/**
		 * 
		 */
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

			// create and place key name label
			label = new JLabel("Key Type: ", SwingConstants.RIGHT);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.insets = new Insets(5, 0, 0, 5);
			c.anchor = GridBagConstraints.EAST;
			gb.setConstraints(label, c);
			inputPane.add(label);

			// place key name field
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

		/*
		 * Validates the text fields in KeyUpdateDialog and then calls
		 * key.branchUpdate() if the fields are valid. Returns the operation
		 * status.
		 */
		private int validateUpdate() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();
				String skev = kev.getText();
				String skcv = kcv.getText();

				if (skev.length() == 0 || skcv.length() == 0) {
					return VALIDATIONERROR;
				}

				mvb.updateStatusBar("Updating key...");

				if (key.updateKey(skid, styp, skev, skcv)) {
					mvb.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					mvb.updateStatusBar("Operation failed.");
					return OPERATIONFAILED;
				}
			} catch (NumberFormatException ex) {
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

			// when the return key is pressed while in the
			// branchID field, the action performed by the ok button
			// is executed
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

		/*
		 * Validates the text fields in KeyDeleteDialog and then calls
		 * key.branchDelete() if the fields are valid. Returns the operation
		 * status.
		 */
		private int validateDelete() {
			try {
				String skid = kid.getText();
				String styp = typ.getText();

				if (skid.length() == 0 || styp.length() == 0) {
					return VALIDATIONERROR;
				}

				mvb.updateStatusBar("Deleting key...");

				if (key.deleteKey(skid, styp)) {
					mvb.updateStatusBar("Operation successful.");
					showAllKeys();
					return OPERATIONSUCCESS;
				} else {
					Toolkit.getDefaultToolkit().beep();
					mvb.updateStatusBar("Operation failed.");
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
