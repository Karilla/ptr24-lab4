#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cobalt/stdio.h>
#include <sys/mman.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include "audio_utils.h"

#define PERIOD_AUDIO 1333333

#define DEBUG 1

typedef void *audio_data_t;

void audio_task(void *arg)
{
	ssize_t nb_byte_read;
	rt_task_set_periodic(rt_task_self(), TM_NOW, PERIOD_AUDIO);
	while (1)
	{
		nb_byte_read = read_samples(arg, 256);
		write_samples(arg, nb_byte_read);
		if (rt_task_wait_period(NULL))
		{
			rt_printf("Delai depassed\n");
		}
	}
}

void video_task(void *arg)
{
}

int main(int argc, char *argv[])
{
	uint16_t *audio_data;
	init_audio();

	init_ioctl(0);
	RT_TASK audio;

	init_ioctl(0);

	int ret;

	audio_data = (audio_data_t *)calloc(256, sizeof(audio_data_t));
	if (audio_data == NULL)
	{
		rt_printf("Error allocating memory\n");
		exit(EXIT_FAILURE);
	}
	mlockall(MCL_CURRENT | MCL_FUTURE);

	if (rt_task_create(&audio, "timer", 0, 99, T_JOINABLE) != 0)
	{
		rt_printf("Error creating task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_start(&audio, &audio_task, audio_data) != 0)
	{
		rt_printf("Error starting task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_join(&audio) != 0)
	{
		rt_printf("Error joining task\n");
		exit(EXIT_FAILURE);
	}

	rt_task_delete(&audio);

	munlockall();

	clear_audio();

	return EXIT_SUCCESS;
}
