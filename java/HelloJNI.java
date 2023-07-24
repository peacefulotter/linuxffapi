public class HelloJNI {  // Save as HelloJNI.java

   static {
      System.loadLibrary("hello");
   }
  
   private native int upload_effect(int fd, struct ff_effect* effect);
   private native void sayHello();
  
   // Test Driver
   public static void main(String[] args) {
      new HelloJNI().sayHello();  // Create an instance and invoke the native method
   }
}