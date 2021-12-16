
#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1024

char* read_file(char* filename) {
  int read_size = -1;
  char* buf = (char*)malloc(sizeof(char) * MAXLINE);
  if (buf == NULL) {
    return NULL;
  }
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    free(buf);
    return NULL;
  }
  read_size = fread(buf, sizeof(char), MAXLINE, file);
  fclose(file);
  buf[read_size] = '\0';
  if (read_size < MAXLINE) {
    return buf;
  } else {
    free(buf);
    return NULL;
  }
}

#endif
