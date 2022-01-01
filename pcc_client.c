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

//todos:
//todo - 1. receive the number of printable chars from server and print it (in the format specified in the form)
//todo - 2. check the error handling is as required (to stderr and with errno)


void *safe_malloc(size_t size){
    void *ptr = malloc(size);
    if (!ptr && (size > 0)) {
        fprintf(stderr,"malloc failed. %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}


int main(int argc, char *argv[]){
    // declaring variables for returned values:
    int sockfd;
    size_t ret_val;

    // parse command line arguments:
    if (argc != 4){
        fprintf(stderr, "Wrong number of arguments.\n");
        exit(1);
    }
    char *ip_addr = argv[1];
    u_int16_t port_num = strtol(argv[2], NULL, 10);
    char *file_path = argv[3];

    // create a socket:
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr,"Could not create socket. \n");
        exit(1);
    }

    //create a struct for connecting:
    struct sockaddr_in serv_addr;
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

    //read massage from file:
    FILE *f = fopen(file_path, "r");
    if(!f){
        fprintf(stderr, "failed to open the file. %s\n", strerror(errno));
        exit(1);
    }
    int cnt = 0;
    while(fgetc(f) != EOF){
        cnt++;
    }
    char *msg = (char*)safe_malloc(sizeof(char)*cnt);
    rewind(f);
    ret_val = fread(msg, 1, cnt, f);
    if (ret_val < cnt){
        fprintf(stderr, "error in reading file. %s\n", strerror(errno));
        exit(1);
    }
    fclose(f);

    // msg length:
    u_int32_t msg_len = htonl((u_int32_t)strlen(msg));
    // temp vars:
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

    // closing the socket:
    close(sockfd);

    exit(0);

}


