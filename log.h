#pragma once
#include "stdio.h"


// Log info: White
#define _LOG_I(f_, ...) printf("\033[37m" f_ "\033[37m%c", __VA_ARGS__)
#define LOG_I(...) _LOG_I(__VA_ARGS__, '\n')
// #undef _LOG_I

// Log success: Green
#define _LOG_S(f_, ...) printf("\033[92m" f_ "\033[37m%c", __VA_ARGS__)
#define LOG_S(...) _LOG_S(__VA_ARGS__, '\n')
// #undef _LOG_S

// Log warning: Yellow
#define _LOG_W(f_, ...) printf("\033[33m" f_ "\033[37m%c", __VA_ARGS__)
#define LOG_W(...) _LOG_W(__VA_ARGS__, '\n')
// #undef _LOG_W

// Log error: Red
#define _LOG_E(f_, ...) printf("\033[31m" f_ "\033[37m%c", __VA_ARGS__)
#define LOG_E(...) _LOG_E(__VA_ARGS__, '\n')
// #undef _LOG_E

#define LOG_D(...)  // printf((f_"\n"), ##__VA_ARGS__) // Log debug
