#include "stdio_impl.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// ringos
#include <common/ros_debug.h>
#include <services/files/srv_files.h>
#include <kernel/devices/devices_common.h>

static uint64_t get_srv_files()
{
	static uint64_t g_srv_files = 0;
	if (g_srv_files > 0)
	{
		// Already opened.
		return g_srv_files;
	}

	const char* endpoint_name = ROS_SRV_FILES_NAME;
	int32_t interface_version = ROS_SRV_FILES_VERSION_CURRENT;
	int32_t expected_numer_of_functions = ROS_SRV_FILES_RPC_COUNT;

	char full_name[ROS_MAX_DEVICE_NAME_LENGTH + 1];
	snprintf("%s_v%d", endpoint_name, interface_version);

	int32_t num_of_functions;
	if (ros_sys_rpc_get(full_name, &g_srv_files, &num_of_functions) != ROS_STATUS_SUCCESS)
	{
		ROS_PRINTF_ERROR("Failed to get '%s' driver RPC.", endpoint_name);
		return 0;
	}

	if (num_of_functions != expected_numer_of_functions)
	{
		ROS_PRINTF_ERROR("Unexpected RPC interface for '%s'.", endpoint_name);
		return 0;
	}

	return g_srv_files;
}

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

	uint64_t g_srv_files = get_srv_files();
	if (!g_srv_files)
	{
		return NULL;
	}

	if (ros_srv_files_open_file(g_srv_files, filename, &fd) != ROS_STATUS_SUCCESS)
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

	__syscall(SYS_close, fd);
	return 0;
}
