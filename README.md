# Simple Socket Implementation

## TCP

This is a simple TCP socket that can semd simple text messages between a client and server.
 

To compile the server code, open a terminal and run

`` gcc server.c -o server
``

In another terminal, run 
`` gcc client.c -o client
``

To run them, in their individual terminals run:

`` ./server <port>
``

`` ./client <port>
``

where *<port>* refers to the port you want to run the communication on.
