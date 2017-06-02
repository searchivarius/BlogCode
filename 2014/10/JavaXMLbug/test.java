import java.io.*;

import javax.xml.parsers.DocumentBuilder; 
import javax.xml.parsers.DocumentBuilderFactory;


import org.w3c.dom.*;
import org.xml.sax.*;


public class test {
  public static void main(String []args) {
    try {
      DocumentBuilder dbld = DocumentBuilderFactory.newInstance().newDocumentBuilder();
  
      System.out.println(args[0]);

      Document doc = dbld.parse(new File(args[0]));
  
      // Get the document's root XML node
      NodeList root = doc.getChildNodes();
 
      String text = getNodeValue("text", getNode("DOC", root).getChildNodes());
  
      System.out.println("=========================");
      System.out.println(text);
      System.out.println("=========================");
    }
    catch ( Exception e ) {
    e.printStackTrace();
  }
  }

  static String getNodeValue(String tagName, NodeList nodes ) {
    for ( int x = 0; x < nodes.getLength(); x++ ) {
        Node node = nodes.item(x);
        System.out.println("@@@" + node.getNodeName());
        if (node.getNodeName().equalsIgnoreCase(tagName)) {
            NodeList childNodes = node.getChildNodes();
            for (int y = 0; y < childNodes.getLength(); y++ ) {
                Node data = childNodes.item(y);
                if ( data.getNodeType() == Node.TEXT_NODE )
                    return data.getNodeValue();
            }
        }
    }
    return "";
  }

  static Node getNode(String tagName, NodeList nodes) {
    for ( int x = 0; x < nodes.getLength(); x++ ) {
        Node node = nodes.item(x);
        if (node.getNodeName().equalsIgnoreCase(tagName)) {
            return node;
        }
    }
 
    return null;
  }
}
