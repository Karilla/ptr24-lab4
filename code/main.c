#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sched.h>
#include <fcntl.h>
#include <cobalt/stdio.h>
#include <sys/mman.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include "audio_utils.h"

#define PERIOD_AUDIO	 1333333 // 1/((48000 * 2 * 2) /256)
#define PERIOD_VIDEO	 66666666
#define AUDIO_FRAME_SIZE 256
#define VIDEO_FRAME_SIZE 307200 // (320*240) * 4 byte
#define SIZEARRAY	 15 // Arbitrary size for the array

#define SWITCH_1	 (0x1 << 1)
#define SWITCH_0	 0x1

#define DEBUG		 1

typedef void *audio_data_t;
typedef void *video_data_t;

/**
 * @brief Audio task that reads and writes audio data and record it if the switch 0 is on
*/
void audio_task(void *arg)
{
	//Used to measure the time taken by the audio task
	RTIME now, previous;
	RTIME diff[SIZEARRAY];
	ssize_t nb_byte_read;
	bool isRecording = false;
	int fd_wav;
	int count = 0;

	previous = rt_timer_read();
	rt_printf("Audio Task Started\n");

	rt_task_set_periodic(rt_task_self(), TM_NOW, PERIOD_AUDIO);

	// Read and write audio data
	while ((read_switch(0) & SWITCH_1) > 0) {
		now = rt_timer_read();
		nb_byte_read = read_samples(arg, AUDIO_FRAME_SIZE);
		write_samples(arg, nb_byte_read);

		// If isRecording is true, the audio is recorded
		if (isRecording) {
			append_wav_data(fd_wav, arg, nb_byte_read);
			if ((read_switch(0) & SWITCH_0) == 0) {
				rt_printf("Recording ended\n");

				close(fd_wav);
				isRecording = false;
			}
		} else {
			// If the switch 0 is on, the recording starts
			if ((read_switch(0) & SWITCH_0) > 0) {
				rt_printf("Recording started\n");
				fd_wav = open("./funky.wav", O_RDWR | O_CREAT);
				if (fd_wav == NULL) {
					rt_printf("Can't create wav file\n");
					return;
				}
				write_wav_header(fd_wav);
				append_wav_data(fd_wav, arg, nb_byte_read);
				isRecording = true;
			}
		}

		// Measure the time taken by the audio task	to characterize the jitter
		diff[count] = now - previous;
		previous = now;
		count = (count + 1) % SIZEARRAY;
		if (count == (SIZEARRAY - 1)) {
			rt_printf("Time taken by audio\n");

			for (int i = 0; i < count; i++) {
				rt_printf("%lld\n", diff[i]);
			}
		}

		if (rt_task_wait_period(NULL)) {
			rt_printf("Delay audio exceeded\n");
		}
	}

	close(fd_wav);
	rt_printf("Audio task ended\n");
}

/**
 * @brief Video task that reads and writes video data
*/
void video_task(void *arg)
{
	// Used to measure the time taken by the video task
	RTIME now, previous;
	RTIME diff[SIZEARRAY];
	int count = 0;
	previous = rt_timer_read();

	rt_printf("Video Task Started\n");
	ssize_t nb_byte_read;
	int fd_raw;
	int read_size;

	rt_task_set_periodic(rt_task_self(), TM_NOW, PERIOD_VIDEO);

	// Open the raw file of the video
	fd_raw = open("/usr/resources/output_video.raw", O_RDONLY);

	if (fd_raw == NULL) {
		rt_printf("Cant create raw file\n");
		return;
	}
	// Read and write video data
	while ((read_switch(0) & SWITCH_1) > 0) {
		now = rt_timer_read();

		read_size = read(fd_raw, get_video_buffer(), VIDEO_FRAME_SIZE);
		if (read_size < 0) {
			rt_printf("Error reading the video\n");
			close(fd_raw);
			return;
		}
		// Set the video buffer at the beginning of the file for when the file is finished
		if (read_size == 0) {
			lseek(fd_raw, 0, SEEK_SET);
		}
		if (rt_task_wait_period(NULL)) {
			rt_printf("Delay video exceeded\n");
		}

		// Measure the time taken by the audio task	to characterize the jitter
		diff[count] = now - previous;
		previous = now;
		count = (count + 1) % SIZEARRAY;
		if (count == (SIZEARRAY - 1)) {
			rt_printf("Time taken by Video\n");

			for (int i = 0; i < count; i++) {
				rt_printf("%lld\n", diff[i]);
			}
		}
	}
	rt_printf("Video Task Ended\n");
	close(fd_raw);
}

int main(int argc, char *argv[])
{
	// Buffer for audio and video data
	uint16_t *audio_data;
	uint16_t *video_data;
	// CPU affinity to separate audio and video task
	cpu_set_t cpu0;
	cpu_set_t cpu1;
	CPU_SET(0, &cpu0);
	CPU_SET(1, &cpu1);

	// Initialisation
	init_audio();
	init_video();
	init_ioctl(0);
	RT_TASK audio;
	RT_TASK video;

	int ret;
	int err = 0;

	// Allocate memory for audio and video data
	audio_data = (audio_data_t *)calloc(AUDIO_FRAME_SIZE, sizeof(audio_data_t));
	video_data = (video_data_t *)calloc(VIDEO_FRAME_SIZE, sizeof(video_data_t));

	if (audio_data == NULL || video_data == NULL) {
		rt_printf("Error allocating memory\n");
		exit(EXIT_FAILURE);
	}
	mlockall(MCL_CURRENT | MCL_FUTURE);

	if (rt_task_create(&audio, "Audio Timer", 0, 99, T_JOINABLE) != 0 ||
	    rt_task_create(&video, "Video Timer", 0, 99, T_JOINABLE) != 0) {
		rt_printf("Error creating audio task\n");
		err = 1;
		goto end;
	}

	if (rt_task_set_affinity(&audio, &cpu0) != 0 ||
	    rt_task_set_affinity(&video, &cpu1) != 0) {
		rt_printf("Error creating audio task\n");
		err = 1;
		goto end;
	}

	if (rt_task_start(&audio, &audio_task, audio_data) != 0 ||
	    rt_task_start(&video, &video_task, video_data) != 0) {
		rt_printf("Error starting task\n");
		err = 1;
		goto end;
	}

	if (rt_task_join(&audio) != 0 || rt_task_join(&video) != 0) {
		rt_printf("Error joining task\n");
		err = 1;
	}

	rt_task_delete(&audio);
	rt_task_delete(&video);

	munlockall();

end:
	clear_audio();
	clear_video();
	clear_ioctl();
	free(audio_data);
	free(video_data);

	if (err) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
