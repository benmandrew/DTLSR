
#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

#define LOG_BUF_LEN 80

char* logfile_name = NULL;

void set_logfile_name(char* name) {
	if (logfile_name == NULL) {
		logfile_name = (char*)malloc(LOG_BUF_LEN * sizeof(char));
	}
	// '<name>.log'
	sprintf(logfile_name, "%s.log", name);
}

static char* _get_time_str(void) {
	time_t now;
	struct tm ts;
	char* time_buf = (char*)malloc(LOG_BUF_LEN * sizeof(char));

	time(&now);
	ts = *localtime(&now);
	int l = strftime(time_buf, LOG_BUF_LEN * sizeof(char), "[%H:%M:%S]- ", &ts);
	time_buf[l] = '\0';
	return time_buf;
}

int log_f(const char* format, ...) {
	if (logfile_name == NULL) {
		exit(EXIT_FAILURE);
	}
	char* time_buf = _get_time_str();
	FILE* file = fopen(logfile_name, "a");
	if (file == NULL) {
		return -1;
	}
	fprintf(file, "%s", time_buf);
	va_list argptr;
	va_start(argptr, format);
	vfprintf(file, format, argptr);
	va_end(argptr);
	fprintf(file, "\n");
	int status = fclose(file);
	free(time_buf);
	return status;
}

#endif
