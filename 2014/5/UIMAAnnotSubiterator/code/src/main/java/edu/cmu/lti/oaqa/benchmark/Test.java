/*
 *  Copyright 2013 Carnegie Mellon University
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package edu.cmu.lti.oaqa.benchmark;

import java.util.List;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.CAS;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.text.AnnotationFS;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.resource.ResourceInitializationException;
import org.cleartk.token.type.Sentence;
import org.uimafit.util.CasUtil;

import edu.cmu.lti.POS;

/**
 * 
 * Testing efficiency of subiterator in UIMA and selectCovered in UIMA-fit.
 * 
 * @author Leonid Boytsov
 *
 */
public class Test extends JCasAnnotator_ImplBase {
  Integer mSubiterChoice = 0;
  
  @Override
  public void initialize(UimaContext aContext)
  throws ResourceInitializationException {
    super.initialize(aContext);
    
    mSubiterChoice = (Integer) aContext.getConfigParameterValue("SUBITER_CHOICE");
  }  
  
  @Override
  public void process(JCas aJCas)
  throws AnalysisEngineProcessException {
    long start = System.currentTimeMillis();
    long qty = 0;
    
    try {
      FSIterator<Annotation> sentenceIterator = AnnotUtils.getIterator(aJCas,
                                                        Sentence.typeIndexID);
      
      for (int counter = 1;
          sentenceIterator.isValid();
          counter++, sentenceIterator.moveToNext()) {
       Annotation  sentence = sentenceIterator.get();
       
       if (mSubiterChoice == 1) {
         FSIterator<Annotation> subIter = AnnotUtils.getSubIterator(aJCas,
                                                               POS.typeIndexID,
                                                               sentence, true);         
         while (subIter.isValid()) {
           ++qty;
           subIter.moveToNext();
         }
       
       } else if (mSubiterChoice == 2) {
    	 CAS cas = aJCas.getCas();
         List<AnnotationFS> subIter = CasUtil.selectCovered(
        		 											   cas,
        		 											   CasUtil.getType(cas, POS.class),
                                                               sentence);
         
         for (AnnotationFS a:subIter) {
           ++qty;
         }
       } else if (mSubiterChoice == 3) {
    	 CAS cas = aJCas.getCas();
    	 /* This one is supposed to be substantially slower !!!! */
         List<AnnotationFS> subIter = CasUtil.selectCovered(
        		 											   cas,
        		 											   CasUtil.getType(cas, POS.class),
                                                               sentence.getBegin(),sentence.getEnd());
         
         for (AnnotationFS a:subIter) {
           ++qty;
         }
       } else {
         FSIterator<Annotation> subIter = AnnotUtils.getIterator(aJCas,
                                                                POS.typeIndexID);
         while (subIter.isValid()) {
           Annotation an = subIter.get();
           if (an.getBegin() >= sentence.getBegin() && 
               an.getEnd() <= sentence.getEnd()) {
             ++qty;
           }
           subIter.moveToNext();
         }
         
       }              
      }

    
    } catch (Exception e) {
      throw new AnalysisEngineProcessException(e);
    }   
    
    float diff = System.currentTimeMillis() - start;
    
    System.out.println("===============================================\n");
    System.out.println("Subiter choice index: " + mSubiterChoice + " time: " + diff + 
                       " ms, # of POS tags: " + qty + 
                       " spent one annot: " + (diff/qty) + " ms");
    System.out.println("\n\n===============================================");
  }
}
