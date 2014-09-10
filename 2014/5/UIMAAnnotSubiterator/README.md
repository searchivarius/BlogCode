Benchmarking UIMA subiterator function. See blog entries: 

http://searchivarius.org/blog/how-fast-uima-subiterator-function  
http://searchivarius.org/blog/selectcovered-substantially-better-version-uima-subiterator  

You need Java, Maven, html2text (Unix utility), and a Senna Parser version 3.0 (http://ml.nec-labs.com/senna/).
My code uses slightly modified version of SENNA_main.c https://github.com/searchivarius/BlogCode/blob/master/2014/3/15  


After compiling  the SENNA parser, you need to configure the senna annotator.
Open the file code/src/main/resources/parsers/senna.yaml and modify the following variables:

* senna_home
* tmp_dir

senna_home is a location of the compiled parser. tmp_dir is just any temporary directory.

Finally, you can go to the *code* folder and start the script: run/run_test.sh

The main benchmarking module:

https://github.com/searchivarius/BlogCode/blob/master/2014/5/UIMAAnnotSubiterator/code/src/main/java/edu/cmu/lti/oaqa/benchmark/Test.java
