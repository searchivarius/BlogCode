
/* First created by JCasGen Mon Feb 10 00:50:30 EST 2014 */
package edu.cmu.lti;

import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.JCasRegistry;
import org.apache.uima.cas.impl.CASImpl;
import org.apache.uima.cas.impl.FSGenerator;
import org.apache.uima.cas.FeatureStructure;
import org.apache.uima.cas.impl.TypeImpl;
import org.apache.uima.cas.Type;
import org.apache.uima.cas.impl.FeatureImpl;
import org.apache.uima.cas.Feature;
import org.apache.uima.jcas.tcas.Annotation_Type;

/** 
 * Updated by JCasGen Mon Feb 10 00:50:30 EST 2014
 * @generated */
public class Chunk_Type extends Annotation_Type {
  /** @generated */
  @Override
  protected FSGenerator getFSGenerator() {return fsGenerator;}
  /** @generated */
  private final FSGenerator fsGenerator = 
    new FSGenerator() {
      public FeatureStructure createFS(int addr, CASImpl cas) {
  			 if (Chunk_Type.this.useExistingInstance) {
  			   // Return eq fs instance if already created
  		     FeatureStructure fs = Chunk_Type.this.jcas.getJfsFromCaddr(addr);
  		     if (null == fs) {
  		       fs = new Chunk(addr, Chunk_Type.this);
  			   Chunk_Type.this.jcas.putJfsFromCaddr(addr, fs);
  			   return fs;
  		     }
  		     return fs;
        } else return new Chunk(addr, Chunk_Type.this);
  	  }
    };
  /** @generated */
  @SuppressWarnings ("hiding")
  public final static int typeIndexID = Chunk.typeIndexID;
  /** @generated 
     @modifiable */
  @SuppressWarnings ("hiding")
  public final static boolean featOkTst = JCasRegistry.getFeatOkTst("edu.cmu.lti.Chunk");
 
  /** @generated */
  final Feature casFeat_ChunkType;
  /** @generated */
  final int     casFeatCode_ChunkType;
  /** @generated */ 
  public String getChunkType(int addr) {
        if (featOkTst && casFeat_ChunkType == null)
      jcas.throwFeatMissing("ChunkType", "edu.cmu.lti.Chunk");
    return ll_cas.ll_getStringValue(addr, casFeatCode_ChunkType);
  }
  /** @generated */    
  public void setChunkType(int addr, String v) {
        if (featOkTst && casFeat_ChunkType == null)
      jcas.throwFeatMissing("ChunkType", "edu.cmu.lti.Chunk");
    ll_cas.ll_setStringValue(addr, casFeatCode_ChunkType, v);}
    
  



  /** initialize variables to correspond with Cas Type and Features
	* @generated */
  public Chunk_Type(JCas jcas, Type casType) {
    super(jcas, casType);
    casImpl.getFSClassRegistry().addGeneratorForType((TypeImpl)this.casType, getFSGenerator());

 
    casFeat_ChunkType = jcas.getRequiredFeatureDE(casType, "ChunkType", "uima.cas.String", featOkTst);
    casFeatCode_ChunkType  = (null == casFeat_ChunkType) ? JCas.INVALID_FEATURE_CODE : ((FeatureImpl)casFeat_ChunkType).getCode();

  }
}



    