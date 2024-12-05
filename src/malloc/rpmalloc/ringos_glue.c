#include <stddef.h>

#include "rpmalloc.h"

static void initialize_if_needed()
{
  // TODO! this is not thread safe.
  static int initialized = 0;
  if (initialized)
  {
    return;
  }

  rpmalloc_initialize();
  initialized = 1;
}

void* __libc_malloc_impl(size_t n)
{
  initialize_if_needed();

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
