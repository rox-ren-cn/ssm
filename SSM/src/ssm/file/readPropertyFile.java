package ssm.file;

import java.io.*;

public class readPropertyFile {

    public String readProperty(String sFile, String sAttribute) {
        String s1, s2, s3 = null;
        BufferedReader bufferedreader;

        try {
            bufferedreader = new BufferedReader(new InputStreamReader(new FileInputStream(sFile)));
            while ((s1 = bufferedreader.readLine()) != null) {
                s1 = s1.trim();
                if (s1.length() > 2) {
                    int l;
                    for (l = 0; l < s1.length(); l++) {
                        if (!s1.substring(l, l + 1).equals(" ")) {
                            break;
                        }
                    }

                    s1 = s1.substring(l);
                    if (!s1.substring(0, 1).equals("#")
                            && !s1.substring(0, 1).equals("[")) {
                        int i = s1.indexOf('=');
                        if (i > 0) {
                            s2 = s1.substring(0, i).trim();
                            if (s2.equals(sAttribute)) {
                                s3 = s1.substring(i + 1).trim();
                                System.out.println(s2 + " === " + s3);
                                break;
                            }
                        }
                    }
                }
            }
            bufferedreader.close();
        } catch (FileNotFoundException filenotfoundexception) {
            try (FileOutputStream fileoutputstream = new FileOutputStream(sFile)) {
                byte abyte0[] = new byte[6];
                for (int k = 0; k < 5; k++) {
                    abyte0[k] = 57;
                }

                abyte0[5] = 57;
                fileoutputstream.write(abyte0);
            } catch (FileNotFoundException filenotfoundexception1) {
                System.out.println("FileNotFoundException ");
            } catch (IOException e) {
                System.out.println("IOException " + e.getMessage());
            }
            return null;
        } catch (IOException ioexception) {
            return null;
        }
        return s3;
    }
}
