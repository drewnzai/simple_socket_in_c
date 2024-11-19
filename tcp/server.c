#include "common.h"

int fd = -1;

void handle_close(){
  if(fd != -1){
    shutdown(fd, 2);
    close(fd);
  }
  
  printf("\nTerminating the process gracefully\n");
  exit(0);
}

int main(int argc, char *argv[]){

  if(argc < 2){
    fprintf(stderr, "Usage format ./server <port>");
    exit(1);
  }

  printf("Starting server on port %s...\n", argv[1]);

  struct addrinfo hints, *results;
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size = sizeof(their_addr);
  char buffer[1024], ip_addr[INET_ADDRSTRLEN];
  int status, new_fd;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, argv[1], &hints, &results)) != 0){
    fprintf(stderr, "Could not get address info because %s\n", gai_strerror(status));
    exit(1);
  }

  if((fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) < 0){
    perror("Could not create a socket");
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
    new_fd = accept(fd, (struct sockaddr*) &their_addr, &their_addr_size);

    inet_ntop(((struct sockaddr *) &their_addr)->sa_family, &(((struct sockaddr_in *) &their_addr)->sin_addr), ip_addr, INET_ADDRSTRLEN);

    printf("Accepted connection from %s\n", ip_addr);

    while(1){
        int received = recv(new_fd, buffer, sizeof(buffer), 0);
        
        if(received < 0){
          perror("Error receiving data");
          break;
        }else if(received == 0){
          printf("Connection closed\n");          
          break;
        }

        buffer[received] = '\0';

        printf("Received message: %s\n", buffer);

        memset(buffer, 0, sizeof(buffer));
        
        printf("Reply: ");
        
        fgets(buffer, sizeof(buffer), stdin);
        
        if(strncmp(buffer, "exit", 4) == 0){
          break;
        }

        buffer[(strlen(buffer)) -1] = '\0';

        int sent = send(new_fd, buffer, strlen(buffer), 0);

        if(sent < 0){
          perror("Error sending data");
          break;
        }

        printf("Bytes sent: %d\n", sent);

        memset(buffer, 0, sizeof(buffer));
    }
    
    shutdown(new_fd, 2);
    close(new_fd);
  }

  handle_close();

  return 0;

}
