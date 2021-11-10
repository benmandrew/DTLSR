
#ifndef LOGGING_H
#define LOGGING_G

#include <stdio.h>
#include <stdarg.h>

// #ifdef DEBUG
// #define LOGFILE "log.txt"
// #else
#define LOGFILE "/log"
// #endif

int log_f(const char* format, ...) {
  FILE* file = fopen(LOGFILE, "a");
  if (file == NULL) {
    return -1;
  }
  va_list argptr;
  va_start(argptr, format);
  vfprintf(file, format, argptr);
  va_end(argptr);
  return fclose(file);
}

#endif
