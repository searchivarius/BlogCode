/*
 * A more sophisticated HTML-cleaner
 *
 * Author: Leonid Boytsov
 * Copyright (c) 2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

package cleaner;

import org.apache.uima.UimaContext;
import org.apache.uima.resource.ResourceInitializationException;
import org.htmlparser.Parser;
import org.htmlparser.util.*;


public class LeoHtmlCleaner extends AbstractHtmlCleaner {
  public LeoHtmlCleaner(UimaContext aContext) throws ResourceInitializationException {
  }

  @Override
  public String CleanText(String html, String encoding) {
    String baseHref = "http://fake-domain.com";
    
    LeoCleanerUtil   res = null;

    
    try {
      Parser HtmlParser = Parser.createParser(html, encoding);

      res = new LeoCleanerUtil(baseHref);      
      HtmlParser.visitAllNodesWith(res);
    } catch (ParserException e) {
      System.err.println(" Parser exception: " + e + " trying simple conversion");
      res = null;
    }           

    if (res != null) {
      return res.GetBodyText();
    }  
    // Plan B!!!
    Pair<String,String> sres = LeoCleanerUtil.SimpleProc(html);

    return sres.getSecond();
  }
  
}
