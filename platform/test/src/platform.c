#include "platform.h"

#include "esm/esm.h"
#include "esm/esm_timer.h"

#include <termios.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

#define SIMULATION_TIME_MS	(1UL * 24UL * 3600UL * 1000UL)

ESM_THIS_FILE;

static int tracefd;

__attribute__((weak)) void app_process(char key)
{
	(void)key;
}

void platform_init(void)
{
	srand((unsigned int)time(NULL));
	mkfifo("/tmp/esm", 0666);
	ESM_PRINTF("Listening on '/tmp/esm'\r\n");
	tracefd = open("/tmp/esm", O_WRONLY);
	ESM_ASSERT_MSG(tracefd > 0, "%s", strerror(errno));
}

void platform_wait(void)
{
	ssize_t p;

	p = ESM_RANDOM(1000);
	if(p == 0)
	{
		app_process(' ');
		p = ESM_RANDOM(2);
		if(p == 0)
		{
			esm_global_time++;
			if(esm_global_time == SIMULATION_TIME_MS)
			{
				close(tracefd);
				exit(0);
			}
			if(esm_timer_next() == 0)
			{
				esm_timer_fire();
			}
		}
	}
	else
	{
		p = ESM_RANDOM(2);
		if(p == 0)
		{
			esm_global_time++;
			if(esm_global_time == SIMULATION_TIME_MS)
			{
				close(tracefd);
				exit(0);
			}
			if(esm_timer_next() == 0)
			{
				esm_timer_fire();
			}
		}

		p = ESM_RANDOM(1000);
		if(p == 0)
		{
			app_process(' ');
		}
	}

}

void platform_trace_write(uint8_t const *data, size_t size)
{
	ssize_t s = write(tracefd, data, size);
	ESM_ASSERT_MSG(s > 0, "%s", strerror(errno));
	esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = (void*)0,
			.receiver = trace_esm,
	};
	esm_send_signal(&sig);
}
