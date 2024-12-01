#include <stddef.h>

void* __libc_malloc_impl(size_t n)
{
  return NULL;
}

void __libc_free(void *p)
{
}

void* __libc_realloc(void *p, size_t n)
{
  return NULL;
}

void* aligned_alloc(size_t align, size_t len)
{
  return NULL;
}

void __malloc_donate(char *start, char *end)
{
  // This is only used by dynlink, so we probably don't need that.
}
