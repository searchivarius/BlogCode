/*
 * HTML-cleaner annotator
 * 
 * It is a proxy class that can use various cleaners
 * as long as they are inherited from the AbstractHtmlCleaner and
 * have a parameter-less constructor.
 *
 * Author: Leonid Boytsov
 * Copyright (c) 2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

package cleaner;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.jcas.JCas;
import org.apache.uima.resource.ResourceInitializationException;

import ToolsNLP.*;


/*
 * This class saves cleaned HTML as a separate annotation.
 */

public class HtmlCleanAnnotator extends JCasAnnotator_ImplBase {
  private AbstractHtmlCleaner workHorse;
  /**
   * @see AnalysisComponent#initialize(UimaContext)
   */
  public void initialize(UimaContext aContext) throws ResourceInitializationException {
    super.initialize(aContext);
    // Get config. parameter values
    String className = (String) aContext.getConfigParameterValue("HtmlCleanerClass");
    
    workHorse = AbstractHtmlCleaner.createInstance(className, aContext);
  }  
  /**
   * @see JCasAnnotator_ImplBase#process(JCas)
   */
  public void process(JCas aJCas) {
    // get HTML
    String html = aJCas.getDocumentText();
    

    // TODO: not all files may have the utf-8 encoding
    String cleanedText = workHorse.CleanText(html, "UTF-8"); 

    // Create annotation
    Document annotation = new Document(aJCas, 0, cleanedText.length());
    annotation.setText(cleanedText);
    annotation.addToIndexes();
  }

}
