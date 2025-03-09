#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif // LOG_LEVEL

// Log levels
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

extern void initialise_monitor_handles(void);

// Semihosting print
#if (SEMIHOSTING==1)
#define PRINT printf
#else
// Should implement uart based logging here
#define PRINT 
#endif // SEMIHOSTING
#ifdef ENABLE_LOGGING
#define ERROR(...) do { if (LOG_LEVEL >= LOG_LEVEL_ERROR) { PRINT("ERROR: "); PRINT(__VA_ARGS__); PRINT("\n"); } } while(0)
#define WARN(...) do { if (LOG_LEVEL >= LOG_LEVEL_WARN) { PRINT("WARN: "); PRINT(__VA_ARGS__); PRINT("\r\n"); } } while(0)
#define INFO(...) do { if (LOG_LEVEL >= LOG_LEVEL_INFO) { PRINT("INFO: "); PRINT(__VA_ARGS__); PRINT("\r\n"); } } while(0)
#define DEBUG(...) do { if (LOG_LEVEL >= LOG_LEVEL_DEBUG) { PRINT("DEBUG: "); PRINT(__VA_ARGS__); PRINT("\r\n"); } } while(0)
#else
#define initialise_monitor_handles(...)
#define ERROR(...)
#define WARN(...)
#define INFO(...)
#define DEBUG(...)
#endif // ENABLE_LOGGING

#endif // UTIL_H