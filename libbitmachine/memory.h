#pragma once

#ifdef _KERNEL_BUILD

void* memory_sbrk(ptrdiff_t incr);
void* memory_malloc(size_t size);
void  memory_free(void*);
void* memory_realloc(void* ptr, size_t size);
void  memory_cleanupThread(unsigned pid);

extern unsigned _memory_total;

#define memory_total() _memory_total

#endif
