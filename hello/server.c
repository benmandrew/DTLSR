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

#define PORT 8080
#define MAXLINE 1024

#ifdef DEBUG
#define CONF_FILENAME "test.conf"
#else
#define CONF_FILENAME "/hello.conf"
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

void set_destination_ip(struct sockaddr_in* addr) {
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

int driver(int argc, char** argv) {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
	
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

  cliaddr.sin_family = AF_INET;
  set_destination_ip(&cliaddr);
  cliaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	int len, n;
	len = sizeof(cliaddr); //len is value/result

  while (1) {
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                 (struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
           (const struct sockaddr *) &cliaddr, len);
    printf("Hello message sent.\n");
  }
	
	return 0;
}

void make_daemon(void) {
	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();
	if (process_id < 0) {
		exit(EXIT_FAILURE);
	}
	// Parent process
	if (process_id > 0) {
		exit(EXIT_SUCCESS);
	}
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}
	chdir("/");
	// close(STDIN_FILENO);
	// close(STDOUT_FILENO);
	// close(STDERR_FILENO);
}

int main(int argc, char* argv[]) {
  log_f("Hello from server\n");
  log_f("What the hell\n");
	int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd': daemonise = 1; break;
      default:
        fprintf(stderr, "Usage: %s [-d]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (daemonise) {
    make_daemon();
  }
  log_f("Hello from server\n");
  log_f("What the hell\n");
  while (1) {
    sleep(1);
    printf("Hello\n");
    log_f("logg\n");
  }
  // return driver(argc, argv);
}
