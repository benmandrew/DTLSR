// https://www.geeksforgeeks.org/udp-server-client-implementation-c/

// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "logging.h"
#include "daemonise.h"

#define PORT 8080
#define MAXLINE 1024

#ifdef DEBUG
#define CONF_FILENAME "test.conf"
#else
#define CONF_FILENAME "hello.conf"
#endif

// Kill daemonised server
// lsof -i:8080
// kill -9 $(lsof -t -i:8080)

char* read_file(char* filename) {
  int read_size = -1;
  char* buf = malloc(sizeof(char) * MAXLINE);
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

void set_own_ip(struct sockaddr_in* addr) {
  /*
   * Parsing example:
   * interface eth0
   * ip address 10.0.0.1/24
   * !
   */
  char* contents = read_file(CONF_FILENAME);
  char* pch = NULL;
  strtok(contents, "\n");
  strtok(NULL, " ");
  strtok(NULL, " ");
  pch = strtok(NULL, "/");
  inet_pton(AF_INET, pch, &(addr->sin_addr));
}

void set_ips(struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr) {
  /*
   * Parsing example:
   * interface eth0
   * ip address 10.0.0.2/24
   * !
   */
  char* contents = read_file(CONF_FILENAME);
  char* pch = NULL;
  strtok(contents, "\n");
  strtok(NULL, " ");
  strtok(NULL, " ");
  pch = strtok(NULL, "/");
  if (strcmp(pch, "10.0.0.1") == 0) {
    inet_pton(AF_INET, "10.0.0.2", &(cliaddr->sin_addr));
  } else {
    inet_pton(AF_INET, "10.0.0.1", &(cliaddr->sin_addr));
  }
  inet_pton(AF_INET, pch, &(servaddr->sin_addr));
}

int driver(int argc, char** argv) {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
	
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	// set_own_ip(&servaddr);
	servaddr.sin_port = htons(PORT);

  cliaddr.sin_family = AF_INET;
  // inet_pton(AF_INET, "10.0.0.2", &(cliaddr.sin_addr));
  cliaddr.sin_port = htons(PORT);


	set_ips(&servaddr, &cliaddr);
	
  log_f("Server IP: %s", inet_ntoa(servaddr.sin_addr));
  log_f("Client IP: %s", inet_ntoa(cliaddr.sin_addr));

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		log_f("Bind failed");
		exit(EXIT_FAILURE);
	}
	
	int len, n;
	len = sizeof(cliaddr); //len is value/result

  while (1) {
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                 (struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';
    log_f("Client : %s", buffer);
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
           (const struct sockaddr *) &cliaddr, len);
    log_f("Hello message sent.");
  }
	return 0;
}

int main(int argc, char* argv[]) {
  set_logfile_name("server");
  log_f("Server Started");
	int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd': daemonise = 1; break;
      default:
        log_f("Server usage: %s [-d]", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (daemonise) {
    make_daemon();
    log_f("Daemonisation successful");
  }
  return driver(argc, argv);
}
