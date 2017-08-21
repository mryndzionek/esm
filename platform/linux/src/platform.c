#include "platform.h"

#include "esm/esm.h"

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

ESM_THIS_FILE;

static int fd;

void platform_init(void)
{
	srand(time(NULL));
	mkfifo("/tmp/esm", 0666);
	ESM_PRINTF("Listening on '/tmp/esm'\r\n");
	fd = open("/tmp/esm", O_WRONLY);
	ESM_ASSERT_MSG(fd > 0, "%s", strerror(errno));
}

void platform_trace_write(uint8_t const *data, size_t size)
{
	int s = write(fd, data, size);
	ESM_ASSERT_MSG(s > 0, "%s", strerror(errno));
	esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = (void*)0,
			.receiver = trace_esm,
	};
	esm_send_signal(&sig);
}
