#/bin/bash
mvn compile exec:java -Dexec.mainClass=edu.cmu.lti.oaqa.ecd.driver.ECDDriver -Dexec.args=test
