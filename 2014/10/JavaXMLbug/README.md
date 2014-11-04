This is an example of apparently erroneous parsing of XML when XML version is set to 1.1.  Just run ``run_test.sh`` and compare input/output files. The only difference between two input files is the XML version. Yet, when it is set to 1.1, the value of the field text is extracted **incorrectly**!

This problem was observed in Java 7, e.g., in the following configuration:
``
java version "1.7.0_65"
Java(TM) SE Runtime Environment (build 1.7.0_65-b17)
Java HotSpot(TM) 64-Bit Server VM (build 24.65-b04, mixed mode)
``
