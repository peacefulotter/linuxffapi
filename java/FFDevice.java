import java.util.Arrays;
import java.util.List;
import java.util.StringJoiner;

public class FFDevice {
    private static final int CAPABILITY_OFFSET = 16;
    private static final List<String> EFFECT_NAMES = Arrays.asList(
        "FF_RUMBLE", "FF_PERIODIC", "FF_CONSTANT", "FF_SPRING", "FF_FRICTION",
        "FF_DAMPER", "FF_INERTIA", "FF_RAMP", "FF_SQUARE", "FF_TRIANGLE",
        "FF_SINE", "FF_SAW_UP", "FF_SAW_DOWN", "FF_CUSTOM", "/", "/",
        "FF_GAIN", "FF_AUTOCENTER"
    );

    private final int fd;

    private FFDevice(int fd) {
        this.fd = fd;
    }

    private static boolean getBit(long n, int offset) {
        return ((n >> offset) & 1) == 1;
    }

    public static FFDevice get() throws Exception {
        int fd = NativeDevice.nativeOpenDevice();
        if (fd == -1) 
            throw new Exception("Failed to open FF device, is it connected?");
        return new FFDevice(fd);
    }

    public int close() {
        return NativeDevice.nativeCloseDevice(fd);
    }

    public String getName() {
        return NativeDevice.nativeGetDeviceName(fd);
    }

    public void printCapabilities() {
        long capabilities = NativeDevice.nativeGetDeviceCapabilities(fd);
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
        NativeDevice.nativeSetAutocenter(fd, value);
    }

    /**
        * @return -1 if failed, otherwise succeeded
        */
    public int playEffect(FFEffect effect) {
        short id = NativeDevice.nativePlayEffect(fd, effect);
        System.out.println("id: " + id);
        effect.setId(id);
        return id;
    }

    public void removeEffect(FFEffect effect) {
        NativeDevice.nativeRemoveEffect(fd, effect.getId());
    }
}
