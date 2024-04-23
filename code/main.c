#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cobalt/stdio.h>
#include <sys/mman.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include "audio_utils.h"

void audio_task(void *arg)
{
	struct *wav_header wh;
    struct wav_file wf;

    uint16_t *audio_data; 




    rt_task_wait_period(NULL);
}

int main(int argc, char *argv[])
{
	init_audio();

	init_ioctl(0);
	RT_TASK audio;

	init_ioctl(0);

	int ret;

	mlockall(MCL_CURRENT | MCL_FUTURE);

	if (rt_task_create(&timer, "timer", 0, 99, T_JOINABLE) != 0) {
		rt_printf("Error creating task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_start(&timer, &xenomai_timer, NULL) != 0) {
		rt_printf("Error starting task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_join(&timer) != 0) {
		rt_printf("Error joining task\n");
		exit(EXIT_FAILURE);
	}

	rt_task_delete(&timer);

	munlockall();

	clear_audio();

	return EXIT_SUCCESS;
}
