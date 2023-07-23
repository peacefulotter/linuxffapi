# Assuming: export JAVA_HOME=/usr/lib/jvm/jdk-18/
javac -h . HelloJNI.java
gcc -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o libhello.so HelloJNI.c