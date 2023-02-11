#include <stdio.h>
#include "nvidia/gdk/nvml.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "nvtemp.h"
/* #include "/home/james/c/vargc.h" */

/* #define COUNT_ARR(...) PP_NARG(__VA_ARGS__) */
#define NANO_MULTIPLIER 1000000
#define LOOP_INTERVAL 200 * NANO_MULTIPLIER

int main()
{
	unsigned int speedAt[83];
	for (int i = 0, j = sizeof fanSpeed/sizeof fanSpeed[0]; i < j; ++i)
		speedAt[gpuTemp[i]] = fanSpeed[i];
	nvmlDevice_t device;
	if (nvmlInit() != NVML_SUCCESS)
		goto ERROR;
	if (nvmlDeviceGetHandleByIndex(0, &device) != NVML_SUCCESS)
		goto ERROR_SHUTDOWN;
	int temp;
	int tmp = 0;
	unsigned int speed;
	unsigned int i;
	struct timespec interval;
	interval.tv_nsec = LOOP_INTERVAL;
	for (;;) {
		if (nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
			goto ERROR_SHUTDOWN;
		if (abs(temp - tmp) > 1) {
			if (!fork())
				goto ERROR_SHUTDOWN;
			execl("/bin/nvidia-settings", "nvidia-settings -a \"[gpu:0]/GPUFanControlState=1\" -a \"[fan:0]/GPUTargetFanSpeed=", speedAt[temp], NULL);
		}
		switch (temp) {
		case 45: speed = 36; break; case 46: speed = 37; break; case 47: speed = 38; break; case 48: speed = 39; break; case 49: speed = 40; break; case 50: speed = 42; break; case 51: speed = 44; break; case 52: speed = 46; break; case 53: speed = 48; break; case 54: speed = 50; break; case 55: speed = 54; break; case 56: speed = 56; break; case 57: speed = 57; break; case 58: speed = 58; break; case 59: speed = 60; break; case 60: speed = 62; break; case 61: speed = 64; break; case 62: speed = 66; break; case 63: speed = 68; break; case 64: speed = 70; break; case 65: speed = 72; break; case 66: speed = 74; break; case 67: speed = 76; break; case 68: speed = 78; break; case 69: speed = 80; break; case 70: speed = 82; break; case 71: speed = 84; break; case 72: speed = 86; break; case 73: speed = 87; break; case 74: speed = 88; break; case 75: speed = 89; break; case 76: speed = 90; break; case 77: speed = 91; break; case 78: speed = 92; break; case 79: speed = 93; break; case 80: speed = 94; break; case 81: speed = 96; break; case 82: speed = 98; break; case 83: speed = 100; break;
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
