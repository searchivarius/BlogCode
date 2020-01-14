import org.apache.lucene.util.SmallFloat;

public class test {
  public static void main(String [] args) {
    int prev = -1;
    for (int i = 0; i < 256; i++) {
      int curr = SmallFloat.byte4ToInt((byte)i);
      System.out.println(i + " decoded as " + curr + " diff from previous: " + (curr - prev));
      prev = curr;
    }
  }
}
