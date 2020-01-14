import org.apache.lucene.util.SmallFloat;

public class test {
  /** Float-like encoding for positive longs that preserves ordering and 4 significant bits. 
   * It's copy pasted from Lucene's code.
   **/
  public static int longToInt4(long i) {
    if (i < 0) {
      throw new IllegalArgumentException("Only supports positive values, got " + i);
    }
    System.out.println("Adjusted value to encode:" + i);
    int numBits = 64 - Long.numberOfLeadingZeros(i);
    if (numBits < 4) {
      // subnormal value
      return Math.toIntExact(i);
    } else {
      // normal value
      int shift = numBits - 4;
      // only keep the 4 most significant bits
      int encoded = Math.toIntExact(i >>> shift);
      System.out.println("Shift value: " + shift + " encoded value: " + encoded);
      // clear the most significant bit, which is implicit
      encoded &= 0x07;
      // encode the shift, adding 1 because 0 is reserved for subnormal values
      encoded |= (shift + 1) << 3;
      return encoded;
    }
  }
  
  private static final int MAX_INT4 = longToInt4(Integer.MAX_VALUE);
  private static final int NUM_FREE_VALUES = 255 - MAX_INT4;
  
  /**
   * Encode an integer to a byte. It is built upon {@link #longToInt4(long)}
   * and leverages the fact that {@code longToInt4(Integer.MAX_VALUE)} is
   * less than 255 to encode low values more accurately.
   * It's copy pasted from Lucene's code.
   */
  public static byte intToByte4(int i) {
    System.out.println("Original value to encode:" + i);
    if (i < 0) {
      throw new IllegalArgumentException("Only supports positive values, got " + i);
    }
    if (i < NUM_FREE_VALUES) {
      return (byte) i;
    } else {
      return (byte) (NUM_FREE_VALUES + longToInt4(i - NUM_FREE_VALUES));
    }
  }
  
  public static void main(String [] args) {
    int prev = -1;
    for (int i = 0; i < 256; i++) {
      int curr = SmallFloat.byte4ToInt((byte)i);
      System.out.println(i + " decoded as " + curr + " diff from previous: " + (curr - prev));
      prev = curr;
    }
    System.out.println("===================");
    int testArr[] = {512, 15535, 19000, Integer.MAX_VALUE};
    System.out.println("NUM_FREE_VALUES: "+ NUM_FREE_VALUES);
    for (int i : testArr) {
      intToByte4(i);
      System.out.println("===================");
    }
  }
}
