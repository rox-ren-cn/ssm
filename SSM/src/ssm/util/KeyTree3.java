package ssm.util;

import java.util.ArrayList;
import java.util.List;

public class KeyTree3 {
	private KeyBean key;
	private List<KeyTree3> children;

	public KeyTree3(KeyBean kb) {
		key = kb;
		children = null;
	}

	public boolean isLeaf() {
		return children == null || children.isEmpty();
	}

	public KeyTree3 addChild(KeyBean kb) {
		if (children == null)
			children = new ArrayList<KeyTree3>();

		KeyTree3 kt = new KeyTree3(kb);
		children.add(kt);
		return kt;
	}

	public KeyBean getKey() {
		return key;
	}

	public int getChildCount() {
		return children.size();
	}

	public KeyTree3 getChild(int index) {
		return children.get(index);
	}

	public KeyTree3[] getChildren() {
		return (KeyTree3[]) children.toArray();
	}
}
