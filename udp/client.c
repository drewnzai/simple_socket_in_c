#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

int fd = -1;

void handle_close(){
  if(fd != -1){
    shutdown(fd, 2);
    close(fd);
  }

  printf("\nExiting gracefully\n");

  exit(0);
}

int main(int argc, char *argv[]){
  int status;
  struct addrinfo hints, *results;
  struct sockaddr_storage their_addr;
  char buffer[1024];

  printf("Starting the client...\n");

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if((status = getaddrinfo(NULL, argv[1], &hints, &results)) != 0){
    fprintf(stderr, "Could not get address info because %s\n", gai_strerror(status));
    exit(1);
  }

  memcpy(&their_addr, results->ai_addr, results->ai_addrlen);

  if((fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) < 0){
    perror("Could not create the socket");
    exit(1);
  }

  signal(SIGINT, handle_close);

  printf("Client started successfully, sending on port %s\n", argv[1]);

  while(1){
    printf("Enter the message: ");
    fgets(buffer, sizeof(buffer), stdin);

    if(strncmp(buffer, "exit", 4) == 0){
      break;
    }

    buffer[(strlen(buffer)) -1] = '\0';

    int sent = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*) &their_addr, sizeof(their_addr));

    if(sent < 0){
      perror("Error sending data");
      break;
    }

    printf("Bytes sent: %d\n", sent); 
  }

  handle_close();
  freeaddrinfo(results);

}
