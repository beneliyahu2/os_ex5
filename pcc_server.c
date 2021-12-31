//
// Created by Abigail Ben Eliyahu on Dec 2021.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>


int main(int argc, char *argv[]){

    // declaring variables for returned values:
    int totalsent = -1;
    int nsent     = -1;
    int len       = -1;
    int n         =  0;
    int listenfd  = -1;
    int connfd    = -1;

    // initalizing data structure to collect statistics from all clients:
    int chars_stat[256]; // todo malloc?

    // parse command line arguments:
    if (argc != 1){
        fprintf(stderr, "wrong number of arguments.\n");
        exit(1);
    }
    ushort port = strtol(argv[1], NULL, NULL);

    // create a socket to listen to:
    listenfd = socket( AF_INET, SOCK_STREAM, 0 );

    //create a struct for the binding:
    //todo

    // bind the socket to the ip address:
    if( 0 != bind( listenfd, (struct sockaddr*) &serv_addr, addrsize ) )
    {
        printf("\n Error : Bind Failed. %s \n", strerror(errno));
        return 1;
    }

    // start listening:
    if( 0 != listen( listenfd, 10 ) ) //maximum 10 pending clients in the queue
    {
        printf("\n Error : Listen Failed. %s \n", strerror(errno));
        return 1;
    }

    // serving clients from the queue:
    while( 1 ) {
        // Accept a connection.
        connfd = accept( listenfd, NULL, NULL); //todo change to "curr_client_sd"

}

