import java.lang.Exception;

public class HelloJNI {

   static {
      System.loadLibrary("nativedevice");
   }

   public static void main(String[] args) throws Exception {

      FFDevice jni = FFDevice.get();
      System.out.println(jni.getName());
      jni.printCapabilities();

      jni.setAutocenter(1);
      Thread.sleep(2000);
      jni.setAutocenter(0);
      System.out.println("Autocenter done");
      Thread.sleep(1000);

      FFEffect effect = new FFEffect((short) 15000,  (short) 150, Direction.LEFT);
      System.out.println("Playing effect");
      System.out.println("BEFORE SETTING DIR " + effect.getDir().ordinal());
      jni.playEffect(effect);
      Thread.sleep(2000);

      System.out.println("Playing effect");
      System.out.println("BEFORE SETTING DIR " + effect.getDir().ordinal());
      effect.setDir(Direction.RIGHT); // .setLevel((short) -15000);
      System.out.println("AFTER SETTING DIR " + effect.getDir().ordinal());
      jni.playEffect(effect);
      Thread.sleep(1000);

      jni.removeEffect(effect);
      jni.close();
   }
}