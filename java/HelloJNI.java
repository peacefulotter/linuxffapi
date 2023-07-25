import java.util.Arrays;
import java.util.List;
import java.lang.Exception;

public class HelloJNI {

   public static final List<String> EFFECT_NAMES = Arrays.asList(
      "FF_RUMBLE", "FF_PERIODIC", "FF_CONSTANT", "FF_SPRING", "FF_FRICTION",
      "FF_DAMPER", "FF_INERTIA", "FF_RAMP", "FF_SQUARE", "FF_TRIANGLE",
      "FF_SINE", "FF_SAW_UP", "FF_SAW_DOWN", "FF_CUSTOM", "/", "/",
      "FF_GAIN", "FF_AUTOCENTER"
   );

   static {
      System.loadLibrary("hello");
   }
  
   private static native int openDevice();
   private static native int getCapabilities(int fd);
   // private static native int setAutocenter(int fd, double amount);
   // private static native FFEffect createEffect(short level, short length);
   // private static native int uploadEffect(int fd, FFEffect effect);
   // private static native int playEffect(int fd, short code, int value);
   // private static native int removeEffect(int fd, FFEffect effect);
  
   private static boolean getBit(int n, int offset) {
      return ((n >> offset) & 1) == 1;
   }

   public static void main(String[] args) throws Exception {
      FFEffect effect1 = new FFEffect(1, 2);
      FFEffect effect2 = new FFEffect(3, 4);
      int fd = openDevice();

      if (fd == -1) {
         throw new Exception("fd == -1");
      }

      System.out.println(fd);
      int capabilities = getCapabilities(fd);
      System.out.println(Integer.toBinaryString(capabilities));

      int len = EFFECT_NAMES.size();
      for (int i = 0; i < len; i++) {
         if ( getBit(capabilities, len - i - 1)) {
            System.out.println(len - i - 1 + " " + EFFECT_NAMES.get(i));
         }
      }

      // uploadEffect(fd, effect1);      
      // uploadEffect(fd, effect2);
   }

   static class FFEffect {
      public final int level;
      public final int length;

      public FFEffect(int level, int length) {
         this.level = level;
         this.length = length;
      }
   }
}