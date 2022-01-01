//
// Created by Abigail Ben Eliyahu on Dec 2021.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char *argv[]){
    // declaring variables for returned values:
    int sockfd = -1;

    // parse command line arguments: todo uncomment
//    if (argc != 3){
//        fprintf(stderr, "wrong number of arguments.\n");
//        exit(1);
//    }
//    char *ip_add = argv[1];
//    u_int16_t port = strtol(argv[2], NULL, 10);
//    char *file_path = argv[3];

    // create a socket:
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    //create a struct for connecting:
    struct sockaddr_in serv_addr; // where we Want to get to
    //socklen_t addrsize = sizeof(struct sockaddr_in ); todo check why need that

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10000); // Note: htons for endiannes
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // the local host

    // connect socket to the server:
    if( connect(sockfd, (struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
        printf("\n Error : Connect Failed. %s \n", strerror(errno));
        return 1;
    }

    //msg: - temp todo del
    char *msg = "the massage from the client.";
    u_int32_t msg_len = htons(strlen(msg));
    ssize_t n_written = 0;

    //sending the length of the msg: todo uncomment
//    while( n_written <  sizeof(msg_len)){
//        n_written = write(sockfd, msg, 28);
//    }
//    n_written = 0;

    // sending the message:
    while( n_written <  msg_len){
       n_written = write(sockfd, msg, 28);
    }

    // closing the socket:
    close(sockfd);

    return 0; //todo - need this?

}


