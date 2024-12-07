#include "stdio_impl.h"
#include <sys/ioctl.h>

// ringos
#include <kernel/syscalls.h>

size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
	// ringos
	ros_sys_debug_string(buf);

	return;

	struct winsize wsz;
	f->write = __stdio_write;
	if (!(f->flags & F_SVB) && __syscall(SYS_ioctl, f->fd, TIOCGWINSZ, &wsz))
		f->lbf = -1;
	return __stdio_write(f, buf, len);
}
