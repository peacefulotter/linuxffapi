#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define NB_WHEEL_BTNS 18
#define NB_WHEEL_AXIS 6

// Mapping of keys
#define LOGITECH_G29_BTN_CROSS       0x120
#define LOGITECH_G29_BTN_RECT        0x121
#define LOGITECH_G29_BTN_CIRCLE      0x122
#define LOGITECH_G29_BTN_TRIANGLE    0x123
#define LOGITECH_G29_BTN_SHIFT_UP    0x124
#define LOGITECH_G29_BTN_SHIFT_DOWN  0x125
#define LOGITECH_G29_BTN_R2          0x126
#define LOGITECH_G29_BTN_L2          0x127
#define LOGITECH_G29_BTN_SHARE       0x128
#define LOGITECH_G29_BTN_OPTION      0x129
#define LOGITECH_G29_BTN_R3          0x12a
#define LOGITECH_G29_BTN_L3          0x12b
#define LOGITECH_G29_BTN_PLUS        0x2c3
#define LOGITECH_G29_BTN_MINUS       0x2c4
#define LOGITECH_G29_BTN_TURN_RIGHT  0x2c5
#define LOGITECH_G29_BTN_TURN_LEFT   0x2c6
#define LOGITECH_G29_BTN_RETURN      0x2c7
#define LOGITECH_G29_BTN_PLAYSTATION 0x2c8

// Mapping of absolute axis
#define LOGITECH_G29_ABS_WHEEL       ABS_X
#define LOGITECH_G29_ABS_CLUTCH      ABS_Y
#define LOGITECH_G29_ABS_THROTTLE    ABS_Z
#define LOGITECH_G29_ABS_BRAKE       ABS_RZ
#define LOGITECH_G29_ABS_ARROW_X     ABS_HAT0X
#define LOGITECH_G29_ABS_ARROW_Y     ABS_HAT0Y

#define INPUT_NAME_MAX_SIZE 15

struct _wheel_input {
    char name[INPUT_NAME_MAX_SIZE + 1];
    int code;
};

struct _wheel_input wheel_buttons[] = {
    { .name = "BTN_CROSS",          .code = LOGITECH_G29_BTN_CROSS },
    { .name = "BTN_RECT",           .code = LOGITECH_G29_BTN_RECT },
    { .name = "BTN_CIRCLE",         .code = LOGITECH_G29_BTN_CIRCLE },
    { .name = "BTN_TRIANGLE",       .code = LOGITECH_G29_BTN_TRIANGLE },
    { .name = "BTN_SHIFT_UP",       .code = LOGITECH_G29_BTN_SHIFT_UP },
    { .name = "BTN_SHIFT_DOWN",     .code = LOGITECH_G29_BTN_SHIFT_DOWN },
    { .name = "BTN_R2",             .code = LOGITECH_G29_BTN_R2 },
    { .name = "BTN_L2",             .code = LOGITECH_G29_BTN_L2 },
    { .name = "BTN_SHARE",          .code = LOGITECH_G29_BTN_SHARE },
    { .name = "BTN_OPTION",         .code = LOGITECH_G29_BTN_OPTION },
    { .name = "BTN_R3",             .code = LOGITECH_G29_BTN_R3 },
    { .name = "BTN_L3",             .code = LOGITECH_G29_BTN_L3 },
    { .name = "BTN_PLUS",           .code = LOGITECH_G29_BTN_PLUS },
    { .name = "BTN_MINUS",          .code = LOGITECH_G29_BTN_MINUS },
    { .name = "BTN_TURN_RIGHT",     .code = LOGITECH_G29_BTN_TURN_RIGHT },
    { .name = "BTN_TURN_LEFT",      .code = LOGITECH_G29_BTN_TURN_LEFT },
    { .name = "BTN_RETURN",         .code = LOGITECH_G29_BTN_RETURN },
    { .name = "BTN_PLAYSTATION",    .code = LOGITECH_G29_BTN_PLAYSTATION },
};

struct _wheel_input wheel_axis[] = {
    { .name = "ABS_WHEEL",          .code = LOGITECH_G29_ABS_WHEEL },
    { .name = "ABS_CLUTCH",         .code = LOGITECH_G29_ABS_CLUTCH },
    { .name = "ABS_THROTTLE",       .code = LOGITECH_G29_ABS_THROTTLE },
    { .name = "ABS_BRAKE",          .code = LOGITECH_G29_ABS_BRAKE },
    { .name = "ABS_ARROW_X",        .code = LOGITECH_G29_ABS_ARROW_X },
    { .name = "ABS_ARROW_Y",        .code = LOGITECH_G29_ABS_ARROW_Y },
};

struct wheel_status {
    int btns[NB_WHEEL_BTNS];
    int axis[NB_WHEEL_AXIS];
};


int test_bit(const char* bitmask, int bit) {
    return bitmask[bit / 8] & (1 << (bit % 8));
}

