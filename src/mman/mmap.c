#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include "syscall.h"

// ringos
#include <common/ros_debug.h>
#include <kernel/syscalls.h>

static void dummy(void) { }
weak_alias(dummy, __vm_wait);

#define UNIT SYSCALL_MMAP2_UNIT
#define OFF_MASK ((-0x2000ULL << (8*sizeof(syscall_arg_t)-1)) | (UNIT-1))

void *__mmap(void *start, size_t len, int prot, int flags, int fd, off_t off)
{
	// ringos
	ROS_ASSERT(flags == MAP_PRIVATE | MAP_ANONYMOUS);
	ROS_ASSERT(fd == -1);
	ROS_ASSERT(off == 0);
	ROS_ASSERT(len > 0);

	uint64_t address = (uint64_t) start;
	uint64_t num_of_pages = len / ROS_MEMORY_PAGE_SIZE;

	if (ros_sys_allocate_memory(&address, num_of_pages) != ROS_STATUS_SUCCESS)
	{
		return NULL;
	}

	return (void*) address;

	// ringos end.

	long ret;
	if (off & OFF_MASK) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (len >= PTRDIFF_MAX) {
		errno = ENOMEM;
		return MAP_FAILED;
	}
	if (flags & MAP_FIXED) {
		__vm_wait();
	}
#ifdef SYS_mmap2
	ret = __syscall(SYS_mmap2, start, len, prot, flags, fd, off/UNIT);
#else
	ret = __syscall(SYS_mmap, start, len, prot, flags, fd, off);
#endif
	/* Fixup incorrect EPERM from kernel. */
	if (ret == -EPERM && !start && (flags&MAP_ANON) && !(flags&MAP_FIXED))
		ret = -ENOMEM;
	return (void *)__syscall_ret(ret);
}

weak_alias(__mmap, mmap);
