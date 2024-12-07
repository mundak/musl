#include <sys/auxv.h>
#include <errno.h>
#include "libc.h"

// ringos
#include <common/ros_debug.h>

unsigned long __getauxval(unsigned long item)
{
	// ringos:
	// The only reason this code is here, is because LLVM in file lse_atomics/sysaux.inc uses this function.
	// And it's easier to recompile musl than LLVM. It should be fixed there.
	if (item == 16 /* AT_HWCAP */ || item == 26 /* AT_HWCAP2 */)
	{
		return 0x0;
	}

	ROS_NOT_IMPLEMENTED;

	size_t *auxv = libc.auxv;
	if (item == AT_SECURE) return libc.secure;
	for (; *auxv; auxv+=2)
		if (*auxv==item) return auxv[1];
	errno = ENOENT;
	return 0;
}

weak_alias(__getauxval, getauxval);
