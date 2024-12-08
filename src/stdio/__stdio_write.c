#include "stdio_impl.h"
#include <sys/uio.h>

// ringos
#include <kernel/syscalls.h>
#include <common/ros_debug.h>

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
	// ringos
	if (f == stdout || f == stderr)
	{
		ROS_ASSERT(len < 1024);
		char buffer[1024];
		strncpy(buffer, buf, len);
		buffer[len] = 0;
		ros_sys_debug_string(buffer);
		return len;
	}

	ROS_NOT_IMPLEMENTED;

	struct iovec iovs[2] = {
		{ .iov_base = f->wbase, .iov_len = f->wpos-f->wbase },
		{ .iov_base = (void *)buf, .iov_len = len }
	};
	struct iovec *iov = iovs;
	size_t rem = iov[0].iov_len + iov[1].iov_len;
	int iovcnt = 2;
	ssize_t cnt;
	for (;;) {
		cnt = syscall(SYS_writev, f->fd, iov, iovcnt);
		if (cnt == rem) {
			f->wend = f->buf + f->buf_size;
			f->wpos = f->wbase = f->buf;
			return len;
		}
		if (cnt < 0) {
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;
			return iovcnt == 2 ? 0 : len-iov[0].iov_len;
		}
		rem -= cnt;
		if (cnt > iov[0].iov_len) {
			cnt -= iov[0].iov_len;
			iov++; iovcnt--;
		}
		iov[0].iov_base = (char *)iov[0].iov_base + cnt;
		iov[0].iov_len -= cnt;
	}
}
