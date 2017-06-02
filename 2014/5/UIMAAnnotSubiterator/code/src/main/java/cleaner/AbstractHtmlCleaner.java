/*
 * The Abstract class for an HTML-cleaner. The class implements only the
 * factory function createInstance(). It expects that an child class
 * does have a public parameterless constructor.
 *
 * Author: Leonid Boytsov
 * Copyright (c) 2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

package cleaner;

import java.lang.reflect.Constructor;

import org.apache.uima.UimaContext;
import org.apache.uima.resource.ResourceInitializationException;

public abstract class AbstractHtmlCleaner {
  public static AbstractHtmlCleaner createInstance(String className, UimaContext aContext) {    
    try {
      Class<? extends AbstractHtmlCleaner> myClass = (Class<? extends AbstractHtmlCleaner>) 
                                                      Class.forName(className);
      
      Class[]  types = {UimaContext.class};
      Object[] params = {aContext};
      Constructor<? extends AbstractHtmlCleaner> constr = myClass.getConstructor(types);
      AbstractHtmlCleaner inst = constr.newInstance(params);
      
      return inst;
    } catch (ClassNotFoundException e) {
      System.err.println("Don't have the class: " + className);
      System.exit(1);
    } catch (NoSuchMethodException e) {
      System.err.println("The class: " + className + 
                         " doesn't have the public constructor with parameter UimaContext");
    } catch (SecurityException e) {
      System.err.println("The class: " + className + 
          " doesn't have the public constructor with parameter UimaContext");
    
    } catch (InstantiationException e) {
      System.err.println("The class: " + className + 
          " is not an instance of " + AbstractHtmlCleaner.class.getName());    
    } catch (Exception e) {
      System.err.println("Error creating the instance of the class : " + className + e.getMessage());     
    }
    
    System.exit(1);    
    return null;
  }
  
  public void init(UimaContext aContext) throws ResourceInitializationException {}
  
  public abstract String CleanText(String html, String encoding);

}
