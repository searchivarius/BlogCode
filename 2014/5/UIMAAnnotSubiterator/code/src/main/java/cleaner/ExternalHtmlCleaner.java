/*
 * HTML-cleaner based on an external script
 *
 * Author: Leonid Boytsov
 * Copyright (c) 2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

package cleaner;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.StringWriter;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.uima.UimaContext;
import org.apache.uima.resource.ResourceInitializationException;

public class ExternalHtmlCleaner extends AbstractHtmlCleaner {
  
  private String extScript;
  
  public ExternalHtmlCleaner(UimaContext aContext) throws ResourceInitializationException {
    try {
      extScript = (String) aContext.getConfigParameterValue("extScript");
      if (extScript == null) {
        throw new Exception("The parameter extScript is not specified!");
      }
      System.out.println("Using script: " + extScript);
    } catch (Exception e) {
      System.err.println("Initialization error: " + e);
      throw new ResourceInitializationException(e);
    }
  }  

  @Override
  public String CleanText(String html, String encoding) {
    try {
      File      tempIn = File.createTempFile("ExternalParserInput", ".tmp");
      String    tempInName = tempIn.getCanonicalPath();
      File      tempOut = File.createTempFile("ExternalParserOutput", ".tmp");
      String    tempOutName = tempOut.getCanonicalPath();      
      
      FileUtils.writeStringToFile(tempIn, html, encoding);
      
      String cmd = extScript + " " + tempInName + " " + tempOutName;
      System.out.println("Executing: " + cmd);
      Process proc = Runtime.getRuntime().exec(cmd);
      
      if (proc == null) {
        System.err.println("Cannot execute command: " + extScript);
        return null;
      }

      StringWriter err = new StringWriter();
      IOUtils.copy(proc.getErrorStream(), err, encoding);
      
      String ErrStr = err.toString();
      
      if (!ErrStr.isEmpty()) {
        System.err.println("External script " + extScript + " returned errors:");
        System.err.println(ErrStr);
        
        throw new Exception("External script " + extScript + " returned errors");
      }
            
      String out = FileUtils.readFileToString(tempOut);
      
      tempIn.delete();
      tempOut.delete();
      
      return LeoCleanerUtil.CollapseSpaces(out);
    } catch (Exception e) {
      System.err.println("Failed to run the script " + extScript + " Error: " + e);
      System.exit(1);
    }
    return null;
  }

}
