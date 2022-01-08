
#include "process/perf_logging.h"
#include "process/perf_logging_pi.h"

#include <string.h>

#include "process/logging.h"

struct data {
  unsigned long long ms;
  char d[16];
};

#define LOG_BUF_LEN 8192
#define FILENAME_LEN 80

char buffer[LOG_BUF_LEN];
size_t buf_tail = 0;
char *perf_logfile_name = NULL;
struct timeval tv;

void set_perf_logfile_path(char *path) {
  if (perf_logfile_name == NULL) {
    perf_logfile_name = (char *)malloc(FILENAME_LEN * sizeof(char));
  }
  char *time_str = get_time_str();
  // '<name>_[H-M-S].log'
  sprintf(perf_logfile_name, "%s_%s.log", path, time_str);
  free(time_str);
}

unsigned long long get_ms(void) {
  gettimeofday(&tv, NULL);
  return (unsigned long long)(tv.tv_sec) * 1000 +
         (unsigned long long)(tv.tv_usec) / 1000;
}

void flush() {
  if (perf_logfile_name == NULL) {
    // return 0;
    exit(EXIT_FAILURE);
  }
  FILE *file = fopen(perf_logfile_name, "ab+");
  if (file == NULL) {
    exit(EXIT_FAILURE);
  }
  fwrite(buffer, 1, buf_tail, file);
  fclose(file);
}

void perf_log_f(char data[16]) {
  struct data *d = (struct data*)&buffer[buf_tail];
  d->ms = get_ms();
  memcpy(&d->d, data, 16 * sizeof(char));
  buf_tail += sizeof(struct data);
  if (buf_tail > 0.9 * LOG_BUF_LEN) {
    flush();
		buf_tail = 0;
  }
}
