Just run the script ``demo.sh``. It will execute three pipes that will:
1. create and cat a sufficiently long file
2. feed this output into ``head -10000``
3. count the number of lines using ``wc -l``

After each execution, we print the pipe status using the bash array variable PIPESTATUS. It can be seen that in all the scenarios, it indicates an error in the first pipe component! Also, if we do not ignore the SIGPIPE signal inside a Python script, it terminates with an exception. 

**Bottom line**: it seems that in unix/bash there is no reliable way to check if certain pipelines terminate successfully.
