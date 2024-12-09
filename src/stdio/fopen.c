#include "stdio_impl.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// ringos
#include <common/ros_debug.h>
#include <services/files/srv_files.h>

FILE *fopen(const char *restrict filename, const char *restrict mode)
{
	FILE *f;
	int fd;
	int flags;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		errno = EINVAL;
		return 0;
	}

	/* Compute the flags to pass to open() */
	flags = __fmodeflags(mode);

	// ringos
	ROS_PRINTF_DEBUG("filename: %s", filename);

	uint64_t srv_files = ringos_get_srv_files();
	if (!srv_files)
	{
		return NULL;
	}

	if (ros_srv_files_open_file(srv_files, filename, &fd) != ROS_STATUS_SUCCESS)
	{
		ROS_PRINTF_ERROR("Failed to open file '%s'.", filename);
		return NULL;
	}

	// fd = sys_open(filename, flags, 0666);
	// if (fd < 0) return 0;
	// if (flags & O_CLOEXEC)
	// 	__syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);

	f = __fdopen(fd, mode);
	if (f) return f;

  ROS_SHOULD_NOT_BE_HERE;

  __syscall(SYS_close, fd);
	return 0;
}
