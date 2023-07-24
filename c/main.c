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

int play_effect(int fd, unsigned short code, signed int value) 
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

int autocenter(int fd, double amount) 
{
    double value = 0xFFFFUL * CLAMP(amount, 0, 1);
    return play_effect(fd, FF_AUTOCENTER, value);
}

void get_capabilities(int fd) 
{
    unsigned long features[FEATURES_LEN];
    size_t size = 16;
    int res = ioctl(fd, EVIOCGBIT(EV_FF, size), features);
    printf("res: %d\n", res);
    for (size_t i = FF_RUMBLE; i <= FF_AUTOCENTER; i++)
    {
        printf("%s = %lu\n", features_name[i - FF_RUMBLE], (features[1] >> (i - sizeof(long) * 8)) & 0x01);
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

int open_wheel() 
{
    return open("/dev/input/event6", O_RDWR);
}

int main()  {
    int fd = open_wheel();
    printf("%d\n", fd);

    get_capabilities(fd);
    
    struct ff_effect* effect = create_effect(32767, 100);
    if (effect == NULL)
        return 1;

    int uploaded = upload_effect(fd, effect);
    printf("uploaded = %d, effect.id = %hu \n", uploaded, effect->id);
    play_effect(fd, effect->id, 1);

    sleep(1);

    effect->direction = FF_DIRECTION_RIGHT;
    // effect.u.constant.level = ((signed short) 32767);
    uploaded = upload_effect(fd, effect);
    printf("uploaded = %d, effect.id = %hu \n", uploaded, effect->id);
    play_effect(fd, effect->id, 1);

    sleep(1);

    int removed = remove_effect(fd, effect);
    printf("removed = %d\n", removed);

    free(effect);

    return 0;
}