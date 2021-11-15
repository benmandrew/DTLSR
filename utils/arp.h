
#ifndef ARP_H
#define ARP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "logging.h"

#define ARP_BUF_LEN 1024

static size_t _read_arp_file(char** buffer) {
  FILE* file = fopen("/proc/net/arp", "r");
  if (file == NULL) {
    return 0;
  }
  *buffer = (char*) malloc(ARP_BUF_LEN * sizeof(char));
  size_t end = fread(*buffer, sizeof(char), ARP_BUF_LEN-1, file);
  (*buffer)[end] = '\0';
  fclose(file);
  return end;
}

static u_int8_t _split_into_rows(char* table, size_t size, char*** rows) {
  // Detect the number of rows in the
  // table (including the header) so
  // that we know what to allocate
  u_int8_t n_rows = 0;
  for (size_t i = 0; i < size; i++) {
    if (table[i] == '\n') n_rows++;
  }
  // Allocate space for n rows plus a null terminator
  *rows = (char**) malloc(n_rows * sizeof(char*));
  // Split the table into rows
  char* row = strtok(table, "\n");
  u_int8_t i = 0;
  while (row != NULL) {
    (*rows)[i++] = row;
    row = strtok(NULL, "\n");
  }
  // Return the number of rows without the header
  return n_rows - 1;
}

/* Row format is IP address then other stuff
 * Thus we just read out the IP address then stop
 */
static void _parse_ip(char* row, char* out) {
  int i = 0;
  while (i < 15 || row[i] != ' ') {
    out[i] = row[i];
    i++;
  }
  out[i] = '\0';
}

static long* _parse_neighbour_ips(char** rows, u_int8_t n_ips) {
  char ip[16];
  long* addrs = (long*) malloc(n_ips * sizeof(long));
  memset(addrs, 0, n_ips * sizeof(long*));

  for (u_int8_t i = 0; i < n_ips; i++) {
    long addr = addrs[i];
    _parse_ip(rows[i + 1], ip);
    inet_pton(AF_INET, ip, &(addr));
  }
  return addrs;
}

 u_int8_t get_neighbour_ips(long** ips) {
  char* table;
  char** rows;
  size_t len = _read_arp_file(&table);
  u_int8_t n_ips = _split_into_rows(table, len, &rows);
  *ips = _parse_neighbour_ips(rows, n_ips);
  free(table);
  free(rows);
  return n_ips;
}

#endif
