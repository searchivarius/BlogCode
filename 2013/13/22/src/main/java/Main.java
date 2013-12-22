import java.util.Arrays;

public class Main {

  public static void main(String[] args) {
    if (args.length != 1) {
      System.err.println("Show supply the # of strings");
      System.exit(1);
    }
    int qty = Integer.parseInt(args[0]);
    int maxLen = 60;
    int minLen = 40;
    
    char buf[] = new char [maxLen];
    
    Arrays.fill(buf, 'a');
    
    URL2DocID arr[] = new URL2DocID[qty];
    
    int sz = 0;
    
    for (int i = 0; i < qty; ++i) {
      int len = minLen + (int)Math.ceil((maxLen - minLen) * Math.random());
      
      sz += 2*len + 4;
      
      arr[i] = new URL2DocID(new String(buf, 0, len), i);
    }    
    
    System.out.println("Before sorting!");
    Arrays.sort(arr);
    System.out.println("Sorting is done, total size of strings and ints: " 
                        + sz/1024.0/1024.0/1024.0 + " Gb");
  }

}
