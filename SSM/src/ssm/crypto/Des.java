package ssm.crypto;

import ssm.tools.LogUtil;

import java.util.Arrays;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

public class Des {

	static {
		hexArray = "0123456789ABCDEF".toCharArray();
		try {
			cdes = Cipher.getInstance("DES/ECB/NoPadding");
			c3des = Cipher.getInstance("DESede/ECB/NoPadding");
		} catch (final NoSuchAlgorithmException | NoSuchPaddingException ex) {
			throw new RuntimeException("Failed to init DUKT", ex);
		}
	}

	public static byte[] toBytes(String s) {
		int len = s.length();
		byte[] data = new byte[len / 2];
		for (int i = 0; i < len; i += 2) {
			data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character.digit(s.charAt(i + 1), 16));
		}
		return data;
	}

	public static String toHexStr(byte[] buf) {
		char[] hexChars = new char[buf.length * 2];
		int v;

		for (int j = 0; j < buf.length; j++) {
			v = buf[j] & 0xFF;
			hexChars[j * 2] = hexArray[v >>> 4];
			hexChars[j * 2 + 1] = hexArray[v & 0x0F];
		}

		return new String(hexChars);
	}

	public static void logic(logic_op op, byte[] a, byte[] b) {
		int la = a.length;
		int lb = b.length;
		// b is right aligned to a
		if (lb > la) {
			b = Arrays.copyOfRange(b, lb - la, lb);
			lb = b.length;
		}
		int lc = la > lb ? la - lb : 0;

		for (int i = 0; i < la; ++i) {
			if (i < lc) {
				switch (op) {
				// padding zero to left of b
				case and: // A & 0 = 0
					a[i] = 0;
					break;
				case or: // A | 0 = A
					break;
				case xor: // A ^ 0
					a[i] ^= 0;
					break;
				}
			} else {
				switch (op) {
				case and:
					a[i] &= b[i - lc];
					break;
				case or:
					a[i] |= b[i - lc];
					break;
				case xor:
					a[i] ^= b[i - lc];
					break;
				}
			}
		}
	}

	public static byte[] des(enc_op op, byte[] key, byte[] data) {
		SecretKeySpec ks = new SecretKeySpec(key, "DES");

		try {
			switch (op) {
			case enc:
				cdes.init(Cipher.ENCRYPT_MODE, ks);
				break;
			case dec:
				cdes.init(Cipher.DECRYPT_MODE, ks);
				break;
			}
			return cdes.doFinal(data);
		} catch (InvalidKeyException | IllegalBlockSizeException | BadPaddingException e) {
			LogUtil.d(e.getLocalizedMessage(), e.getMessage());
		}
		return null;
	}

	public static byte[] des3(enc_op op, byte[] key, byte[] data) {
		SecretKeySpec ks = new SecretKeySpec(key, "DESede");
		try {
			switch (op) {
			case enc:
				c3des.init(Cipher.ENCRYPT_MODE, ks);
				break;
			case dec:
				c3des.init(Cipher.DECRYPT_MODE, ks);
				break;
			}
			return c3des.doFinal(data);
		} catch (InvalidKeyException | IllegalBlockSizeException | BadPaddingException e) {
			LogUtil.d(e.getLocalizedMessage(), e.getMessage());
		}
		return null;
	}

	public static byte[] GetPinBlock(String pan, String pin) {
		// byte[] p1 = toBytes("0000" + String.format("%016s", pan).substring(3,
		// 14));
		// logic(logic_op.xor, p1, toBytes(String.format("%-16s", "0" + ('0' +
		// pin.length()) + pin + "").replace(' ', 'F')));
		// return p1;
		return toBytes(String.format("%-16s", "0" + Integer.toString(pin.length()) + pin).replace(' ', 'F'));
	}

	public static String EncLMK(String lmk) {
		byte[] x = toBytes(lmk);
		byte[] xx = des3(enc_op.enc, bdk, x);
		LMK = toHexStr(xx);
		LMK += LMK.substring(0, 16);
		return LMK;
	}

	public static String DecLMK(String lmk) {
		return toHexStr(des3(enc_op.dec, toBytes(BDK), toBytes(lmk)));
	}

	public static String Enc(String text) {
		byte[] x = toBytes(text);
		byte[] lmk = des3(enc_op.dec, bdk, toBytes(LMK));
		byte[] xx = des3(enc_op.enc, lmk, x);
		return toHexStr(xx);
	}

	public static String Dec(String text) {
		return toHexStr(des3(enc_op.dec, des3(enc_op.dec, toBytes(BDK), toBytes(LMK)), toBytes(text)));
	}

	public static String Enc(String key, String text) {
		return toHexStr(des3(enc_op.enc, toBytes(key), toBytes(text)));
	}

	public static String Dec(String key, String text) {
		return toHexStr(des3(enc_op.dec, toBytes(key), toBytes(text)));
	}

	public static String decryptText(String key, String text) {
		return toHexStr(des3(enc_op.dec, toBytes(key), toBytes(text)));
	}

	private final static String BDK = "0123456789ABCDEFFEDCBA98765432100123456789ABCDEF";
	private final static byte[] bdk = toBytes(BDK);
	private static String LMK = null;

	public static String getLMK() {
		return LMK;
	}

	public static void setLMK(String lMK) {
		LMK = lMK;
	}

	final protected static char[] hexArray;

	public static enum logic_op {
		and, or, xor
	};

	public static enum enc_op {
		enc, dec
	};

	private static final Cipher cdes, c3des;
}
