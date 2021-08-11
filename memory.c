#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"


typedef struct
{
  u64 total_allocated;
  u64 tagged_allocated[MEM_LAST_ELEM];
} mem_stat;


internal const char* g_mem_tags[MEM_LAST_ELEM] = {"UNKNOWN"};


global_var mem_stat g_stats;

void mem_initialize()
{
  memset(&g_stats, 0, sizeof(mem_stat));
}


void mem_shutdown();


void* mem_alloc(u64 size, memory_t tag)
{
  g_stats.total_allocated += size;
  g_stats.tagged_allocated[tag] += size;

  void* ptr = malloc(size);
  mem_zero(ptr, size);

  return ptr;
}


void mem_free(void* ptr, u64 size, memory_t tag)
{
  g_stats.total_allocated -= size;
  g_stats.tagged_allocated[tag] -= size;

  free(ptr);
}


void* mem_zero(void* ptr, u64 size)
{
  return mem_set(ptr, 0, size);
}


void* mem_copy(void* dest, void* source, u64 size)
{
  return memcpy(dest, source, size);
}


void* mem_set(void* dest, i32 value, u64 size)
{
  return memset(dest, value, size);
}


#define BUF_SIZE 8000
char* mem_get_stat()
{
  const u64 kib = 1024;
  const u64 mib = kib * 1024;
  const u64 gib = mib * 1024;

  char buffer[BUF_SIZE] = "Memory usage:\n";

  u64 offset = strlen(buffer);
  for (u32 i = 0; i < MEM_LAST_ELEM; ++i) {
    char unit[4] = "XiB";
    f32 amount = 1.0f;
    u64 current_tot = g_stats.tagged_allocated[i];
    if (current_tot >= gib) {
      unit[0] = 'G';
      amount = current_tot / (f32)gib;
    } else if (current_tot >= mib) {
      unit[0] = 'M';
      amount = current_tot / (f32)mib;
    } else if (current_tot >= kib) {
      unit[0] = 'K';
      amount = current_tot / (f32)kib;
    } else {
      unit[0] = 'B';
      unit[1] = 0;
      amount = (f32)current_tot;
    }

    i32 len = snprintf(buffer + offset, BUF_SIZE, "%s: %.2f%s\n", g_mem_tags[i],
                       amount, unit);
    offset += len;
  }

  char* out_string = strdup(buffer);
  return out_string;
}