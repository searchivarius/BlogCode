/*
 * This file is created from FileSystemCollection reader.
 * Modified by the OAQA team and Leonid Boytsov
 *  
 * Originally licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package collection.fs;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.text.Normalizer;
import java.text.Normalizer.Form;

import org.apache.uima.resource.ResourceConfigurationException;
import org.apache.uima.resource.ResourceInitializationException;
import org.apache.uima.util.FileUtils;
import org.apache.uima.util.Progress;
import org.apache.uima.util.ProgressImpl;
import cleaner.AbstractHtmlCleaner;

import edu.cmu.lti.oaqa.framework.DataElement;
import edu.cmu.lti.oaqa.framework.collection.AbstractCollectionReader;

/**
 * A simple collection reader that reads documents from a directory in the
 * filesystem. It can be configured with the following parameters:
 * <ul>
 * <li><code>InputDirectory</code> - path to directory containing files</li>
 * <li><code>Encoding</code> (optional) - character encoding of the input files</li>
 * <li><code>HtmlCleanerClass</code> (optional) - an instance of the AbstractHtmlCleaner 
 *           to strip HTML tags </li>
 * </ul>
 * 
 * 
 */
public class FileSystemCollectionReader extends AbstractCollectionReader {
    /**
     * Name of configuration parameter that must be set to the path of a
     * directory containing input files.
     */
    public static final String PARAM_INPUTDIR = "InputDirectory";

    /**
     * Name of configuration parameter that contains the character encoding used
     * by the input files. If not specified, the default system encoding will be
     * used.
     */
    public static final String PARAM_ENCODING = "Encoding";

    /**
     * Name of optional configuration parameter that defines a class
     * that can strip HTML tags (the class should inherit from AbstractHtmlCleaner)
     */
    public static final String PARAM_HTML_CLEANER = "HtmlCleanerClass";

    /**
     * Name of optional configuration parameter that indicates including the
     * subdirectories (recursively) of the current input directory.
     */
    public static final String PARAM_SUBDIR = "BrowseSubdirectories";

    private ArrayList<File> mFiles;

    private String mEncoding;

    private Boolean mRecursive;

    private int mCurrentIndex;

    private AbstractHtmlCleaner mWorkHorse;

    // private String text;
    /**
     * @see org.apache.uima.collection.CollectionReader_ImplBase#initialize()
     */
    public void initialize() throws ResourceInitializationException {
        super.initialize();
        File directory = new File(
                ((String) getConfigParameterValue(PARAM_INPUTDIR)).trim());

        mEncoding = (String) getConfigParameterValue(PARAM_ENCODING);
        mRecursive = (Boolean) getConfigParameterValue(PARAM_SUBDIR);
        if (null == mRecursive) { // could be null if not set, it is optional
            mRecursive = Boolean.FALSE;
        }
        mCurrentIndex = 0;

        String className = (String) getConfigParameterValue(PARAM_HTML_CLEANER);
        
        if (className != null) {
          mWorkHorse = AbstractHtmlCleaner.createInstance(className, getUimaContext());
        }

        // if input directory does not exist or is not a directory, throw
        // exception
        if (!directory.exists() || !directory.isDirectory()) {
            throw new ResourceInitializationException(
                    ResourceConfigurationException.DIRECTORY_NOT_FOUND,
                    new Object[] { PARAM_INPUTDIR,
                            this.getMetaData().getName(), directory.getPath() });
        }

        // get list of files in the specified directory, and subdirectories if
        // the
        // parameter PARAM_SUBDIR is set to True
        mFiles = new ArrayList<File>();
        addFilesFromDir(directory);
    }

    /**
     * This method adds files in the directory passed in as a parameter to
     * mFiles. If mRecursive is true, it will include all files in all
     * subdirectories (recursively), as well.
     * 
     * @param dir
     */
    private void addFilesFromDir(File dir) {
        File[] files = dir.listFiles();
        for (int i = 0; i < files.length; i++) {
            if (!files[i].isDirectory()) {
                mFiles.add(files[i]);
            } else if (mRecursive) {
                addFilesFromDir(files[i]);
            }
        }
    }

    /**
     * @see org.apache.uima.collection.CollectionReader#hasNext()
     */
    public boolean hasNext() {
        return mCurrentIndex < mFiles.size();
    }
    
    /**
     * @see org.apache.uima.collection.base_cpm.BaseCollectionReader#close()
     */
    public void close() throws IOException {
    }

    /**
     * @see org.apache.uima.collection.base_cpm.BaseCollectionReader#getProgress()
     */
    public Progress[] getProgress() {
        return new Progress[] { new ProgressImpl(mCurrentIndex, mFiles.size(),
                Progress.ENTITIES) };
    }

    /**
     * Gets the total number of documents that will be returned by this
     * collection reader. This is not part of the general collection reader
     * interface.
     * 
     * @return the number of documents in the collection
     */
    public int getNumberOfDocuments() {
        return mFiles.size();
    }

    @Override
    protected DataElement getNextElement() throws Exception {
        // open input stream to file
        File file = (File) mFiles.get(mCurrentIndex++);
        String text = FileUtils.file2String(file, mEncoding);
        
        if (mWorkHorse != null) {
          text = mWorkHorse.CleanText(text, mEncoding);
        }
        
        // Remove diacritics as well
        text = Normalizer.normalize(text, 
                  Form.NFD).replaceAll("\\p{InCombiningDiacriticalMarks}+", "");


        return new DataElement(getDataset(), String.valueOf(mCurrentIndex - 1), 
                               text, file.getAbsolutePath());
    }

}
