#pragma once
#include "stdio.h"

#define LOG_I(f_, ...) printf((f_"\n"), ##__VA_ARGS__)      // Log info
#define LOG_S(f_, ...) printf((f_"\n"), ##__VA_ARGS__)      // Log success
#define LOG_W(f_, ...) printf((f_"\n"), ##__VA_ARGS__)      // Log warning
#define LOG_E(f_, ...) printf((f_"\n"), ##__VA_ARGS__)      // Log error
#define LOG_D(f_, ...) printf((f_"\n"), ##__VA_ARGS__)      // Log debug