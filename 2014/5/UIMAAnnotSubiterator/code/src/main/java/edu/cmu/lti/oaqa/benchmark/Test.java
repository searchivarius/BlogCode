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

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.resource.ResourceInitializationException;

import org.cleartk.token.type.Sentence;


import edu.cmu.lti.POS;

/**
 * 
 * Testing efficiency of subiterator in UIMA.
 * 
 * @author Leonid Boytsov
 *
 */
public class Test extends JCasAnnotator_ImplBase {
  boolean mUseSubiter = false;
  
  @Override
  public void initialize(UimaContext aContext)
  throws ResourceInitializationException {
    super.initialize(aContext);
    
    mUseSubiter = (Boolean) aContext.getConfigParameterValue("USE_SUBITER");
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
       
       if (mUseSubiter) {
         FSIterator<Annotation> subIter = AnnotUtils.getSubIterator(aJCas,
                                                               POS.typeIndexID,
                                                               sentence, true);         
         while (subIter.isValid()) {
           ++qty;
           subIter.moveToNext();
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
    System.out.println("Use index: " + mUseSubiter + " time: " + diff + 
                       " ms, # of POS tags: " + qty + 
                       " spent one annot: " + (diff/qty) + " ms");
    System.out.println("\n\n===============================================");
  }
}
