#include <stdio.h>
#include "nvidia/gdk/nvml.h"
#include <time.h>
#include "/home/james/c/vargc.h"
#include "nvtemp.h"

#define NANO_MULTIPLIER 1000000
#define LOOP_INTERVAL 200 * NANO_MULTIPLIER
#define COUNT_ARR(...) PP_NARG(__VA_ARGS__)

int main()
{
	nvmlDevice_t device;
	if (nvmlInit() != NVML_SUCCESS)
		goto ERROR;
	if (nvmlDeviceGetHandleByIndex(0, &device) != NVML_SUCCESS)
		goto ERROR_SHUTDOWN;
	unsigned int temp;
	unsigned int tmp = 0;
	unsigned int speed;
	unsigned int i;
	struct timespec interval;
	interval.tv_nsec = LOOP_INTERVAL;
	for (;;) {
		if ((temp = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS))
			goto ERROR_SHUTDOWN;
		if (temp != tmp) {
			for (int i = 0; i < arrSize; ++i)
				;
		}
		tmp = temp;
		nanosleep(&interval, NULL);
	}
	if (nvmlShutdown() != NVML_SUCCESS)
		goto ERROR;
	return 0;
ERROR_SHUTDOWN:
	nvmlShutdown();
ERROR:
	perror("");
	return 1;
}
