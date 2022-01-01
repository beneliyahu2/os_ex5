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

    // parse command line arguments:
    if (argc != 4){
        fprintf(stderr, "Wrong number of arguments.\n");
        exit(1);
    }
    char *ip_addr = argv[1];
    u_int16_t port_num = strtol(argv[2], NULL, 10);
//    char *file_path = argv[3]; todo uncomment

    // create a socket:
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr,"Could not create socket. \n");
        exit(1);
    }

    //create a struct for connecting:
    struct sockaddr_in serv_addr; // where we Want to get to

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num); // htons for endiannes
    if(inet_pton(AF_INET, ip_addr, &(serv_addr.sin_addr)) < 0){ //"127.0.0.1" the local host
        fprintf(stderr, "error converting ip address to binary. %s\n", strerror(errno));
        exit(1);
    }

    // connect socket to the server:
    if( connect(sockfd, (struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
        fprintf(stderr, "Connect Failed. %s \n", strerror(errno));
        exit(1);
    }

    //msg: - temp todo del
    char *msg = "the massage from the client.";
    u_int32_t msg_len = htonl((u_int32_t)strlen(msg));
    printf("\nn before conversion is : %u", (u_int32_t)strlen(msg)); //todo del
    printf("\nn in network rep: %u\n", msg_len); //todo del
    ssize_t total_sent;
    ssize_t sent_bytes;


    //sending the length of the msg:
    total_sent = 0;
    char *str_n = (char*)&msg_len;
    while(total_sent < sizeof(u_int32_t)){
        sent_bytes = write(sockfd, str_n, sizeof(u_int32_t)-total_sent);
        str_n += sent_bytes; //pointer to the rest of the bytes to send
        total_sent += sent_bytes;
    }

    // sending the message:
    total_sent = 0;
    while(total_sent < strlen(msg)){
        sent_bytes = write(sockfd, msg, strlen(msg));
        msg += sent_bytes; //pointer to the rest of the bytes to send
        total_sent += sent_bytes;
    }
    printf("\nsent msg\n"); //todo del

    // closing the socket:
    close(sockfd);

    return 0; //todo - need this?

}