void read_btns(int fd, struct wheel_status* wheel) {
    char code_bits[KEY_MAX / 8 + 1];
    memset(&code_bits, 0, sizeof(code_bits));
    
    if (ioctl(fd, EVIOCGKEY(sizeof(code_bits)), code_bits) < 0)
        return;

    for (int i = 0; i < NB_WHEEL_BTNS; i++) {
        int ev_code = wheel_buttons[i].code;
        wheel->btns[i] = test_bit(code_bits, ev_code) > 0 ? 1 : 0;
    }
    // for (int ev_code = 0;ev_code < KEY_MAX; ev_code++) 
    // {
    //     if (test_bit(code_bits, ev_code)) {
    //         printf("code: %x, %d\n", ev_code, ev_code);
    //     }
    // }
}

void read_abs(int fd, struct wheel_status* wheel) 
{
    /* 
        This does not seem to change anything 

    char ev_bits[EV_MAX / 8 + 1];
    memset(&ev_bits, 0, sizeof(ev_bits));
        
    if (ioctl(fd, EVIOCGBIT(EV_SYN, sizeof(ev_bits)), ev_bits) < 0)
        return;

    if (!test_bit(ev_bits, EV_ABS))
        return;
    */

    char code_bits[KEY_MAX / 8 + 1];
    memset(&code_bits, 0, sizeof(code_bits));

    if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(code_bits)), code_bits) < 0)
        return;

    struct input_absinfo absinfo;
    for (int i = 0; i < NB_WHEEL_AXIS; i++) 
    {
        int ev_code = wheel_axis[i].code;
        memset(&absinfo, 0, sizeof(absinfo));
        ioctl(fd, EVIOCGABS(ev_code), &absinfo);
        wheel->axis[i] = absinfo.value;
    }
    // for (int ev_code = 0;ev_code < KEY_MAX; ev_code++) 
    // {
    //     if (test_bit(code_bits, ev_code)) {
    //         memset(&absinfo, 0, sizeof(absinfo));
    //         ioctl(fd, EVIOCGABS(ev_code), &absinfo);
    //         printf(
    //             "code: %x, val: %d, min: %d, max: %d\n",
    //             ev_code, absinfo.value, absinfo.minimum, absinfo.maximum
    //         );
    //     }
    // }
}

void print_status(struct wheel_status* wheel) {
    printf("================\n");
    printf("Buttons: \n");
    for (size_t i = 0; i < NB_WHEEL_BTNS; i++)
    {
        if (wheel->btns[i] == 1)
            printf("%s, ", wheel_buttons[i].name);
    }
    printf("\nAxis: \n");
    for (size_t i = 0; i < NB_WHEEL_AXIS; i++)
    {
        printf("%s: %d\n", wheel_axis[i].name, wheel->axis[i]);
    }
}

void ioctl_capabilities(int fd)
{
    int ev_type, ev_code;
    char ev_bits[EV_MAX/8 + 1], code_bits[KEY_MAX / 8 + 1];
    struct input_absinfo absinfo;

    memset(&ev_bits, 0, sizeof(ev_bits));

    if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0)
        return;

    // Build a dictionary of the device's capabilities
    for (ev_type=0; ev_type < EV_MAX; ev_type++) {
        if (test_bit(ev_bits, ev_type)) {

            // printf("ev_type: %d\n", ev_type);

            memset(&code_bits, 0, sizeof(code_bits));
            ioctl(fd, EVIOCGBIT(ev_type, sizeof(code_bits)), code_bits);

            for (ev_code = 0; ev_code < KEY_MAX; ev_code++) {
                if (test_bit(code_bits, ev_code)) {
                    // Get abs{min,max,fuzz,flat} values for ABS_* event codes
                    if (ev_type == EV_ABS) {
                        memset(&absinfo, 0, sizeof(absinfo));
                        ioctl(fd, EVIOCGABS(ev_code), &absinfo);

                        printf("code: %x, val: %d, min: %d, max: %d, fuzz: %d, flat: %d, res: %d\n",
                                                   ev_code,
                                                   absinfo.value,
                                                   absinfo.minimum,
                                                   absinfo.maximum,
                                                   absinfo.fuzz,
                                                   absinfo.flat,
                                                   absinfo.resolution);
                    }
                }
            }
        }
    }
}

// Only works for joysticks
// size_t get_axis_count(int fd)
// {
//     __u8 axes;
//     if (ioctl(fd, JSIOCGAXES, &axes) == -1)
//         return 0;
//     return axes;
// }

// Only works for joysticks
// size_t get_button_count(int fd)
// {
//     __u8 buttons;
//     if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
//         return 0;
//     return buttons;
// }

int main( void )
{
    int fd = open("/dev/input/event24", O_RDWR);
    printf("fd: %d\n", fd);

    // printf("seek: %d\n", lseek(fd, PEDAL_OFFSET, SEEK_SET));
    // if ( write(fd, &buf, sizeof(buf)) < 0 )
    //     perror("perror output");

    struct wheel_status wheel;
    memset(&wheel, 0, sizeof(wheel));
    // = calloc(1, sizeof(struct wheel_status));

    while (1) {
        read_btns(fd, &wheel);
        read_abs(fd, &wheel);
        print_status(&wheel);
        sleep(0.1);
    }
    
    close(fd);
    // free(wheel);

    return 0;
}