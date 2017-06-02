

/* First created by JCasGen Mon Feb 10 00:50:30 EST 2014 */
package edu.cmu.lti;

import org.apache.uima.jcas.JCas; 
import org.apache.uima.jcas.JCasRegistry;
import org.apache.uima.jcas.cas.TOP_Type;

import org.apache.uima.jcas.tcas.Annotation;


/** Part-Of-Speach (POS) tag.
 * Updated by JCasGen Mon Feb 10 00:50:30 EST 2014
 * XML source: /home/leo/SourceTreeGit/ToolsNLP/Project/src/main/resources/types/SennaTypes.xml
 * @generated */
public class POS extends Annotation {
  /** @generated
   * @ordered 
   */
  @SuppressWarnings ("hiding")
  public final static int typeIndexID = JCasRegistry.register(POS.class);
  /** @generated
   * @ordered 
   */
  @SuppressWarnings ("hiding")
  public final static int type = typeIndexID;
  /** @generated  */
  @Override
  public              int getTypeIndexID() {return typeIndexID;}
 
  /** Never called.  Disable default constructor
   * @generated */
  protected POS() {/* intentionally empty block */}
    
  /** Internal - constructor used by generator 
   * @generated */
  public POS(int addr, TOP_Type type) {
    super(addr, type);
    readObject();
  }
  
  /** @generated */
  public POS(JCas jcas) {
    super(jcas);
    readObject();   
  } 

  /** @generated */  
  public POS(JCas jcas, int begin, int end) {
    super(jcas);
    setBegin(begin);
    setEnd(end);
    readObject();
  }   

  /** <!-- begin-user-doc -->
    * Write your own initialization here
    * <!-- end-user-doc -->
  @generated modifiable */
  private void readObject() {/*default - does nothing empty block */}
     
 
    
  //*--------------*
  //* Feature: Tag

  /** getter for Tag - gets An actual POS-tag, e.g., NN or VP
   * @generated */
  public String getTag() {
    if (POS_Type.featOkTst && ((POS_Type)jcasType).casFeat_Tag == null)
      jcasType.jcas.throwFeatMissing("Tag", "edu.cmu.lti.POS");
    return jcasType.ll_cas.ll_getStringValue(addr, ((POS_Type)jcasType).casFeatCode_Tag);}
    
  /** setter for Tag - sets An actual POS-tag, e.g., NN or VP 
   * @generated */
  public void setTag(String v) {
    if (POS_Type.featOkTst && ((POS_Type)jcasType).casFeat_Tag == null)
      jcasType.jcas.throwFeatMissing("Tag", "edu.cmu.lti.POS");
    jcasType.ll_cas.ll_setStringValue(addr, ((POS_Type)jcasType).casFeatCode_Tag, v);}    
  }

    