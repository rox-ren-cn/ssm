package ssm.util;

import java.util.Arrays;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

public class Des {

	private static final Log log = LogFactory.getLog(Des.class);
	static {
		hexArray = "0123456789ABCDEF".toCharArray();
		try {
			cdes = Cipher.getInstance("DES/ECB/NoPadding");
			c3des = Cipher.getInstance("DESede/ECB/NoPadding");
		} catch (final NoSuchAlgorithmException | NoSuchPaddingException ex) {
			log.debug("Failed to init Des module");
			throw new RuntimeException("Failed to init Des module", ex);
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
			log.debug("des error:" + e.getMessage());
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
			log.debug("des3 error:" + e.getMessage());
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

	public static String GetPin(String pan, String pinblock) throws KeyException, DataException {
		String a = pan.trim();
		String b = String.format("%19s", a);
		String c = b.replace(' ', '0');
		String s = c.substring(6, 18);
		byte[] pc = toBytes("0000" + s);
		logic(logic_op.xor, pc, pinblock.getBytes());
		if (pc[1] < 4 || pc[1] > 12)
			throw new DataException("Invalid PIN Block");
		return new String(pc, 2, pc[1]);
	}

	// Convert double length key to triple length key for DESede spec
	public static byte[] toKey(String key) {
		return toKey(toBytes(key));
	}

	public static byte[] toKey(byte[] key) {
		byte[] key3 = new byte[24];
		System.arraycopy(key, 0, key3, 0, 16);
		System.arraycopy(key3, 0, key3, 16, 8);
		return key3;
	}

	public static String BDKEnc(String lmk) {
		return toHexStr(des3(enc_op.enc, toKey(BDK), toBytes(lmk)));
	}

	public static String BDKDec(String lmk) {
		return toHexStr(des3(enc_op.dec, toKey(BDK), toBytes(lmk)));
	}

	public static String LMKEnc(String lmk) {
		return toHexStr(des3(enc_op.enc, toKey(BDKDec(LMK)), toBytes(lmk)));
	}

	public static String LMKDec(String lmk) {
		return toHexStr(des3(enc_op.dec, toKey(BDKDec(LMK)), toBytes(lmk)));
	}

	// Key and text are encrypted by LMK
	public static String Enc2(String key, String text) {
		byte[] clearLMK = des3(enc_op.dec, toKey(BDK), toBytes(LMK));
		byte[] clearKey = des3(enc_op.dec, toKey(clearLMK), toBytes(key));
		byte[] clearTxt = des3(enc_op.dec, toKey(clearLMK), toBytes(text));
		return toHexStr(des3(enc_op.enc, toKey(clearKey), clearTxt));
	}

	// Key and text are encrypted by LMK
	public static String Dec2(String key, String text) {
		byte[] clearLMK = des3(enc_op.dec, toKey(BDK), toBytes(LMK));
		byte[] clearKey = des3(enc_op.dec, toKey(clearLMK), toBytes(key));
		byte[] clearTxt = des3(enc_op.dec, toKey(clearLMK), toBytes(text));
		return toHexStr(des3(enc_op.dec, toKey(clearKey), clearTxt));
	}

	public static String Enc(String key, String text) {
		return toHexStr(des3(enc_op.enc, toKey(key), toBytes(text)));
	}

	public static String Dec(String key, String text) {
		return toHexStr(des3(enc_op.dec, toKey(key), toBytes(text)));
	}

	public static String randomKey() {
		SecureRandom random = new SecureRandom();
		byte bKey[] = new byte[16];
		random.nextBytes(bKey);
		return toHexStr(bKey);
	}

	private final static String BDK = "0123456789ABCDEFFEDCBA9876543210";
	private static String LMK = null;

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

	public static String logic(logic_op xor, String part1, String part2) {
		byte[] p1 = new byte[part1.length() / 2];
		p1 = toBytes(part1);
		logic(xor, p1, toBytes(part2));
		return toHexStr(p1);
	}
}
