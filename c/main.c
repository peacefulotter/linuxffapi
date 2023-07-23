#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BITS_TO_LONGS(x) \
        (((x) + 8 * sizeof (unsigned long) - 1) / (8 * sizeof (unsigned long)))
#define FEATURES_LEN BITS_TO_LONGS(FF_CNT)
unsigned long features[FEATURES_LEN];
char* features_name[] = {
    "FF_RUMBLE", "FF_PERIODIC", "FF_CONSTANT", "FF_SPRING", "FF_FRICTION",
    "FF_DAMPER", "FF_INERTIA", "FF_RAMP", "FF_SQUARE", "FF_TRIANGLE",
    "FF_SINE", "FF_SAW_UP", "FF_SAW_DOWN", "FF_CUSTOM", "/", "/",
    "FF_GAIN", "FF_AUTOCENTER"
};
#define FF_DIRECTION_LEFT 0x4000
#define FF_DIRECTION_RIGHT 0xC000

int autocenter(int fd, double amount) {

    double force = MIN((double) 1, MAX((double) 0, amount));

    struct input_event ie;
    ie.type = EV_FF;
    ie.code = FF_AUTOCENTER;
    ie.value = 0xFFFFUL * force;

    return write(fd, &ie, sizeof(ie));
}

int main()  {
    struct input_event play;
    struct input_event stop;
    struct ff_effect effect;

    int fd;
    fd = open("/dev/input/event6", O_RDWR);
    printf("%d\n", fd);

    size_t size = 16;
    int res = ioctl(fd, EVIOCGBIT(EV_FF, size), features);
    printf("res: %d\n", res);
    for (size_t i = FF_RUMBLE; i <= FF_AUTOCENTER; i++)
    {
        printf("%s = %lu\n", features_name[i - FF_RUMBLE], (features[1] >> (i - sizeof(long) * 8)) & 0x01);
    }
    
    struct ff_constant_effect constant;
    constant.level = ((signed short) 32767); // max = 32767
    // struct ff_envelope envelope;
    // envelope.attack_length = 0;
    // envelope.attack_level = 0;
    // envelope.fade_length = 0;
    // envelope.fade_level = 0;
    // constant.envelope = envelope;

    struct ff_replay replay;
    replay.delay = 0;
    replay.length = 100;

    struct ff_trigger trigger;
    trigger.button = 0;
    trigger.interval = 0;
    
    effect.direction = FF_DIRECTION_LEFT;
    effect.id = -1;
    effect.replay = replay;
    effect.trigger = trigger;
    effect.type = FF_CONSTANT;
    effect.u.constant = constant;

    int uploaded = ioctl(fd, EVIOCSFF, &effect);
    printf("uploaded = %d, effect.id = %hu \n", uploaded, effect.id);

    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;

    play.time = time;
    play.type = EV_FF;
    play.code = effect.id;
    play.value = 1;
    write(fd, (const void*) &play, sizeof(play));

    sleep(1);

    effect.direction = FF_DIRECTION_RIGHT;
    // effect.u.constant.level = ((signed short) 32767);
    uploaded = ioctl(fd, EVIOCSFF, &effect);
    printf("uploaded = %d, effect.id = %hu \n", uploaded, effect.id);
    write(fd, (const void*) &play, sizeof(play));

    sleep(1);

    stop.type = EV_FF;
    stop.code = effect.id;
    stop.value = 0;
    write(fd, (const void*) &stop, sizeof(stop));

    int removed = ioctl(fd, EVIOCRMFF, effect.id);
    printf("removed = %d\n", removed);

    return 0;
}