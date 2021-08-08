#pragma once
#include "stdio.h"

// Log info: White
#define LOG_I(f_, ...) printf(("\033[37m" f_ "\033[37m\n"), ##__VA_ARGS__)

// Log success: Green
#define LOG_S(f_, ...) printf(("\033[92m" f_ "\033[37m\n"), ##__VA_ARGS__)

// Log warning: Yellow
#define LOG_W(f_, ...) printf(("\033[33m" f_ "\033[37m\n"), ##__VA_ARGS__)

// Log error: Red
#define LOG_E(f_, ...) printf(("\033[31m" f_ "\033[37m\n"), ##__VA_ARGS__)

#define LOG_D(f_, ...)  // printf((f_"\n"), ##__VA_ARGS__) // Log debug