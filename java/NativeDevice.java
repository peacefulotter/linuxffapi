public abstract class NativeDevice {
    static native int nativeOpenDevice();
    static native int nativeCloseDevice(int fd);
    static native long nativeGetDeviceCapabilities(int fd);
    static native String nativeGetDeviceName(int fd);
    static native int nativeSetAutocenter(int fd, double amount);
    static native short nativePlayEffect(int fd, FFEffect effect);
    static native int nativeRemoveEffect(int fd, int id);
}
