#include <dirent.h>
#include <string.h>
#include "main.h"

char* features_name[] = {
    "FF_RUMBLE", "FF_PERIODIC", "FF_CONSTANT", "FF_SPRING", "FF_FRICTION",
    "FF_DAMPER", "FF_INERTIA", "FF_RAMP", "FF_SQUARE", "FF_TRIANGLE",
    "FF_SINE", "FF_SAW_UP", "FF_SAW_DOWN", "FF_CUSTOM", "/", "/",
    "FF_GAIN", "FF_AUTOCENTER"
};

int upload_effect(int fd, struct ff_effect* effect) 
{
    return ioctl(fd, EVIOCSFF, effect);
}

size_t play_effect(int fd, unsigned short code, signed int value) 
{
    // TODO: remove time?
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;

    struct input_event ie;
    ie.time = time;
    ie.type = EV_FF;
    ie.code = code;
    ie.value = value;
    return write(fd, &ie, sizeof(ie));
}

int remove_effect(int fd, struct ff_effect* effect) 
{
    play_effect(fd, effect->id, 0);
    return ioctl(fd, EVIOCRMFF, effect->id);
}

size_t autocenter(int fd, double amount) 
{
    int value = (int) (0xFFFFUL * CLAMP(amount, 0, 1));
    return play_effect(fd, FF_AUTOCENTER, value);
}

void get_capabilities(int fd) 
{
    unsigned long features[FEATURES_LEN];
    
    if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)), features) == -1)
        return;

    for (size_t i = FF_RUMBLE; i <= FF_AUTOCENTER; i++)
    {
        unsigned long bit = (features[1] >> (i - sizeof(long) * 8)) & 0x01;
        printf("%s = %lu\n", features_name[i - FF_RUMBLE], bit);
    }
}

struct ff_effect* create_effect(signed short level, unsigned short length) 
{
    struct ff_effect* effect = (struct ff_effect*) calloc(1, sizeof(struct ff_effect));
    if (effect == NULL)
        return NULL;
    
    struct ff_constant_effect constant;
    constant.level = level;

    // struct ff_envelope envelope;
    // envelope.attack_length = 0;
    // envelope.attack_level = 0;
    // envelope.fade_length = 0;
    // envelope.fade_level = 0;
    // constant.envelope = envelope;

    struct ff_replay replay;
    replay.delay = 0;
    replay.length = length;

    struct ff_trigger trigger;
    trigger.button = 0;
    trigger.interval = 0;
    
    effect->direction = FF_DIRECTION_LEFT;
    effect->id = -1;
    effect->replay = replay;
    effect->trigger = trigger;
    effect->type = FF_CONSTANT;
    effect->u.constant = constant;

    return effect;
}

static int is_event_device(const struct dirent *dir) {
	return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
}

int versionsort(const struct dirent ** a, const struct dirent ** b) {
    return 1;
}


/**
 * @brief Scans devices on the computer and returns the number XX corresponding to the eventXX device that supports force feedback
 * 
 * Inspired from 
 * scan_devices(): https://github.com/freedesktop-unofficial-mirror/evtest/blob/master/evtest.c
 * https://github.com/flosse/linuxconsole/blob/master/utils/fftest.c#L60
 */
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
        if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)*sizeof(unsigned char)), features) == -1)
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

int open_ff_device() 
{
    int ff_dev_num = get_ff_device_num();
    printf("ff_dev_num %d\n", ff_dev_num);
    char fname[EVENT_PATH_SIZE];
    snprintf(fname, sizeof(fname), "%s/%s%d", DEV_INPUT_FOLDER, EVENT_DEV_NAME, ff_dev_num);
    return open(fname, O_RDWR);
}

int close_ff_device(int fd) 
{
    return close(fd);
}

void get_device_name(int fd, char* name) 
{    
    ioctl(fd, EVIOCGNAME(sizeof(name) * sizeof(char)), name);
}

int main() 
{
    int fd = open_ff_device();
    printf("fd %d\n", fd);
    if ( fd == -1 ) return 1;

    char name[256] = "???";
    get_device_name(fd, name);
    printf("Device name: %s\n", name);

    get_capabilities(fd);

    printf("autocenter 100 = %ld \n", autocenter(fd, 1));
    sleep(1);
    printf("autocenter 0 = %ld \n", autocenter(fd, 0));
    
    struct ff_effect* effect = create_effect(15000, 100); // 32767
    if (effect == NULL)
        return 1;

    printf("uploaded = %d, effect.id = %d \n", upload_effect(fd, effect), effect->id);
    printf("played = %ld\n", play_effect(fd, effect->id, 1));

    sleep(1);

    effect->direction = FF_DIRECTION_RIGHT;
    printf("uploaded = %d, effect.id = %d \n", upload_effect(fd, effect), effect->id);
    printf("played = %ld\n", play_effect(fd, effect->id, 1));

    sleep(1);

    printf("removed = %d\n", remove_effect(fd, effect));
    close_ff_device(fd);
    free(effect);

    return 0;
}