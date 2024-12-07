#include <stdint.h>
#include <stdarg.h>

// ringos
#include <kernel/syscalls.h>

void ros_dev_log_printf(const char* function, int32_t line, uint8_t level, const char* format, ...)
{
	char buffer[1024];
	buffer[0] = 0;
	va_list args;
	va_start(args, format);

	vsnprintf(buffer, sizeof buffer, format, args);

	va_end(args);

	ros_sys_debug_string(buffer);
}
