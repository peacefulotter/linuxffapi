
// COPY OF ff/c/main.h without functions header;
#include <jni.h>        
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(x,min,max) (MIN(max, MAX(min, amount)))

#define BITS_TO_LONGS(x) (((x) + 8 * sizeof (unsigned long) - 1) / (8 * sizeof (unsigned long)))
#define FEATURES_LEN BITS_TO_LONGS(FF_CNT)

#define FF_DIRECTION_LEFT 0x4000
#define FF_DIRECTION_RIGHT 0xC000

#define EVENT_DEV_NAME "event"
#define DEV_INPUT_FOLDER "/dev/input"
#define EVENT_PATH_SIZE (sizeof(DEV_INPUT_FOLDER) + sizeof(char) + sizeof(EVENT_DEV_NAME) + sizeof(int))


#define nBitsPerUchar          (sizeof(unsigned char) * 8)
/* Number of unsigned chars to contain a given number of bits */
#define nUcharsForNBits(nBits) ((((nBits)-1)/nBitsPerUchar)+1)
/* Index=Offset of given bit in 1 unsigned char */
#define bitOffsetInUchar(bit)  ((bit)%nBitsPerUchar)
/* Index=Offset of the unsigned char associated to the bit at the given index=offset */
#define ucharIndexForBit(bit)  ((bit)/nBitsPerUchar)
/* Value of an unsigned char with bit set at given index=offset */
#define ucharValueForBit(bit)  (((unsigned char)(1))<<bitOffsetInUchar(bit))
/* Test the bit with given index=offset in an unsigned char array */
#define testBit(bit, array)    ((array[ucharIndexForBit(bit)] >> bitOffsetInUchar(bit)) & 1)
