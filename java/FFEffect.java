public class FFEffect {
    private short id, level, length;
    private boolean dir, changed;

    private static short clamp(short x, short min) {
        return (short) Math.max((int) min, Math.min((int) x, (int) Short.MAX_VALUE));
    }

    /**
     * @param level: effect strength (between -32768 and 32767)
     * @param length: time the effect lasts (in ms)
     * @param dir: true: left, false: right
     */
    public FFEffect(short level, short length, boolean dir) {
        setLevel(level);
        setLength(length);
        setDir(dir);
        this.id = -1;
        this.changed = true;
    }

    /**
     * Catches properties changes because the effect requires an entire re-upload in that case 
    */
    private <T> void setterMiddleware(T before, T after) {
        if (!before.equals(after))
            changed = true;
    }

    public short getLevel() { 
        return level; 
    }

    public void setLevel(short level) {
        short newLevel = clamp(level, Short.MIN_VALUE);
        setterMiddleware(this.level, newLevel);
        this.level = newLevel;
    }

    public short getLength() { 
        return length; 
    }

    public void setLength(short length) {
        short newLength = clamp(length, (short) 0);
        setterMiddleware(this.length, newLength);
        this.length = newLength;
    }

    public boolean getDir() { 
        return dir; 
    }

    public void setDir(boolean dir) {
        setterMiddleware(this.dir, dir);
        this.dir = dir;
    }

    int getId() { 
        return id; 
    }

    void setId(short id) {
        this.id = id;
        // setId called in playEffect in which case the effect has been uploaded to the latest version
        this.changed = false;
    }
}
