
#ifndef PERF_LOGGING_H
#define PERF_LOGGING_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

void set_perf_logfile_path(char *path);

void perf_log_f(char data[16]);

#endif
