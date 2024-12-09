#ifndef _STDIO_IMPL_H
#define _STDIO_IMPL_H

#include <stdio.h>
#include "syscall.h"

#define UNGET 8

#define FFINALLOCK(f) ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FLOCK(f) int __need_unlock = ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FUNLOCK(f) do { if (__need_unlock) __unlockfile((f)); } while (0)

#define F_PERM 1
#define F_NORD 4
#define F_NOWR 8
#define F_EOF 16
#define F_ERR 32
#define F_SVB 64
#define F_APP 128

struct _IO_FILE {
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(FILE *);
	unsigned char *wend, *wpos;
	unsigned char *mustbezero_1;
	unsigned char *wbase;
	size_t (*read)(FILE *, unsigned char *, size_t);
	size_t (*write)(FILE *, const unsigned char *, size_t);
	off_t (*seek)(FILE *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	FILE *prev, *next;
	int fd;
	int pipe_pid;
	long lockcount;
	int mode;
	volatile int lock;
	int lbf;
	void *cookie;
	off_t off;
	char *getln_buf;
	void *mustbezero_2;
	unsigned char *shend;
	off_t shlim, shcnt;
	FILE *prev_locked, *next_locked;
	struct __locale_struct *locale;
};

extern hidden FILE *volatile __stdin_used;
extern hidden FILE *volatile __stdout_used;
extern hidden FILE *volatile __stderr_used;

hidden int __lockfile(FILE *);
hidden void __unlockfile(FILE *);

hidden size_t __stdio_read(FILE *, unsigned char *, size_t);
hidden size_t __stdio_write(FILE *, const unsigned char *, size_t);
hidden size_t __stdout_write(FILE *, const unsigned char *, size_t);
hidden off_t __stdio_seek(FILE *, off_t, int);
hidden int __stdio_close(FILE *);

hidden int __toread(FILE *);
hidden int __towrite(FILE *);

hidden void __stdio_exit(void);
hidden void __stdio_exit_needed(void);

#if defined(__PIC__) && (100*__GNUC__+__GNUC_MINOR__ >= 303)
__attribute__((visibility("protected")))
#endif
int __overflow(FILE *, int), __uflow(FILE *);

hidden int __fseeko(FILE *, off_t, int);
hidden int __fseeko_unlocked(FILE *, off_t, int);
hidden off_t __ftello(FILE *);
hidden off_t __ftello_unlocked(FILE *);
hidden size_t __fwritex(const unsigned char *, size_t, FILE *);
hidden int __putc_unlocked(int, FILE *);

hidden FILE *__fdopen(int, const char *);
hidden int __fmodeflags(const char *);

hidden FILE *__ofl_add(FILE *f);
hidden FILE **__ofl_lock(void);
hidden void __ofl_unlock(void);

struct __pthread;
hidden void __register_locked_file(FILE *, struct __pthread *);
hidden void __unlist_locked_file(FILE *);
hidden void __do_orphaned_stdio_locks(void);

#define MAYBE_WAITERS 0x40000000

hidden void __getopt_msg(const char *, const char *, const char *, size_t);

#define feof(f) ((f)->flags & F_EOF)
#define ferror(f) ((f)->flags & F_ERR)

#define getc_unlocked(f) \
	( ((f)->rpos != (f)->rend) ? *(f)->rpos++ : __uflow((f)) )

#define putc_unlocked(c, f) \
	( (((unsigned char)(c)!=(f)->lbf && (f)->wpos!=(f)->wend)) \
	? *(f)->wpos++ = (unsigned char)(c) \
	: __overflow((f),(unsigned char)(c)) )

/* Caller-allocated FILE * operations */
hidden FILE *__fopen_rb_ca(const char *, FILE *, unsigned char *, size_t);
hidden int __fclose_ca(FILE *);

// ringos
#include <common/ros_debug.h>
#include <services/files/srv_files.h>
#include <kernel/devices/devices_common.h>

inline uint64_t ringos_get_srv_files()
{
	static uint64_t srv_files = 0;
	if (srv_files > 0)
	{
		// Already opened.
		return srv_files;
	}

	const char* endpoint_name = ROS_SRV_FILES_NAME;
	int32_t interface_version = ROS_SRV_FILES_VERSION_CURRENT;
	int32_t expected_numer_of_functions = ROS_SRV_FILES_RPC_COUNT;

	char full_name[ROS_MAX_DEVICE_NAME_LENGTH + 1];
	snprintf(full_name, ROS_MAX_DEVICE_NAME_LENGTH, "%s_v%d", endpoint_name, interface_version);

	int32_t num_of_functions;
	if (ros_sys_rpc_get(full_name, &srv_files, &num_of_functions) != ROS_STATUS_SUCCESS)
	{
		ROS_PRINTF_ERROR("Failed to get '%s' driver RPC.", endpoint_name);
		return 0;
	}

	if (num_of_functions != expected_numer_of_functions)
	{
		ROS_PRINTF_ERROR("Unexpected RPC interface for '%s'.", endpoint_name);
		return 0;
	}

	return srv_files;
}


#endif
