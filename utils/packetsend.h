
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1024

char* read_conf_file(char* filename) {
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

void set_ips(struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr) {
  /*
   * Parsing example:
   * interface eth0
   * ip address 10.0.0.2/24
   * !
   */
  char* contents = read_conf_file(CONF_FILENAME);
  char* pch = NULL;
  strtok(contents, "\n");
  strtok(NULL, " ");
  strtok(NULL, " ");
  pch = strtok(NULL, "/");
  log_f("pch: %s", pch);
  if (strcmp(pch, "10.0.0.1") == 0) {
    inet_pton(AF_INET, "10.0.0.2", &(servaddr->sin_addr));
  } else {
    inet_pton(AF_INET, "10.0.0.1", &(servaddr->sin_addr));
  }
  inet_pton(AF_INET, pch, &(cliaddr->sin_addr));
}

#endif
