#pragma once

#include "common_platform.h"


typedef enum
{
  MEM_UNKNOWN,

  MEM_LAST_ELEM
} memory_t;


void mem_initialize();
void mem_shutdown();

void* mem_alloc(u64 size, memory_t tag);
void mem_free(void* ptr, u64 size, memory_t);

void* mem_zero(void* ptr, u64 size);
void* mem_copy(void* dest, void* source, u64 size);
void* mem_set(void* dest, i32 value, u64 size);

char* mem_get_stat();
