#ifndef WALLE_MEMORY_MALLOC_TRACKER_H_
#define WALLE_MEMORY_MALLOC_TRACKER_H_
#include <cstdlib>
#include <unistd.h>
namespace walle {
namespace mem{
extern bool memory_exceeded;

void flush_memory_statistics();

void set_memory_limit_indication(ssize_t size);

void * bypass_malloc(size_t size);

void bypass_free(void* ptr, size_t size);

void * bypass_aligned_alloc(size_t alignment, size_t size);

void bypass_aligned_free(void* ptr, size_t size);

ssize_t malloc_tracker_current();

ssize_t malloc_tracker_peak();

void malloc_tracker_reset_peak();

ssize_t malloc_tracker_total_allocs();

void malloc_tracker_print_status();

void malloc_tracker_print_leaks();
}
}
#endif