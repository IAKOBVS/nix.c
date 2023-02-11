#include "/home/james/c/vargc.h"

#define NANO_MULTIPLIER 1000000
#define LOOP_INTERVAL 200 * NANO_MULTIPLIER
#define COUNT_ARR(...) PP_NARG(__VA_ARGS__)

static const unsigned int fanSpeed[] =
	{36, 37, 38, 39, 40, 42, 44, 46, 48, 50, 54, 56, 57, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 87, 88, 89, 90, 91, 92, 93, 94, 96, 98, 100};
static const unsigned int gpuTemp[] =
	{45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83};
