import java.util.Arrays;
import java.util.List;
import java.util.StringJoiner;
import java.lang.Exception;

public class HelloJNI {

   private static final int CAPABILITY_OFFSET = 16;
   private static final List<String> EFFECT_NAMES = Arrays.asList(
      "FF_RUMBLE", "FF_PERIODIC", "FF_CONSTANT", "FF_SPRING", "FF_FRICTION",
      "FF_DAMPER", "FF_INERTIA", "FF_RAMP", "FF_SQUARE", "FF_TRIANGLE",
      "FF_SINE", "FF_SAW_UP", "FF_SAW_DOWN", "FF_CUSTOM", "/", "/",
      "FF_GAIN", "FF_AUTOCENTER"
   );

   static {
      System.loadLibrary("hello");
   }

   private final int fd;

   private HelloJNI(int fd) {
      this.fd = fd;
   }
  
   private static native int openDevice();
   private static native int closeDevice(int fd);
   private static native long getDeviceCapabilities(int fd);
   private static native String getDeviceName(int fd);
   private static native int setAutocenter(int fd, double amount);
   private static native int playEffect(int fd, FFEffect effect);
   private static native int removeEffect(int fd, int id);

   private static boolean getBit(long n, int offset) {
      return ((n >> offset) & 1) == 1;
   }

   public static HelloJNI get() throws Exception {
      int fd = openDevice();
      if (fd == -1) 
         throw new Exception("Failed to open FF device, is it connected?");
      return new HelloJNI(fd);
   }

   public int close() {
      return closeDevice(fd);
   }

   public String getName() {
      return getDeviceName(fd);
   }

   public void printCapabilities() {
      long capabilities = getDeviceCapabilities(fd);
      int len = EFFECT_NAMES.size();
      StringJoiner sj = new StringJoiner(", ");
      for (int i = 0; i < len; i++) {
         int offset = i + CAPABILITY_OFFSET;
         if ( getBit(capabilities, offset)) {
            sj.add(EFFECT_NAMES.get(i));
         }
      }
      System.out.println(sj);
   }

   /**
    * @param value: between 0 and 1
    */
   public void setAutocenter(double value) {
      setAutocenter(fd, value);
   }

   /**
    * @return -1 if failed, otherwise succeeded
    */
   public int playEffect(FFEffect effect) {
      int id = playEffect(fd, effect);
      System.out.println("id: " + id);
      effect.setId(id);
      return id;
   }

   public void removeEffect(FFEffect effect) {
      removeEffect(fd, effect.getId());
   }
   

   public static void main(String[] args) throws Exception {

      HelloJNI jni = HelloJNI.get();
      System.out.println(jni.getName());
      jni.printCapabilities();

      jni.setAutocenter(1);
      Thread.sleep(2000);
      jni.setAutocenter(0);
      System.out.println("Autocenter done");
      Thread.sleep(1000);

      FFEffect effect1 = new FFEffect((short) 15000,  (short) 250);
      System.out.println("Playing effect");
      jni.playEffect(effect1);
      Thread.sleep(2000);
      
      System.out.println("Playing effect");
      effect1.setLevel((short) -15000);
      jni.playEffect(effect1);
      Thread.sleep(1000);

      jni.removeEffect(effect1);

      jni.close();

      // FFEffect effect1 = new FFEffect(1, 2);
      // FFEffect effect2 = new FFEffect(3, 4);
      // uploadEffect(fd, effect1);      
      // uploadEffect(fd, effect2);
   }

   static class FFEffect {
      private int id;
      private short level, length;

      private static short clamp(short x, short min) {
         return (short) Math.max((int) min, Math.min((int) x, (int) Short.MAX_VALUE));
      }

      /**
       * @param level: effect strength (between -32768 and 32767)
       * @param length: time the effect lasts (in ms)
       */
      public FFEffect(short level, short length) {
         setLevel(level);
         setLength(length);
      }

      short getLevel() { 
         return level; 
      }

      void setLevel(short level) {
         this.level = clamp(level, Short.MIN_VALUE);
      }

      short getLength() { 
         return length; 
      }

      void setLength(short length) {
         this.length = clamp(length, (short) 0);
      }

      int getId() { 
         return id; 
      }

      void setId(int id) {
         this.id = id;
      }
   }
}