#include <stddef.h>

#include "rpmalloc.h"

void* __libc_malloc_impl(size_t n)
{
  return rpmalloc(n);
}

void __libc_free(void *p)
{
  rpfree(p);
}

void* __libc_realloc(void *p, size_t n)
{
  return rprealloc(p, n);
}

void* aligned_alloc(size_t align, size_t len)
{
  return rpaligned_alloc(align, len);
}

void __malloc_donate(char *start, char *end)
{
  // This is only used by dynlink, so we probably don't need that.
}
