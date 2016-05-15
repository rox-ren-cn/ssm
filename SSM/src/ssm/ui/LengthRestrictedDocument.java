package ssm.ui;

import javax.swing.text.*;

public final class LengthRestrictedDocument extends PlainDocument {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private final int limit;
	private final int mode;

	public LengthRestrictedDocument(int limit, int mode) {
		this.limit = limit;
		this.mode = mode;
	}

	@Override
	public void insertString(int offs, String str, AttributeSet a) throws BadLocationException {
		if (str == null)
			return;

		if ((getLength() + str.length()) <= limit) {
			super.insertString(offs, str, a);
		}

		// Input Hex only
		if (mode == 1 && test(str)) {

		}
	}

	private boolean test(String text) {
		try {
			Integer.parseInt(text);
			return true;
		} catch (NumberFormatException e) {
			return false;
		}
	}

}