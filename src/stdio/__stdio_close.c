#include "stdio_impl.h"
#include "aio_impl.h"

// ringos
#include <common/ros_debug.h>

static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int __stdio_close(FILE *f)
{
	uint64_t srv_files = ringos_get_srv_files();
	if (!srv_files)
	{
		return 1;
	}

	if (ros_srv_files_close_file(srv_files, f->fd) != ROS_STATUS_SUCCESS)
	{
		ROS_PRINTF_ERROR("ros_srv_files_close_file failed.");
		return 1;
	}

	return 0;

	// return syscall(SYS_close, __aio_close(f->fd));
}
