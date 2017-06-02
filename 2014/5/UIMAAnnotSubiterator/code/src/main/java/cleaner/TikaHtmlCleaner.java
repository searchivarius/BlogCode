/*
 * HTML-cleaner that is based on Tika
 *
 * Author: Leonid Boytsov, based on the code of chris jordan
 * published at http://chrisjordan.ca/post/15219674437/parsing-html-with-apache-tika
 *
 * Copyright (c) 2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

package cleaner;

import java.io.IOException;
import java.io.InputStream;

import org.apache.commons.io.IOUtils;
import org.apache.tika.sax.*;
import org.apache.tika.exception.TikaException;
import org.apache.tika.metadata.*;
import org.apache.tika.parser.*;
import org.apache.tika.parser.html.HtmlParser;
import org.apache.uima.UimaContext;
import org.apache.uima.resource.ResourceInitializationException;


public class TikaHtmlCleaner extends AbstractHtmlCleaner {

  public TikaHtmlCleaner(UimaContext aContext) throws ResourceInitializationException {
  }

  @Override
  public String CleanText(String html, String encoding) {
    InputStream input;
    try {
      input = IOUtils.toInputStream(html, encoding);
    } catch (IOException e) {
      System.err.println("Error parsing: " + e);
      return null;
    }
    LinkContentHandler linkHandler = new LinkContentHandler();
    BodyContentHandler textHandler = new BodyContentHandler(-1); // -1 disables content limit
    ToHTMLContentHandler toHTMLHandler = new ToHTMLContentHandler();
    TeeContentHandler teeHandler = new TeeContentHandler(linkHandler, textHandler, toHTMLHandler);
    Metadata metadata = new Metadata();
    metadata.set(Metadata.CONTENT_TYPE, "text/html"); 
    metadata.set(Metadata.CONTENT_ENCODING, encoding); 
    ParseContext parseContext = new ParseContext();
    HtmlParser parser = new HtmlParser();
    try {
      parser.parse(input, teeHandler, metadata, parseContext);
    } catch (Exception e) {
      System.err.println("Error parsing: " + e);
      return null;
    }
    /*
    System.out.println("title:\n" + metadata.get("title"));
    System.out.println("links:\n" + linkHandler.getLinks());
    System.out.println("text:\n" + textHandler.toString());
    System.out.println("html:\n" + toHTMLHandler.toString());
    */
    return LeoCleanerUtil.CollapseSpaces(textHandler.toString());
  }
}
