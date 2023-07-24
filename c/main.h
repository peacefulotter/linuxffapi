#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// PATH: /sys/devices/pci0000:00/0000:00:14.0/usb3/3-1/3-1:1.0/0003:046D:C24F.000E/input/input205/device

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(x,min,max) (MIN(max, MAX(min, amount)))

#define BITS_TO_LONGS(x) (((x) + 8 * sizeof (unsigned long) - 1) / (8 * sizeof (unsigned long)))
#define FEATURES_LEN BITS_TO_LONGS(FF_CNT)

#define FF_DIRECTION_LEFT 0x4000
#define FF_DIRECTION_RIGHT 0xC000

/**
 * @brief Uploads an effect to the device corresponding to the handler fd
 * 
 * @param fd: handler
 * @param effect
 * @return int: whether uploading the effect worked or not
 */
int upload_effect(int fd, struct ff_effect* effect);

/**
 * @brief Play an effect to the device corresponding to the handler fd
 *        Playing an effect REQUIRES it to be uploaded first (using upload_effect)  
 * 
 * @param fd: handler 
 * @param code: FF_{CODE} or effect->id
 * @param value: effect strength if used with FF_{CODE} or # of repetitions if used with effect->id
 * @return int: whether playing the effect worked or not
 */
int play_effect(int fd, unsigned short code, signed int value);

/**
 * @brief Remove effect uploaded to the handler fd
 *      /!\ Does not free the ff_effect pointer
 * 
 * @param fd: handler
 * @param effect: effect to be removed 
 * @return int: whether removing the effect worked or not
 */
int remove_effect(int fd, struct ff_effect* effect);

/**
 * @brief Sets the force to autocenter the device
 * 
 * @param fd: handler
 * @param amount: value between 0 and 1, 
 *      0 being no force is applied to center the device, 
 *      1 being the strongest force that can be applied to the device
 * @return int: whether setting the autocenter force worked or not
 */
int autocenter(int fd, double amount);

/**
 * @brief Get the capabilities of the device, such as the various effects it supports
 * 
 * @param fd: handler
 */
void get_capabilities(int fd);

/** 
 * @brief Creates an effect.
 *      /!\ This function does not upload nor play the effect, which are both required in order for an effect to work
 * 
 * @param level: effect strength (between -32768 and 32767)
 * @param length: time the effect lasts (in ms)
 * @return struct ff_effect*: a pointer to the effect or NULL if the operation failed
**/
struct ff_effect* create_effect(signed short level, unsigned short length);

/** 
 * @brief Open the handler corresponding to the device
 * 
 * @return int: fd (handler)
**/
int open_wheel();