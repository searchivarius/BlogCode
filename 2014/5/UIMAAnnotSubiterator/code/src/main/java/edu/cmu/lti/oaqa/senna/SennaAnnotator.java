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
package edu.cmu.lti.oaqa.senna;

import java.io.*;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;
import java.util.regex.Pattern;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.FSIndex;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.FeatureStructure;
import org.apache.uima.util.*;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.cas.FSArray;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.resource.ResourceInitializationException;

import edu.cmu.lti.SemanticRole;
import edu.cmu.lti.POS;
import edu.cmu.lti.NamedEntity;
import edu.cmu.lti.Chunk;

 
/**
 * A tool to parse SENNA's output: chunking, NEs, syntax, SRLs
 * Currently, SENNA is invoked from the command line.
 * Note that SENNA *DOES* have a socket server mode, which may be used in the future.
 * However, it takes only a fraction of a second to load SENNA, so 
 * invoking it from the command line is fine as long as you process batches
 * containing more than about 10 sentences.
 * 
 * @author Leonid Boytsov
 * @author Andrew Hazen Schlaikjer
 * @author Eric Riebling 
 */
public class SennaAnnotator
extends JCasAnnotator_ImplBase
{
	
  	private enum AnnotType {
  		POS,	    // pos-tags
  		CHK,      // chunking
  		NER,      // named entities
  		SRL 	    // semantic role-labeling
  	};
	
    private static final String SENNA_OUTPUT = ".senna-output";
    private static final String SENNA_INPUT = ".senna-input";
    // Apparently using UTF-8 leads to a trouble, stick to ISO-8859-1 (Latin1).
    private static final String ENCODING = "ISO-8859-1";
    /*
     *  Don't collapse several spaces into a single space,
     *  this screws up offsets!
     *  
     *  However, we need to replace newlines with spaces. 
     *  
     */    
    private static Pattern pattern_whitespace = Pattern.compile("[\\s\n\r\t]");
    private static Pattern pattern_SENNA_bad_sentence = Pattern.compile("\\s*\\.\\s*");

    private File              tmpdir;
    private String            senna_binary;
    private String            senna_home;
    private boolean           remove_tmp_files;
    private String            sentence_annot_type;
    private int               sentence_max_len = 1023; // Senna Default
    private Set<AnnotType>    annot_types;

    /**
     * Constructs an instance of this annotator
     */
    public SennaAnnotator() {
      annot_types = new HashSet<AnnotType>();
    }

    /**
     * @return the type string of this annotator
     */
    public static String getType()
    { return "SENNA"; }

    /**
     * @return the version number of this annotator
     */
    public double getVersion()
    { return 1.0; }
    
    /**
     * Configuration parameters:
     * <ul>
     * <li>tmpdir - a directory to store temporary files.</li>
     * <li>senna_home - the name of a SENNA home directory.</li>
     * <li>senna_binary - the name of a SENNA executable.</li>
     * <li>senna_annot - the list of annotation types.</li>
     * </ul>
     * 
     */
    public final String SENNA_ANNOT   = "senna_annot";
    public final String TMPDIR        = "tmpdir";
    public final String SENNA_BINARY  = "senna_binary";
    public final String SENNA_HOME    = "senna_home";
    public final String SENTENCE_ANNOT_TYPE = "sentence_annot_type";
    public final String SENTENCE_MAX_LEN = "sentence_max_len";
    
    @Override
    public void initialize(UimaContext aContext)
    throws ResourceInitializationException
    {
        super.initialize(aContext);
        
        getContext().getLogger().setLevel(Level.INFO);
        
        String config_tmpdir = (String) aContext.getConfigParameterValue(TMPDIR);
        String config_binary = (String) aContext.getConfigParameterValue(SENNA_BINARY);
        String config_home   = (String) aContext.getConfigParameterValue(SENNA_HOME);
        String config_annot  = (String) aContext.getConfigParameterValue(SENNA_ANNOT);
        sentence_annot_type = (String) aContext.getConfigParameterValue(SENTENCE_ANNOT_TYPE);
        Integer config_sent_max_len = (Integer) aContext.getConfigParameterValue(SENTENCE_MAX_LEN);
        
        if (config_annot != null) {
          for (String oneAnnot: config_annot.trim().split(",")) {
            try {
                annot_types.add(AnnotType.valueOf(oneAnnot));
            } catch(IllegalArgumentException ex) {
                throw new ResourceInitializationException(new Exception("Invalid annotator type: " + oneAnnot)); 
            }
          }
        }
        
        if (annot_types.isEmpty()) {
            throw new ResourceInitializationException(
                new Exception("Specify at least annotator type! Parameter: '" 
                               + SENNA_ANNOT + "'"));
        }
        
        if (config_tmpdir != null) {
            tmpdir = new File(config_tmpdir);
            if (!tmpdir.exists()) {
                System.out.println("Specified '" + TMPDIR + 
                                   "' path does not exist: creating "+tmpdir);
                tmpdir.mkdirs();
            }
        } else {

            tmpdir = new File(System.getProperty("java.io.tmpdir"));
            System.out.println("Using default value for '" + TMPDIR + 
                               "' from system.io.tmpdir: " + tmpdir);
        }
        if (!tmpdir.exists()) {
          System.out.println("Specified 'tmpdir' path does not exist: "+tmpdir);
          System.out.println("Created 'tmpdir' at "+tmpdir.getAbsolutePath());
          tmpdir.mkdirs();
        }

        if (config_binary != null) {
            senna_binary = config_binary;
        } else {
            throw new ResourceInitializationException(
                  new Exception("Missing Senna binary name: '" 
                                 + SENNA_BINARY + "'"));
        }

        if (config_home != null) {
            senna_home = config_home;
        } else {
            throw new ResourceInitializationException(
                              new Exception("Missing Senna home: '" 
                                              + SENNA_HOME + "'"));
        }
        
        if (sentence_annot_type == null) {
          throw new ResourceInitializationException(
                 new Exception("Missing the type of the sentence annotation: '" 
                               + SENTENCE_ANNOT_TYPE + "'"));
        }
        
        if (config_sent_max_len != null) {
          sentence_max_len = config_sent_max_len;
        }
                
        remove_tmp_files = false;

        getContext().getLogger().log(Level.INFO, "Tmpdir:                     " + tmpdir);
        getContext().getLogger().log(Level.INFO, "Binary:                     " + senna_binary);
        getContext().getLogger().log(Level.INFO, "Sentence annotation type:   " + sentence_annot_type);
        getContext().getLogger().log(Level.INFO, "Maximum sentence length:    " + sentence_max_len);
        getContext().getLogger().log(Level.INFO, "Remove_tmp_files:           " + remove_tmp_files);
    }

    /**
     * Annotates an existing Passage which resides in the AnnotationsDB.
     * 
     * @param passage a <code>Passage</code> object containing text content to
     * process with SENNA. No other annotations on the document are
     * required for operation of this annotator.
     */
    
    @Override
    public void process(JCas aJCas)
    throws AnalysisEngineProcessException {
        try {
            Vector<Annotation> sentence_tags;
            
            File f_output;
            File f_log = null;
            
            File f_input  = new File(tmpdir, SENNA_INPUT);
            if (!f_input.exists()) f_input.createNewFile();
            
            getContext().getLogger().log(Level.INFO, "Creating input file");
            sentence_tags = createInputFile(aJCas, f_input);
            getContext().getLogger().log(Level.INFO, "Input file created ("+sentence_tags.size()+" sentences): "+f_input);
            
            getContext().getLogger().log(Level.INFO, "Creating output file");

            f_output = new File(tmpdir, SENNA_OUTPUT);
            getContext().getLogger().log(Level.INFO, "Instantiating SENNA process");
            f_log = execSENNAProcess(f_input, f_output);
                
            getContext().getLogger().log(Level.INFO, "SENNA process completed");
    
            
            getContext().getLogger().log(Level.INFO, "Parsing SENNA output");

            parseSENNAOutput(aJCas, f_output, sentence_tags);

            if (remove_tmp_files) {
                getContext().getLogger().log(Level.INFO, "Removing temporary files");
                removeTmpFiles(new File[]{f_input, f_output, f_log});
                getContext().getLogger().log(Level.INFO, "Temporary files removed");
            }

        } catch (Exception e) {
            throw new AnalysisEngineProcessException(e);
        } 

    }
    
    private String getSentenceText(Annotation annot) {
      String fullText = annot.getCoveredText();
      // Don't trim here, it may change the offsets
      String sentence = pattern_whitespace.matcher(fullText).replaceAll(" ");
      /*
       * We need to restrict the length of a sentence, because
       *   1) Senna will cut divide long sentences into smaller one (which will 
       *      ruin the process of parsing Senna's output)
       *   2) It takes very long to parse long sentences.
       */
      if (sentence.length() > sentence_max_len) {
        // Remove the last potentially incomplete word
        sentence = sentence.substring(0, sentence_max_len);
        if (sentence.length() > 0 && sentence.charAt(sentence_max_len-1) != ' ') {
          int k = sentence.lastIndexOf(' ');
          if (k >= 0) {          
            sentence = sentence.substring(0, k);
          }
        }               
      }
      
      return sentence;
    }

    /**
     * Creates a temporary file containing the text to process with SENNA. 
     * To this end, the text should be already split into sentences.
     */
    private Vector<Annotation> createInputFile(JCas jcas, File input)
    throws Exception {
        PrintWriter pw = new PrintWriter
        (new BufferedWriter
                (new OutputStreamWriter
                        (new FileOutputStream(input), ENCODING)));

        Vector<Annotation> sentence_tags = new Vector<Annotation>();

        // Get an iterator for the Sentence annotations.
        Class<Annotation>   SentenceAnnotationClass = 
                            (Class<Annotation>) Class.forName(sentence_annot_type);
        Field               myField = SentenceAnnotationClass.getDeclaredField("typeIndexID");
        int                 typeIndexId = (Integer)myField.get(null);
        FSIndex<Annotation> SentenceAnnotIndex = 
                            jcas.getJFSIndexRepository().getAnnotationIndex(typeIndexId);
        FSIterator<Annotation> SentenceAnnotIter = SentenceAnnotIndex.iterator();


        getContext().getLogger().log(Level.INFO, "Sentence count: " + SentenceAnnotIndex.size());
        String sentence;

        int counter=1;
        while (SentenceAnnotIter.isValid()) {
            Annotation  an = SentenceAnnotIter.get();            

            getContext().getLogger().log(Level.INFO, "SENTENCE " + counter++ + " begin: " + an.getBegin() + " end: " + an.getEnd());


            sentence = getSentenceText(an);
            
            if (sentence.indexOf('\n') >= 0) {
              pw.close();
              throw new Exception("BUG: should not have the newline here");
            }           

            if (pattern_SENNA_bad_sentence.matcher(sentence).matches()) {
                getContext().getLogger().log(Level.INFO, "Skipping bad sentence: " + sentence);
                SentenceAnnotIter.moveToNext();
                continue;
            }
            pw.println(sentence);
            if (pw.checkError()) {
              pw.close();
              throw new IOException("Failed to create input file");
            }
                

            sentence_tags.add(an);

            SentenceAnnotIter.moveToNext();
        }

        pw.close();
        if (pw.checkError())
            throw new IOException("Failed to create input file");
        return sentence_tags;
    }


    /**
     * Instantiates an SENNA process using the supplied parameter file.
     * 
     * @param input a File referencing an input file initialized previously
     * with a call to createInputFile()
     * @param output an output file.
     */
    private File execSENNAProcess(File input, File output)
    throws Exception
    {
        String baseName = tmpdir.getCanonicalPath();
        String inputName = input.getCanonicalPath();
        String outputName = output.getCanonicalPath();
        /* 
         * Output data (column # are zero-indexed):
         * -offsettags      offsets						      columns 1-2
         * -pos             pos tags					      after offsets
         * -chk				      chunks						      after pos tags
         * -ner             named entities (NE)			after chunks
         * -srl            	semantic role labels		after NE
         * -psg				      syntax parse				    always the last column
         * 
         *  Without specifying -posvbs, we don't get SRL's for the verb 'to be'
         *  
         */
        String senna_opts = " -offsettags  -posvbs ";
        
        /*
         * If the list of annotators is empty, SENNA will produce all possible 
         * annotations. If at least one annotator is specified (e.g., using the flag
         * -srl), then SENNA produces only the specified annotations.
         */
        if (annot_types.isEmpty()) {
          throw new Exception("Bug: the list of annotators should not be empty!");
        }
        
        if (annot_types.contains(AnnotType.SRL)) {
            senna_opts += " -srl ";
        }
        if (annot_types.contains(AnnotType.POS)) {
          senna_opts += " -pos ";
        }
        if (annot_types.contains(AnnotType.CHK)) {
          senna_opts += " -chk ";
        }
        if (annot_types.contains(AnnotType.NER)) {
          senna_opts += " -ner ";
        }
        
        File logf = new File(baseName + ".log");
        //logf.deleteOnExit();

        // instantiate a SENNA process
        Process process;
        
        try {

            // in order to pipe stdout and stderr from our subprocess, we start
            // an instance of bash and then exec SENNA from within this shell.

            // SENNA generates its intermediate and final output files within
            // the current working directory, so we first must "cd" to the
            // directory we want output to be stored, making sure to test error
            // status of the "cd" operation.
            
            if (System.getProperty("os.name").startsWith("Windows")) {

                String cmd = "cd " + senna_home + " && " + senna_binary + " " + senna_opts + " < " +
                   inputName + " > " + outputName ;

                getContext().getLogger().log(Level.INFO, "Senna execution command: " + cmd);
                
                process = Runtime.getRuntime().exec("cmd /c " + cmd);
            } else {
                process = Runtime.getRuntime().exec("bash");
                PrintWriter pw = new PrintWriter(process.getOutputStream());
                
                String cmd = "cd " + senna_home + "; ./" + senna_binary + " " + senna_opts+ " < " +
                   inputName + " > " + outputName ;
                
                getContext().getLogger().log(Level.INFO, "Senna execution command: " + cmd);
                      
                pw.print(cmd);
                pw.close();
                    
                    
        				String line;
        				BufferedReader bri = new BufferedReader(new InputStreamReader(process.getInputStream()));
        				BufferedReader bre = new BufferedReader(new InputStreamReader(process.getErrorStream()));
        
        				while ((line = bri.readLine()) != null) {
        					// Reads the stream.
        					System.out.println(line);
        				}
        				bri.close();
        
        				while ((line = bre.readLine()) != null) {
        					// Reads the stream.
        					System.err.println(line);
        				}
        				bre.close();			
            }

            /*
             *  close stdin, stdout, and stderr
             *  Sub-processes will "known" that the communication is over.
             */
            
            process.getInputStream().close();
            process.getErrorStream().close();
        } catch (Exception e) {
            throw new Exception("Failed to execute annotator process", e);
        }


        int rv;
        try {
            rv = process.waitFor();
        } catch (InterruptedException e) {
            process.destroy();
            throw new Exception("process timed out");
        }

        if (rv != 0)
            throw new Exception("process returned error code: "+rv);

        return logf;
    }



    
    /**
     * Parses input stream from SENNA output
     * 
     * @param f_output        the output of SENNA process
     * @param sentence_tags   sentence annotations
     * 
     */
    private void parseSENNAOutput(JCas jcas, File f_output, Vector<Annotation> sentence_tags)
    throws /*IllegalInputFormatException,*/ Exception
    {
        /* 
         // this tries to pluck out terms that include the Unicode soft hyphen char.
         // TODO: where was it supposed to be used? when Leo got this code,
         //       the pattern whitespace wasn't used already.
        Pattern whitespace = Pattern.compile("[\\w'/\\-]+[\\xC2\\xAD\\n]?");
        */
        
        // Leo read the below comment, but it didn't make sense to him.
        // Where did previous authors use hyphens?
        /* 
         * CNS hack; these authors rely heavily---albeit superfluously---on the three hyphen
         * token.  We can preserve grammatical accuracy and make parsing more accurate
         * by replacing these with (comma,space,space) and not throw off any offsets.
         * Doing it for just 2 hyphens should cover both cases
         */

        int sentIndex  = 0;
        String line;

        BufferedReader br_output = null;
        try {
            br_output = new BufferedReader(new InputStreamReader(
                    new FileInputStream(f_output), ENCODING));
    
            while (!((line = br_output.readLine()) == null)) {
                ArrayList<ArrayList<String>>    oneSentParts = new ArrayList<ArrayList<String>>();
                
                int nFields = -1;
                                
                Annotation sentAnnot = sentence_tags.get(sentIndex);
                sentIndex++;                
                
                do {
                    ArrayList<String> parts = new ArrayList<String>();
                    for (String s: line.split("\\s+")) parts.add(s);
                    /*
                     * Sometimes a line begins with space, sometimes
                     * with a string. So, the split() method may produce an
                     * extra empty element. Let's delete it.
                     */
                    if (!parts.isEmpty() && parts.get(0).equals("")) {
                      parts.remove(0);
                    }
                    
                    if (nFields == -1) {
                      nFields = parts.size();
                    } else if (nFields != parts.size()) {
                      String sentText = getSentenceText(sentAnnot);
                      
                      System.err.println("Parse for the following sentence has " + 
                                        " different number of fields depending on the row:\n" + sentText);    
                    }
                    oneSentParts.add(parts);
                // There is an empty line after every parsed sentence
                } while (!(line = br_output.readLine()).equals(""));                                              
    
                int fieldOffset = 3;
                
                if (annot_types.contains(AnnotType.POS)) {
                    ParsePOS(jcas, sentAnnot.getBegin(), sentIndex, nFields, fieldOffset, oneSentParts);
                    fieldOffset++;
                }
                if (annot_types.contains(AnnotType.CHK)) {
                    ParseCHUNK(jcas, sentAnnot.getBegin(), sentIndex, nFields, fieldOffset, oneSentParts);
                    fieldOffset++;
                }
                if (annot_types.contains(AnnotType.NER)) {
                    ParseNER(jcas, sentAnnot.getBegin(), sentIndex, nFields, fieldOffset, oneSentParts);
                    fieldOffset++;
                }
                if (annot_types.contains(AnnotType.SRL)) {
                    ParseSRL(jcas, sentAnnot.getBegin(), sentIndex, nFields, fieldOffset, oneSentParts);
                    fieldOffset++;
                }
            }
        } finally {
            if (br_output != null) br_output.close();
        }
    }
        
    private void ParsePOS(JCas jcas, int sentBegin, int sentIndex, 
                          int nFields, int fieldOffset, ArrayList<ArrayList<String>> oneSentParts) throws Exception {
       if (nFields > fieldOffset) {
          int tokenCount = oneSentParts.size();

          for (int j = 0; j < tokenCount; j++) {                            
              ArrayList<String> splits = oneSentParts.get(j);

              String label = splits.get(fieldOffset);

              // Parse begin, end offsets
              int spanBegin = Integer.parseInt(splits.get(1));
              int spanEnd   = Integer.parseInt(splits.get(2));

              
              POS annotation = new POS(jcas);
              annotation.setBegin(sentBegin + spanBegin);
              annotation.setEnd(sentBegin + spanEnd);
              
              annotation.setTag(label);
              annotation.addToIndexes();
          }
       }
    }
        
    private void ParseCHUNK(JCas jcas, int sentBegin, int sentIndex, 
                          int nFields, int fieldOffset, ArrayList<ArrayList<String>> oneSentParts) throws Exception {
        String label;

        int spanBegin       = 0, 
            spanEnd         = 0;
      

        if (nFields > fieldOffset) {
          ArrayList<Chunk> annotList = new ArrayList<Chunk>();
          int tokenCount = oneSentParts.size();

          spanEnd   = 0;
          spanBegin = 0;
              
          Chunk annotation = null;
              
          for (int j = 0; j < tokenCount; j++) {                            
            ArrayList<String> splits = oneSentParts.get(j);

            label = splits.get(fieldOffset);

            // Parse begin, end offsets
            spanBegin = Integer.parseInt(splits.get(1));
            spanEnd   = Integer.parseInt(splits.get(2));

            /*
             * Four cases are possible:
             * S-<type>    annotation spans one token
             * B-<type>    annotation spans more than one token, and the current token is the beginning
             * I-<type>    annotation spans more than one token, and the current token is an intermediate one
             * B-<type>    annotation spans more than one token, and the current token is the ending
            */
            if (label.startsWith("S-")) { // emit standalone annot.
              annotation = new Chunk(jcas);
              annotation.setBegin(sentBegin + spanBegin);
              annotation.setEnd(sentBegin + spanEnd);
              String featureName = label.substring(2, label.length());
              annotation.setChunkType(featureName);
              annotList.add(annotation);
            }
            if (label.startsWith("B-")) { // annotation starts
              annotation = new Chunk(jcas);
              annotation.setBegin(sentBegin + spanBegin);
              String featureName = label.substring(2, label.length());
              annotation.setChunkType(featureName);
              // to be finished when we encounter the end of this annotation
            }
            if (label.startsWith("E-")) { // annotation ends
              /*
               *  SENNA may have some buggy output,
               *  where a start of an annotation is not marked.
               *  In such as case, we won't set annoation to a non-NULL
               *  
               */                    
              if (annotation != null) {
                annotation.setEnd(sentBegin + spanEnd);
                annotList.add(annotation);
              }
            }
            /*
             * We ignore the case "I-" here, because we care only 
             * about start and the end positions.
             */
          }
                  
          for (Annotation a: annotList) a.addToIndexes();
       }
    }
        
    private void ParseNER(JCas jcas, int sentBegin, int sentIndex, 
                          int nFields, int fieldOffset, ArrayList<ArrayList<String>> oneSentParts) throws Exception {
        String label;

        int spanBegin       = 0, 
            spanEnd         = 0;
      

        if (nFields > fieldOffset) {
          ArrayList<NamedEntity> annotList = new ArrayList<NamedEntity>();
          int tokenCount = oneSentParts.size();

          spanEnd   = 0;
          spanBegin = 0;
              
          NamedEntity annotation = null;
              
          for (int j = 0; j < tokenCount; j++) {                            
            ArrayList<String> splits = oneSentParts.get(j);

            label = splits.get(fieldOffset);

            // Parse begin, end offsets
            spanBegin = Integer.parseInt(splits.get(1));
            spanEnd   = Integer.parseInt(splits.get(2));

            /*
             * Four cases are possible:
             * S-<type>    annotation spans one token
             * B-<type>    annotation spans more than one token, and the current token is the beginning
             * I-<type>    annotation spans more than one token, and the current token is an intermediate one
             * B-<type>    annotation spans more than one token, and the current token is the ending
            */
            if (label.startsWith("S-")) { // emit standalone annot.
              annotation = new NamedEntity(jcas);
              annotation.setBegin(sentBegin + spanBegin);
              annotation.setEnd(sentBegin + spanEnd);
              String featureName = label.substring(2, label.length());
              annotation.setEntityType(featureName);
              annotList.add(annotation);
            }
            if (label.startsWith("B-")) { // annotation starts
              annotation = new NamedEntity(jcas);
              annotation.setBegin(sentBegin + spanBegin);
              String featureName = label.substring(2, label.length());
              annotation.setEntityType(featureName);
              // to be finished when we encounter the end of this annotation
            }
            if (label.startsWith("E-")) { // annotation ends
              /*
               *  SENNA may have some buggy output,
               *  where a start of an annotation is not marked.
               *  In such as case, we won't set annoation to a non-NULL
               *  
               */                    
              if (annotation != null) {              
                annotation.setEnd(sentBegin + spanEnd);
                annotList.add(annotation);
              }
            }
            /*
             * We ignore the case "I-" here, because we care only 
             * about start and the end positions.
             */
          }
                  
          for (Annotation a: annotList) a.addToIndexes();
       }
    }
        
    private void ParseSRL(JCas jcas, int sentBegin, int sentIndex, 
                          int nFields, int fieldOffset, ArrayList<ArrayList<String>> oneSentParts) throws Exception {
      String label;

      int spanBegin       = 0, 
          spanEnd         = 0;
      
      // This formula is only correct, if the syntax parse is not produced
      int parseQty = nFields - fieldOffset - 1;

      if (parseQty > 0) {
      
          int tokenCount = oneSentParts.size();

          for (int i = 0; i < parseQty; i++) {
              ArrayList<SemanticRole> annotList = new ArrayList<SemanticRole>();
              spanEnd   = 0;
              spanBegin = 0;
              
              SemanticRole annotation = null;
              SemanticRole parent = null;
              Vector<SemanticRole> children = new Vector<SemanticRole>();
              
              for (int j = 0; j < tokenCount; j++) {                            
                  ArrayList<String> splits = oneSentParts.get(j);

                  label = splits.get(i + fieldOffset + 1 /* to skip a text item */);

                  // Parse begin, end offsets
                  spanBegin = Integer.parseInt(splits.get(1));
                  spanEnd   = Integer.parseInt(splits.get(2));

                  // Tricky state machine to start/end various annotation types
                  if (!label.equals("O")) {
                      if (label.startsWith("S-")) { // emit standalone annot.
                          annotation = new SemanticRole(jcas);
                          annotation.setBegin(sentBegin + spanBegin);
                          annotation.setEnd(sentBegin + spanEnd);
                          String featureName = label.substring(2, label.length());
                          annotation.setLabel(featureName);
                          annotList.add(annotation);
                          annotation.setChildren(null);

                          if (label.equals("S-V") || label.equals("S-AM-MOD")) {
                              parent = annotation;
                          }
                          else
                              children.add(annotation);

                      }
                      if (label.startsWith("B-")) { // begin annotation
                          annotation = new SemanticRole(jcas);
                          annotation.setChildren(null);
                          annotation.setBegin(sentBegin + spanBegin);
                          annotation.setLabel(label.substring(2, label.length()));
                          // to be finished when we encounter 'end annotation'
                      }
                  }
                  if (label.startsWith("E-")) { // 'end annotation'
                    /*
                     *  SENNA may have some buggy output,
                     *  where a start of an annotation is not marked.
                     *  In such as case, we won't set annoation to a non-NULL
                     *  
                     */                    
                    if (annotation != null) {
                      annotation.setEnd(sentBegin + spanEnd);
                      annotList.add(annotation);

                      if (label.equals("E-V")) {
                          parent = annotation;
                      } else {
                          children.add(annotation);
                      }
                    }
                  }
              }
              // link children to parents
              java.util.Iterator<SemanticRole> it = children.iterator();
              while (it.hasNext()) {
                  annotation = it.next();
                  annotation.setParent(parent);

                  // no children for SemanticRole
                  if (parent != null)
                      parent.setChildren(updateArray(jcas, parent.getChildren(), annotation));
              }

              java.util.Iterator<SemanticRole> annotListIt = annotList.iterator();
                  
              while (annotListIt.hasNext()) {
                  SemanticRole annot = annotListIt.next();
                  annot.addToIndexes();
              }
          }
       }
    }

    /**
     * Add a single element to (the end of) an existing FSArray
     * @param jcas
     * @param array
     * @param ann
     * @return
     */
    public static FSArray updateArray(JCas jcas, FSArray array, FeatureStructure ann) {
      FSArray arr;
      if (array == null) {
        arr = new FSArray(jcas,1);
      } else {
        arr = new FSArray(jcas,array.size()+1);
        for (int i=0; i<array.size(); i++) arr.set(i, array.get(i));
      }
      arr.set(arr.size()-1, ann);
      return arr;
    }
      

    /**
     * deletes a list of File objects, registering removal failure as a warning
     * to log.
     */
    private void removeTmpFiles(File[] files) {
        for (int i=0; i < files.length; i++) {
            if (!files[i].delete()) {
                System.out.println("Failed to delete tmp file \""+files[i]+"\"");
            }
        }
    }

}
