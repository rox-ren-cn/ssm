package ssm.util;

import java.util.ArrayList;
import java.util.List;

public class KeyTree {
	boolean isLeaf;
	private KeyBean key;
	private List<KeyTree> children;

	private KeyTree TMK;

	public KeyTree(KeyBean lmk) {
		isLeaf = true;
		key = lmk;
		children = new ArrayList<KeyTree>();
	}

	public boolean isLeaf() {
		return isLeaf;
	}

	public void addChild(KeyBean kb) {
		isLeaf = false;
		KeyTree kt = new KeyTree(kb);
		switch (kb.getKeyType()) {
		case "LMK":
			// LMK can't be child
			return;
		case "TMK":
			children.add(kt);
			if (TMK == null && kb.isTMK()) {
				TMK = kt;
			}
			break;
		case "ATM":
			if (TMK != null) {
				TMK.addChild(kb);
			} else {
				// No TMK.INIT, this is an branch
				children.add(kt);
				return;
			}
			break;
		case "ZPK":
		default:
			children.add(kt);
			break;
		}

		return;
	}

	public KeyBean getKey() {
		return key;
	}

	public int getChildCount() {
		return children.size();
	}

	public KeyTree getChild(int index) {
		return children.get(index);
	}

	public KeyTree[] getChildren() {
		return (KeyTree[]) children.toArray();
	}
	
	public KeyBean findKey(String kid, String typ) {
		if (kid.equals(key.getKid()) && typ.equals(key.getTyp())) {
			return key;
		}
		for (KeyTree kt : children) {
			return kt.findKey(kid, typ);
		}
		return null;
	}

	public void initTMK(String tmk) {
		// remove all keys under TMK.INIT
		// 
		
	}
}
