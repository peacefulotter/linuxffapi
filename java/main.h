
// COPY OF ff/c/main.c

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP(x,min,max) (MIN(max, MAX(min, amount)))

#define BITS_TO_LONGS(x) (((x) + 8 * sizeof (unsigned long) - 1) / (8 * sizeof (unsigned long)))
#define FEATURES_LEN BITS_TO_LONGS(FF_CNT)

#define FF_DIRECTION_LEFT 0x4000
#define FF_DIRECTION_RIGHT 0xC000