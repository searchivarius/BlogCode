#!/bin/bash
javac test.java
java -cp . test cas1_0.xml > out1_0.txt
java -cp . test cas1_1.xml > out1_1.txt
echo "Compare the differences:"
diff -ub out1_0.txt out1_1.txt
