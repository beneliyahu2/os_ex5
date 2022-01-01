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

int send_int(int num, int fd){
    int32_t conv = htonl(num);
    char *data = (char*)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(fd, data, left);
        if (rc < 0) {
            return -1;
        }
        data += rc;
        left -= rc;
    }
    while (left > 0);
    return 0;
}

int receive_int(int *num, int fd){
    u_int32_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = read(fd, data, left);
        if (rc <= 0) { /* instead of ret */
            return -1;
        }
        data += rc;
        left -= rc;
    }
    while (left > 0);
    *num = ntohl(ret);
    return 0;
}


int main(int argc, char *argv[]){

    // declaring variables for returned values:
    int listenfd  = -1;
    int connfd    = -1;

    // initalizing data structure to collect statistics from all clients:
//    int chars_stat[256]; todo uncomment

    // parse command line arguments:
    if (argc != 2){
        fprintf(stderr, "wrong number of arguments.\n");
        exit(1);
    }
    u_int16_t port_num = strtol(argv[1], NULL, 10);

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
    serv_addr.sin_port = htons(port_num);

    // bind the socket to the ip address:
    if( 0 != bind( listenfd, (struct sockaddr*) &serv_addr, addrsize ) )
    {
        fprintf(stderr, "Bind Failed. %s \n", strerror(errno));
        exit(1);
    }

    // start listening:
    if( 0 != listen( listenfd, 10)){//maximum 10 pending clients in the queue
        fprintf(stderr,"Listen Failed. %s \n", strerror(errno));
        exit(1);
    }

    // serving clients from the queue:
    while(1) {
        // Accept a connection.
        connfd = accept( listenfd, NULL, NULL); //todo change name to "curr_client_sd"
        if( connfd < 0 ){
            fprintf(stderr, "Accept Failed. %s \n", strerror(errno));
            exit(1);
        }

        // buff to accept the msg: todo del
//        char msg_len_str[sizeof(u_int32_t)]; todo uncomment
        char *buff = (char*)malloc(1028 * sizeof(char));
        ssize_t total_received = 0;
        ssize_t received_bytes = 0;

//        //read the msg length: todo uncomment
//        while (received_bytes < sizeof(u_int32_t)){
//            received_bytes = read(connfd, msg_len_str, sizeof(u_int32_t));
//        }
//        u_int32_t msg_len = ntohl((u_int32_t)msg_len_str);

        //read the msg:
        total_received = 0;
        char *curr_loc_in_buff = buff;
        while (total_received < 28){
            received_bytes = read(connfd, curr_loc_in_buff, 28);
            printf("received %d bytes\n", received_bytes); //todo del
            curr_loc_in_buff += received_bytes;
            total_received += received_bytes;
        }
        buff[28] = '\0';

        printf("\nreceived msg\n"); //todo del

        //print msg: todo del
        printf("the msg: '%s'\n", buff);

        // close socket
        close(connfd);
    }
}

