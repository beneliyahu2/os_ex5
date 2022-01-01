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
    int listenfd  = -1;
    int connfd    = -1;

    // initalizing data structure to collect statistics from all clients:
//    int chars_stat[256]; todo uncomment

    // parse command line arguments: todo uncomment
//    if (argc != 1){
//        fprintf(stderr, "wrong number of arguments.\n");
//        exit(1);
//    }
//    u_int16_t port_num = strtol(argv[1], NULL, 10);

    // create a socket to listen to:
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if( listenfd < 0){
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    //create a struct for the binding:
    struct sockaddr_in serv_addr;
    socklen_t addrsize = sizeof(struct sockaddr_in );

    memset( &serv_addr, 0, addrsize ); //starting with clean (zero values)
    serv_addr.sin_family = AF_INET; //
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = any local machine address
    serv_addr.sin_port = htons(10000); //port number 10000

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
        connfd = accept( listenfd, NULL, NULL); //todo change name to "curr_client_sd"
        if( connfd < 0 ){
            printf("\n Error : Accept Failed. %s \n", strerror(errno));
            return 1;
        }

        // buff to accept the msg: todo del
        u_int32_t msg_len;
        char buff[1028];
        ssize_t bytes_received = 0;

//        //read the msg length: todo uncomment
//        while (bytes_received < sizeof(msg_len)){
//            bytes_received = read(connfd, &msg_len, sizeof(msg_len));
//        }

        //read the msg:
        bytes_received = 0;
        while (bytes_received < 28){
            bytes_received = read(connfd, buff, 28);
        }
        buff[bytes_received] = '\0';

        //print msg: todo del
        printf("%s\n", buff);

        // close socket
        close(connfd);
    }
}

