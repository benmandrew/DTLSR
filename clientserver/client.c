// https://www.geeksforgeeks.org/udp-server-client-implementation-c/

// Client side implementation of UDP client-server model
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
#include "event.h"
#include "packetsend.h"

#define PORT 8080

// #ifdef DEBUG
// #define CONF_FILENAME "test.conf"
// #else
// #define CONF_FILENAME "hello.conf"
// #endif

struct rtentry* neighbours;

// Kill daemonised server
// lsof -i:8080
// kill -9 $(lsof -t -i:8080)

/*
 * Parsing example:
 * interface eth0
 * ip address 10.0.0.2/24
 * !
 */
// void set_ips(struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr) {
//   char* contents = read_file(CONF_FILENAME);
//   char* pch = NULL;
//   strtok(contents, "\n");
//   strtok(NULL, " ");
//   strtok(NULL, " ");
//   pch = strtok(NULL, "/");
//   log_f("pch: %s", pch);
//   if (strcmp(pch, "10.0.0.1") == 0) {
//     inet_pton(AF_INET, "10.0.0.2", &(servaddr->sin_addr));
//   } else {
//     inet_pton(AF_INET, "10.0.0.1", &(servaddr->sin_addr));
//   }
//   inet_pton(AF_INET, pch, &(cliaddr->sin_addr));
// }

// Driver code
int driver(int argc, char** argv) {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from client";
	// struct sockaddr_in servaddr, myaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	// // Filling server information
	// memset(&myaddr, 0, sizeof(myaddr));
	// myaddr.sin_family = AF_INET; // IPv4
	// myaddr.sin_port = htons(PORT);
	// // Filling server information
	// memset(&servaddr, 0, sizeof(servaddr));
	// servaddr.sin_family = AF_INET;
  // servaddr.sin_port = htons(PORT);

	// set_ips(&servaddr, &myaddr);

  // log_f("Server IP: %s", inet_ntoa(servaddr.sin_addr));
  // log_f("Client IP: %s", inet_ntoa(myaddr.sin_addr));

  int n = get_neighbours(&neighbours, "hello");

  struct sockaddr_in* servaddr = (struct sockaddr_in*)&(neighbours[0].rt_dst);
  
  
  
  servaddr->sin_family = AF_INET;
  servaddr->sin_addr.s_addr = inet_addr("10.0.0.2");
  servaddr->sin_port = htons(PORT);

  neighbours_log(neighbours, n);

  event_init();
  int timer = timer_add(0, 100000);

  while (1) {
    if (timer_wait(timer) < 0) {
      continue;
    }
    timer_reset(timer);
    // sleep(3);
		int n, len;
		sendto(sockfd, (const char *)hello, strlen(hello),
			MSG_CONFIRM, (const struct sockaddr *) servaddr,
				sizeof(servaddr));
		log_f("Hello message sent.");
			
		// n = recvfrom(sockfd, (char *)buffer, MAXLINE,
		// 			MSG_WAITALL, (struct sockaddr *) servaddr,
		// 			&len);
		// buffer[n] = '\0';
		// log_f("Server : %s", buffer);
	}
	close(sockfd);
  timer_dealloc(timer);
	return 0;
}

int main(int argc, char* argv[]) {
  set_logfile_name("client");
  log_f("Client started");

	int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd': daemonise = 1; break;
      default:
        fprintf(stderr, "Usage: %s [-d]", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (daemonise) {
    make_daemon();
    log_f("Daemonisation successful");
  }
  return driver(argc, argv);
}
