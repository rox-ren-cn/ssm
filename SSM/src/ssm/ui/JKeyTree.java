package ssm.ui;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Font;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreeCellRenderer;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import ssm.util.KeyBean;
import ssm.util.KeyTreeModel;

/**
 * This class is a JTree subclass that displays the tree of AWT or Swing
 * component that make up a GUI.
 */
public class JKeyTree extends JTree {

	private static final long serialVersionUID = 1L;

	/**
	 * All this constructor method has to do is set the TreeModel and
	 * TreeCellRenderer objects for the tree.
	 */
	public JKeyTree(KeyTreeModel kt) {
		super(kt);
		ImageIcon LeafIcon = new ImageIcon(JKeyTree.class.getResource("/icons/Key.ico"));
		ImageIcon OpenIcon = new ImageIcon(JKeyTree.class.getResource("/icons/OpenKey.ico"));
//		ImageIcon ClosedIcon = new ImageIcon(JKeyTree.class.getResource("/icons/ClosedKey.ico"));

		DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
		renderer.setLeafIcon(LeafIcon);
//		renderer.setClosedIcon(ClosedIcon);
		renderer.setOpenIcon(OpenIcon);
		setCellRenderer(new KeyCellRenderer(getCellRenderer()));
//		setCellRenderer(renderer);
	}

	public JKeyTree() {
		this(null);
	}

	/**
	 * A TreeCellRenderer displays each node of a tree. The default renderer
	 * displays arbitrary Object nodes by calling their toString() method. The
	 * Component.toString() method returns long strings with extraneous
	 * information. Therefore, we use this "wrapper" implementation of
	 * TreeCellRenderer to convert nodes from Component objects to useful String
	 * values before passing those String values on to the default renderer.
	 */
	static class KeyCellRenderer implements TreeCellRenderer {
		TreeCellRenderer renderer; // The renderer we are a wrapper for

		// Constructor: just remember the renderer
		public KeyCellRenderer(TreeCellRenderer renderer) {

			this.renderer = renderer;
		}

		// This is the only TreeCellRenderer method.
		// Compute the string to display, and pass it to the wrapped renderer
		public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded,
				boolean leaf, int row, boolean hasFocus) {
			KeyBean kb = ((KeyTreeModel) value).getKey();
			String newvalue = kb.getTyp() + " - " + kb.getKid() + " : " + kb.getKcv(); // Component
																						// type

			// Use the wrapped renderer object to do the real work
			return renderer.getTreeCellRendererComponent(tree, newvalue, selected, expanded, leaf, row, hasFocus);
		}
	}

	/**
	 * This main() method demonstrates the use of the ComponentTree class: it
	 * puts a ComponentTree component in a Frame, and uses the ComponentTree to
	 * display its own GUI hierarchy. It also adds a TreeSelectionListener to
	 * display additional information about each component as it is selected
	 */
	public static void main(String[] args) {
		// Create a frame for the demo, and handle window close requests
		JFrame frame = new JFrame("ComponentTree Demo");
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
		});

		// Create a scroll pane and a "message line" and add them to the
		// center and bottom of the frame.
		JScrollPane scrollpane = new JScrollPane();
		final JLabel msgline = new JLabel(" ");
		frame.getContentPane().add(scrollpane, BorderLayout.CENTER);
		frame.getContentPane().add(msgline, BorderLayout.SOUTH);

		// Now create the ComponentTree object, specifying the frame as the
		// component whose tree is to be displayed. Also set the tree's font.
		KeyTreeModel root = new KeyTreeModel(new KeyBean("99999999", "LMK", "abcdef9876543210", "D679A3"));
		root.addChild(new KeyBean("99999999", "TMK", "abcdef9876543210", "D679A3"));
		root.addChild(new KeyBean("12345678", "TMK", "abcdef9876543210", "D679A3"));
		root.addChild(new KeyBean("12345678", "TPK", "abcdef9876543210", "D679A3"));
		root.addChild(new KeyBean("88888888", "ZPK", "abcdef9876543210", "D679A3"));

		JTree tree = new JKeyTree(root);
		tree.setFont(new Font("SansSerif", Font.BOLD, 12));

		// Only allow a single item in the tree to be selected at once
		tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

		// Add an event listener for notifications when
		// the tree selection state changes.
		tree.addTreeSelectionListener(new TreeSelectionListener() {
			public void valueChanged(TreeSelectionEvent e) {
				TreePath path = e.getPath();
				KeyTreeModel c = (KeyTreeModel) path.getLastPathComponent();

				KeyBean kb = c.getKey();
				msgline.setText("KID: " + kb.getKid() + "  TYPE: " + kb.getTyp() + "  KCV: " + kb.getKcv());

			}
		});
		tree.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent me) {
				if (me.getClickCount() > 1) {
					TreePath path = tree.getPathForLocation(me.getX(), me.getY());
					KeyTreeModel c = (KeyTreeModel) path.getLastPathComponent();
					KeyBean kb = c.getKey();
					msgline.setText("KID: " + kb.getKid() + "  TYPE: " + kb.getTyp() + "  KCV: " + kb.getKcv());
				}
			}
		});
		// Now that we've set up the tree, add it to the scrollpane
		scrollpane.setViewportView(tree);

		// Finally, set the size of the main window, and pop it up.
		frame.setSize(600, 400);
		frame.setVisible(true);
	}
}