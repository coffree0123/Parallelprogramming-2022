INPUT = /user/hadoop_admin/input/wordcount.txt
OUTPUT = wordcnt-output
MAN_IN = ./sample.in
MAN_OUT = sample_output

JAR = WordCount.jar
CP = $(shell hadoop classpath)

.PHONY: all test clean 

all: ${JAR}

WordCount.jar: ./src/*.java
	mkdir -p ./bin
#	hadoop com.sun.tools.javac.Main -d ./bin ./src/*.java
	javac -cp ${CP} -d ./bin ./src/*.java
	jar -cvf $@ -C ./bin .

test:
	-hdfs dfs -rm -r -f $(OUTPUT)
	-rm -rf local-$(OUTPUT)
	hadoop jar WordCount.jar WordCount $(INPUT) $(OUTPUT)
	hdfs dfs -copyToLocal $(OUTPUT) local-$(OUTPUT)

manual:
	-hdfs dfs -rm -r -f $(MAN_IN)
	-hdfs dfs -rm -r -f $(MAN_OUT)
	-rm -rf local-$(MAN_OUT)
	hdfs dfs -copyFromLocal $(MAN_IN) $(MAN_IN)
	hadoop jar WordCount.jar WordCount $(MAN_IN) $(MAN_OUT)
	hdfs dfs -copyToLocal $(MAN_OUT) local-$(MAN_OUT)

clean:
	-rm -rf ./bin
	-rm $(JAR)
	-hdfs dfs -rm -r -f $(OUTPUT)
	-rm -rf local-wordcnt-output

