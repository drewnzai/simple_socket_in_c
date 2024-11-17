#include "common.h"

// fd is used to manipulate the primary socket in the hnadle_close function
// req_count is used to track the number of requests that are being serviced.
// The second (always even) request is the request for the favicon hence ignored
static int fd = -1, req_count = 0;

void handle_close(){
  printf("\nShutting down server gracefully\n");
  
  if(fd != -1){
    shutdown(fd, 2);
    exit(0);
  }
}

void close_fd(int* fd){
  shutdown(*fd, 2);
  close(*fd);
}

int main(int argc, char *argv[]){

  struct addrinfo hints, *results;
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size = sizeof(client_addr);
  char buffer[1024], ip_addr[INET_ADDRSTRLEN];
  int servicing_fd, status;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, argv[1], &hints, &results)) != 0){
    fprintf(stderr, "Could not get address info because %s\n", gai_strerror(status));
    exit(1);
  }

  if((fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) < 0){
    perror("Could not create socket");
    exit(1);
  }

  if((bind(fd, results->ai_addr, results->ai_addrlen)) != 0){
    perror("Could not bind to the socket");
    exit(1);
  }

  freeaddrinfo(results);

  printf("Started server, waiting on connections...\n");

  signal(SIGINT, handle_close);

  listen(fd, 20);

  while(1){
    servicing_fd = accept(fd, (struct sockaddr*) &client_addr, &client_addr_size);

    inet_ntop(((struct sockaddr *) &client_addr)->sa_family, &(((struct sockaddr_in *) &client_addr)->sin_addr), ip_addr, INET_ADDRSTRLEN);

    printf("Accepted connection from %s\n", ip_addr);
    
    while(1){
      int received = recv(servicing_fd, buffer, sizeof(buffer), 0);

      if(received == 0){
        printf("Connection closed\n");
        break;
      }else if(received < 0){
        printf("Error receiving data\n");
        break;
      }

      buffer[received-4] = '\0';

      printf("\nReceived: %s\n", buffer);

      memset(buffer, 0, sizeof(buffer));

      if(((++req_count) % 2) != 0){

      printf("\nServer: ");
      fgets(buffer, sizeof(buffer), stdin);      
      
      int sent = send(servicing_fd, buffer, strlen(buffer), 0);
      printf("Sent %d bytes\n\n", sent);

      }else{
        char *ignore = "Ignoring request for favicon";
        strncpy(buffer, ignore, 29);
        send(servicing_fd, buffer, strlen(buffer), 0);
        printf("%s\n\n", ignore);
      }

      memset(buffer, 0, sizeof(buffer));
      close_fd(&servicing_fd);
      break;
    }

}
   

  return 0;
}
