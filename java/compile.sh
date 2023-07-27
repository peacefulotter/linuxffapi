# Assuming: export JAVA_HOME=/PATH/TO/JDK
# For example on my setup: /usr/lib/jvm/jdk-18/
javac -h . HelloJNI.java -d ./out/
gcc -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o ./out/libnativedevice.so NativeDevice.c