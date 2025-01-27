The feature is under development
# Simple Socket Implementation

This is a simple TCP socket that can semd simple text messages between a client and server.
 
Head over to the **tcp** folder/directory for the TCP socket implementation and **udp** for the UDP socket implementation.

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

## Webserver
Open the **webserver** folder and compile the *server.c* code and run it just like the tcp/udp examples.

In your browser, enter

``localhost:<port>/sample
``

This should return the sample file in the directory, preferrably as a download.

I could work on this to handle other usecases but frankly, I've lost interest and learnt what I wanted. For a better project, see: https://github.com/IonelPopJara/http-server-c/blob/master/app/server.c
