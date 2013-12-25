#!/bin/bash
qty="$1"
export MAVEN_OPTS="-Xmx16000m"
./memusage.sh mvn exec:java -Dexec.args="$qty" -Dexec.mainClass=Main

