#include "common.h"

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
  socklen_t their_addr_size = sizeof(their_addr);
  char buffer[1024], ip_addr[INET_ADDRSTRLEN];

  printf("Starting the client...\n");

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if((status = getaddrinfo(NULL, argv[1], &hints, &results)) != 0){
    fprintf(stderr, "Could not get address info because %s\n", gai_strerror(status));
    exit(1);
  }

  if((fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) < 0){
    perror("Could not create the socket");
    exit(1);
  }

  if((connect(fd, results->ai_addr, results->ai_addrlen)) < 0){
    perror("Could not connect");
    exit(1);
  }

  inet_ntop(results->ai_family, &(((struct sockaddr_in *) results)->sin_addr), ip_addr, INET_ADDRSTRLEN);
  signal(SIGINT, handle_close);

  printf("Client started successfully, sending on %s:%s\n", ip_addr, argv[1]);

  while(1){
    printf("Enter the message: ");
    fgets(buffer, sizeof(buffer), stdin);

    if(strncmp(buffer, "exit", 4) == 0){
      break;
    }

    buffer[(strlen(buffer)) -1] = '\0';

    int sent = send(fd, buffer, strlen(buffer), 0);

    if(sent < 0){
      perror("Error sending data");
      break;
    }

    printf("Bytes sent: %d\n", sent); 

    int received = recv(fd, buffer, sizeof(buffer), 0);
        
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
  }

  handle_close();
  freeaddrinfo(results);

}
