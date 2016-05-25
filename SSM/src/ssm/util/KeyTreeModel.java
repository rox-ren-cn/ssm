package ssm.util;

import java.util.ArrayList;
import java.util.List;

import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;


public class KeyTreeModel implements TreeModel {
	private KeyBean key;
	private List<KeyTreeModel> children;

	public KeyTreeModel(KeyBean kb) {
		key = kb;
		children = null;
	}

	public KeyTreeModel addChild(KeyBean kb) {
		if (children == null)
			children = new ArrayList<KeyTreeModel>();

		KeyTreeModel kt = new KeyTreeModel(kb);
		children.add(kt);
		return kt;
	}

	public Object getRoot() {
		return this;
	}

	public boolean isLeaf() {
		return children == null || children.isEmpty();
	}

	public boolean isLeaf(Object node) {
		if (!(node instanceof KeyTreeModel))
			return true;
		KeyTreeModel c = (KeyTreeModel) node;
		return c.isLeaf();
	}

	public KeyBean getKey() {
		return key;
	}

	public int getChildCount() {
		return children.size();
	}

	public int getChildCount(Object node) {
		if (node instanceof KeyTreeModel) {
			KeyTreeModel c = (KeyTreeModel) node;
			return c.getChildCount();
		}
		return 0;
	}

	public KeyTreeModel getChild(int index) {
		return children.get(index);
	}

	public Object getChild(Object parent, int index) {
		if (parent instanceof KeyTreeModel) {
			KeyTreeModel c = (KeyTreeModel) parent;
			return c.getChild(index);
		}
		return getChild(index);
	}

	public KeyTreeModel[] getChildren() {
		return (KeyTreeModel[]) children.toArray();
	}
	
	public int getIndexOfChild(Object parent, Object child) {
		if (!(parent instanceof KeyTreeModel))
			return -1;
		KeyTreeModel c = (KeyTreeModel) parent;
		KeyTreeModel[] children = c.getChildren();
		if (children == null)
			return -1;
		for (int i = 0; i < children.length; i++) {
			if (children[i] == child)
				return i;
		}
		return -1;
	}

	@Override
	public void valueForPathChanged(TreePath path, Object newValue) {
	}

	@Override
	public void addTreeModelListener(TreeModelListener l) {
	}

	@Override
	public void removeTreeModelListener(TreeModelListener l) {
	}

}
