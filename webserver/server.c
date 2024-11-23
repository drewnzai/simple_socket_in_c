#include "common.h"

// fd is used to manipulate the primary socket in the hnadle_close function
static int fd = -1;

void handle_close();
void close_fd(int*);

int main(int argc, char *argv[]){

  struct addrinfo hints, *results;
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size = sizeof(client_addr);
  char buffer[1024], ip_addr[INET_ADDRSTRLEN], file_name[100];
  FILE *file;
  int servicing_fd, status, sent = 0;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, argv[1], &hints, &results))){
    fprintf(stderr, "Could not get address info because %s\n", gai_strerror(status));
    exit(1);
  }

  if((fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) < 0){
    perror("Could not create socket");
    exit(1);
  }

  if((bind(fd, results->ai_addr, results->ai_addrlen))){
    perror("Could not bind to the socket");
    exit(1);
  }
  
  listen(fd, 10);

  signal(SIGINT, handle_close);

  printf("Starting the server...\n");
  
  while(1){

    int servicing_fd = accept(fd,
     ((struct sockaddr *) &client_addr),
     &client_addr_size);

    
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

      // Extract the name of the requested file
     if(sscanf(buffer, "GET /%s HTTP/1.1", file_name)){

      // Used to ignore GET requests for a favicon
      if((strcmp(file_name, "favicon.ico")) == 0){
        close_fd(&servicing_fd);
        break;
     }

      inet_ntop(((struct sockaddr *) &client_addr)->sa_family, 
                &(((struct sockaddr_in *) &client_addr)->sin_addr), 
                ip_addr,
                INET_ADDRSTRLEN);

      printf("\nAccepted connection from %s\nFile Request: %s\n", ip_addr, file_name);

      memset(buffer, 0, sizeof(buffer));

      if((file = fopen(file_name, "rb")) != NULL){
        
        // Traverse the whole file
        if((fseek(file, 0, SEEK_END)) < 0){
            fprintf(stderr, "Could not traverse file contents\n");
            break;
          }

        // Determine the size of the file; rewind(file*) returns the file pointer to the starting point
        size_t file_size = ftell(file);
        rewind(file);

        void *file_contents = malloc(file_size);

        if((fread(file_contents, 1, file_size, file)) != file_size){
            fprintf(stderr, "Could not read the file\n");
            break;
          }

        fclose(file);

        // https://github.com/IonelPopJara/http-server-c/blob/master/app/server.c
        sprintf(buffer, 
                "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %ld\r\n\r\n%s", 
                file_size,
                (char *) file_contents);

		    printf("Sending response: %s\n", buffer);

        sent = send(servicing_fd, buffer, strlen(buffer), 0);

        free(file_contents);        
      }else{
          printf("Could not process request; File does not exist\n");
      }

      memset(buffer, 0, sizeof(buffer));
      close_fd(&servicing_fd);
      break;
    }
  }
}
  freeaddrinfo(results);
  return 0;
}

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
