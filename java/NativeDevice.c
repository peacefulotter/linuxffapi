#include "NativeDevice.h"   // Generated
#include "main.h"


// #include <linux/input.h>
// struct input_dev* device = input_allocate_device();
// device->name()

/**
    +---+---------+
    | Z | boolean |
    | B | byte    |
    | C | char    |
    | S | short   |
    | I | int     |
    | J | long    |
    | F | float   |
    | D | double  |
    +-------------+
 */
bool get_class_bool_field(JNIEnv* env, jobject obj, const char* var_name) 
{
    jclass cls = (*env)->GetObjectClass(env, obj);
    return (*env)->GetBooleanField(env, obj, (*env)->GetFieldID(env, cls, var_name, "Z"));
}

int get_class_int_field(JNIEnv* env, jobject obj, const char* var_name) 
{
    jclass cls = (*env)->GetObjectClass(env, obj);
    return (*env)->GetIntField(env, obj, (*env)->GetFieldID(env, cls, var_name, "I"));
}

short get_class_short_field(JNIEnv* env, jobject obj, const char* var_name) 
{
    jclass cls = (*env)->GetObjectClass(env, obj);
    return (*env)->GetShortField(env, obj, (*env)->GetFieldID(env, cls, var_name, "S"));
}

struct ff_effect* create_effect(JNIEnv* env, jobject obj) 
{
    short level = get_class_short_field(env, obj, "level");
	short length = get_class_short_field(env, obj, "length");
	short id = get_class_short_field(env, obj, "id");
	bool dir = get_class_bool_field(env, obj, "dir");
    printf("c got object params\n\tlvl: %d\n\tlength: %d\n\tdir: %d\n\tid: %d\n", level, length, dir, id);

    struct ff_effect* effect = (struct ff_effect*) calloc(1, sizeof(struct ff_effect));
    if (effect == NULL)
        return NULL;
    
    struct ff_constant_effect constant;
    constant.level = (signed short) level;

    struct ff_envelope envelope;
    envelope.attack_length = 0;
    envelope.attack_level = 0;
    envelope.fade_length = 0;
    envelope.fade_level = 0;
    constant.envelope = envelope;

    struct ff_replay replay;
    replay.delay = 0;
    replay.length = length;

    struct ff_trigger trigger;
    trigger.button = 0;
    trigger.interval = 0;

    effect->direction = (dir == 1) ? FF_DIRECTION_LEFT : FF_DIRECTION_RIGHT;
    effect->id = (signed short) id;
    effect->replay = replay;
    effect->trigger = trigger;
    effect->type = FF_CONSTANT;
    effect->u.constant = constant;

    return effect;
}

JNIEXPORT jlong JNICALL Java_NativeDevice_nativeGetDeviceCapabilities(JNIEnv* env, jclass class, jint fd) 
{
    unsigned long features[FEATURES_LEN];

    if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)), features) < 0)
        return 0;

    return features[1];
}


static int is_event_device(const struct dirent *dir) {
	return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
}

int versionsort(const struct dirent ** a, const struct dirent ** b) {
    return 1;
}

int get_ff_device_num()
{
	struct dirent **namelist;
	int ndev = scandir(DEV_INPUT_FOLDER, &namelist, is_event_device, versionsort);
	if (ndev <= 0)
		return -1;

	int devnum;
    int ff_device = -1;
    unsigned char features[1 + FF_MAX/8/sizeof(unsigned char)];

	for (int i = 0; i < ndev; i++)
	{
        // Get event number corresponding to the device at index i
        sscanf(namelist[i]->d_name, "event%d", &devnum);

		char fname[EVENT_PATH_SIZE];
        // get path event name in the format: /dev/input/eventXX
		snprintf(fname, sizeof(fname), "%s/%s%d", DEV_INPUT_FOLDER, EVENT_DEV_NAME, devnum);
        // Open the handler in read only mode
        int fd = open(fname, O_RDONLY);
		if (fd < 0)
			continue;

        // Reads FF features
        if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)*sizeof(unsigned char)), features) < 0)
            continue;

        // Check if opened handler supports force feedback by checking if FF_CONSTANT effects are supported
	    if (testBit(FF_CONSTANT, features)) {
            ff_device = devnum; // Found the FF device!
        }

		close(fd);
		free(namelist[i]);
	}
    
    return ff_device;
}


JNIEXPORT jint JNICALL Java_NativeDevice_nativeOpenDevice(JNIEnv* env, jclass class) 
{
    int ff_dev_num = get_ff_device_num();
    char fname[EVENT_PATH_SIZE];
    snprintf(fname, sizeof(fname), "%s/%s%d", DEV_INPUT_FOLDER, EVENT_DEV_NAME, ff_dev_num);
    return open(fname, O_RDWR);
}

JNIEXPORT jint JNICALL Java_NativeDevice_nativeCloseDevice(JNIEnv* env, jclass class, jint fd) 
{
    return close(fd);
}

JNIEXPORT jstring JNICALL Java_NativeDevice_nativeGetDeviceName(JNIEnv* env, jclass class, jint fd)
{
    char name[256];
    ioctl(fd, EVIOCGNAME(sizeof(name) * sizeof(char)), name);
    return (*env)->NewStringUTF(env, name); 
}

int play_effect(int fd, unsigned short code, signed int value) 
{
    struct input_event ie;
    ie.type = EV_FF;
    ie.code = code;
    ie.value = value;
    return write(fd, &ie, sizeof(ie));
}


JNIEXPORT jint JNICALL Java_NativeDevice_nativeSetAutocenter(JNIEnv* env, jclass class, jint fd, jdouble amount)
{
    int value = (int) (0xFFFFUL * CLAMP(amount, 0, 1));
    return play_effect(fd, FF_AUTOCENTER, value);
}

int upload_effect(int fd, struct ff_effect* effect) {
    return ioctl(fd, EVIOCSFF, effect);
}

JNIEXPORT jshort JNICALL Java_NativeDevice_nativePlayEffect(JNIEnv* env, jclass class, jint fd, jobject obj) 
{
    jclass cls_effect = (*env)->GetObjectClass(env, obj);
    jfieldID id_dir = (*env)->GetFieldID(env, cls_effect, "dir", "LDirection;");
    printf("c id_dir %p\n", id_dir);

    jobject dir_obj = (*env)->GetObjectField(env, cls_effect, id_dir);
    printf("c dir_obj %p\n", dir_obj);

    jclass cls_dir = (*env)->FindClass(env, "LDirection;");
    printf("c cls_dir %p\n", cls_dir);

    int dir = get_class_int_field(env, dir_obj, "value");
    printf("c dir %d\n", dir);

    return -1;

    bool changed = get_class_bool_field(env, obj, "changed");
    struct ff_effect* effect = create_effect(env, obj);

    printf("c changed: %d\n", changed);

    if (effect == NULL)
        return -1;

    if (changed && upload_effect(fd, effect) < 0)
        return -1;

    printf("c upload: %d\n", effect->id);

    if (play_effect(fd, effect->id, 1) < 0)
        return -1;

    short id = effect->id;
    free(effect);

    printf("c played: %d\n", id);
    return id;
}

JNIEXPORT jint JNICALL Java_NativeDevice_nativeRemoveEffect(JNIEnv* env, jclass class, jint fd, jint id) 
{
    return ioctl(fd, EVIOCRMFF, id);
}


