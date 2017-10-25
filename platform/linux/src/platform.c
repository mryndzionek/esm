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

ESM_THIS_FILE;

static int tracefd;
static int timerfd;
static int epollfd;

static const struct itimerspec timeout = {
		.it_value.tv_sec = 0,
		.it_value.tv_nsec = ESM_SEC_IN_TICKS / ESM_TICKS_PER_SEC,
		.it_interval.tv_sec = 0,
		.it_interval.tv_nsec = ESM_SEC_IN_TICKS / ESM_TICKS_PER_SEC
};

__attribute__((weak)) void app_process(char key)
{
	(void)key;
}

void platform_init(void)
{
	int ret;
	struct epoll_event event;
	static struct termios oldt, newt;

	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(tcflag_t)( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);

	srand((unsigned int)time(NULL));
	mkfifo("/tmp/esm", 0666);
	ESM_PRINTF("Listening on '/tmp/esm'\r\n");
	tracefd = open("/tmp/esm", O_WRONLY);
	ESM_ASSERT_MSG(tracefd > 0, "%s", strerror(errno));

	timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	ESM_ASSERT_MSG(timerfd > 0, "%s", strerror(errno));

	epollfd = epoll_create(1);
	ESM_ASSERT_MSG(epollfd != -1, "%s", strerror(errno));

	event.events = EPOLLIN;
	event.data.fd = timerfd;
	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &event);
	ESM_ASSERT_MSG(ret != -1, "%s", strerror(errno));

	event.data.fd = STDIN_FILENO;
	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	ESM_ASSERT_MSG(ret != -1, "%s", strerror(errno));

	ret = timerfd_settime(timerfd, 0, &timeout, NULL);
	ESM_ASSERT(ret == 0);
}

void platform_wait(void)
{
	ssize_t ret;
	struct epoll_event events[1];

	ret = epoll_wait(epollfd, events, 1, -1);
	ESM_ASSERT_MSG(ret != -1, "%s", strerror(errno));

	if (events[0].data.fd == timerfd) {
		esm_global_time++;
		if(esm_timer_next() == 0)
		{
			esm_timer_fire();
		}
		ret = timerfd_settime(timerfd, 0, &timeout, NULL);
		ESM_ASSERT(ret == 0);
	}
	else if(events[0].data.fd == STDIN_FILENO) {
		char key;
		ret = read(STDIN_FILENO, &key, 1);
		ESM_ASSERT(ret == 1);
		app_process(key);
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
