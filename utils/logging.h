
#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

void set_logfile_name(char* name);

int log_f(const char* format, ...);

#endif
