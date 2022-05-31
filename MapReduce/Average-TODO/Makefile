INPUT = /user/hadoop_admin/input/average-large.txt
OUTPUT = avg-output
SAMPLE_IN = ./sample.in
SAMPLE_OUT = sample_out

JAR = Average.jar
CP = $(shell hadoop classpath)

.PHONY: all test clean 

all: ${JAR}

Average.jar: ./src/*.java
	mkdir -p ./bin
#	hadoop com.sun.tools.javac.Main -d ./bin ./src/*.java
	javac -cp ${CP} -d ./bin ./src/*.java
	jar -cvf $@ -C ./bin .

test:
	-hdfs dfs -rm -r -f $(OUTPUT)
	-rm -rf local-$(OUTPUT)
	hadoop jar Average.jar Average $(INPUT) $(OUTPUT)
	hdfs dfs -copyToLocal $(OUTPUT) local-$(OUTPUT)

sample:
	-rm -rf $(SAMPLE_OUT)
	-hdfs dfs -rm -r -f $(SAMPLE_IN)
	-hdfs dfs -rm -r -f $(SAMPLE_OUT)
	-hdfs dfs -copyFromLocal $(SAMPLE_IN) $(SAMPLE_IN)
	hadoop jar Average.jar Average $(SAMPLE_IN) $(SAMPLE_OUT)
	hdfs dfs -copyToLocal $(SAMPLE_OUT) $(SAMPLE_OUT)

clean:
	-rm -rf ./bin
	-rm $(JAR)
	-hdfs dfs -rm -r -f $(OUTPUT)
	-rm -rf $(SAMPLE_OUT)
	-rm -rf local-$(OUTPUT)

