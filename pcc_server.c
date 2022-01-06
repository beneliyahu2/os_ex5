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
#include <signal.h>

//todos:
// todo 1. uncomment initialize of data structure >> DONE <<
// todo 2. check each char received - if it's printable(32<=ch<=126): >> DONE <<
//         add +1 to its place in the data structure and add +1 to a counter (to return to the client) >> DONE <<
// todo 3. send the client the counter of printable chars >> DONE <<
// todo 5. use the SO_REUSEADDR socket option >>DONE<<

// todo 4. handle SIGINT (in an atomic way)
// todo 6. handle TCP errors (as described in the form)

// status vars:
int busy_with_client = 0;
int sigint_thrown = 0;
int chars_stat[126]; // data structure to collect statistics from all clients:


void *safe_malloc(size_t size){
    void *ptr = malloc(size);
    if (!ptr && (size > 0)) {
        fprintf(stderr,"malloc failed. %s\n", strerror(errno));
        exit(1);
    }
    return ptr;
}

void print_stat_and_exit(){
    for (int i = 32; i<126 ; i++){
        printf("char '%c' : %u times\n", i, chars_stat[i]);
    }
    exit(0);
}

void sigint_handler(){
    if (busy_with_client){
        sigint_thrown = 1;
    }
    else{
        print_stat_and_exit();
    }
}

int check_for_errors(ssize_t ret_val, char *action_str, int connfd){
    if (ret_val > 0){
        return 0;
    }
    else if(ret_val == 0 || errno == ETIMEDOUT || errno == ECONNRESET || errno == EPIPE){
        if (ret_val == 0){ // Client process killed unexpectedly
            fprintf(stderr, "Client process killed unexpectedly while %s. %s. Continue serving other clients.\n", action_str, strerror(errno));
        }
        else{ // TCP error
            fprintf(stderr, "Error %s due to TCP error. %s. Will continue serving other clients\n", action_str, strerror(errno));
        }
        close(connfd);
        busy_with_client = 0;
        return 1;
    }
    else{
        fprintf(stderr, "Error %s. %s\n", action_str, strerror(errno));
        exit(1);
    }
}

int main(int argc, char *argv[]){

    // initiate sigint_handler
    struct sigaction sigint;
    sigint.sa_handler = &sigint_handler;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sigint, 0) != 0) {
        fprintf(stderr, "Initiate the signal handler failed. %s\n", strerror(errno));
        exit(1);
    }

    // declaring variables for returned values:
    int listenfd  = -1;
    int connfd    = -1;

    // initializing  the statistics data structure:
    memset(chars_stat, 0, 126*sizeof(int));

    // parse command line arguments:
    if (argc != 2){
        fprintf(stderr, "wrong number of arguments.\n");
        exit(1);
    }
    u_int16_t port_num = strtol(argv[1], NULL, 10);

    // create a socket to listen to:
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if( listenfd < 0){
        fprintf(stderr,"Could not create socket. %s\n", strerror(errno));
        exit(1);
    }
    int enable = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
        fprintf(stderr,"Setsockopt failed. %s\n", strerror(errno));
        exit(1);
    }

    //create a struct for the binding:
    struct sockaddr_in serv_addr;
    socklen_t addrsize = sizeof(struct sockaddr_in );
    memset( &serv_addr, 0, addrsize ); //starting with clean (zero values)
    serv_addr.sin_family = AF_INET; //
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = any local machine address
    serv_addr.sin_port = htons(port_num);

    // bind the socket to the ip address:
    if( 0 != bind(listenfd, (struct sockaddr*) &serv_addr, addrsize)){
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
        busy_with_client = 0;

        // Accept a connection:
        connfd = accept( listenfd, NULL, NULL);
        if( connfd < 0 ){
            fprintf(stderr, "Accept Failed. %s \n", strerror(errno));
            exit(1);
        }
        busy_with_client = 1;

        // vars and buffer to accept n and the msg:
        u_int32_t num;
        char *len_buff = (char*)&num;
        ssize_t total_received;
        ssize_t received_bytes;

        //receive the msg length (n):
        total_received = 0;
        char *curr_loc_in_len_buff = len_buff;
        while (total_received < sizeof(u_int32_t)){
            received_bytes = read(connfd, curr_loc_in_len_buff, sizeof(u_int32_t)-total_received);
            // error handling:
            char *action_str = "receiving the byte stream length from client";
            if (check_for_errors(received_bytes, action_str, connfd)){
                break;
            }
            curr_loc_in_len_buff += received_bytes;
            total_received += received_bytes;
        }
        // in case the error checker detected an error:
        if (! busy_with_client){
            continue;
        }
        u_int32_t n = (u_int32_t)ntohl(num);

        printf("\n N from the client is: %d\n", n); //todo del

        //receive the msg:
        char *msg = (char*)safe_malloc(1028 * sizeof(char));
        total_received = 0;
        char *curr_loc_in_buff = msg;
        while (total_received < n){
            received_bytes = read(connfd, curr_loc_in_buff, n);
            // error handling:
            char *action_str = "receiving the byte stream from client";
            if (check_for_errors(received_bytes, action_str, connfd)){
                break;
            }
            curr_loc_in_buff += received_bytes;
            total_received += received_bytes;
        }
        msg[n] = '\0';
        // in case the error checker detected an error:
        if (! busy_with_client){
            free(msg);
            continue;
        }

        printf("\n msg from the client: '%s'\n", msg); //todo del

        // count printable chars:
        int printable_chars = 0;
        char ch;
        for(int i = 0; i<n ; i++ ){
            ch = msg[i];
            if (ch >= 32 && ch <= 126){
                printable_chars++;
                chars_stat[(int)ch]++;
            }
        }

        // free msg buffer:
        free(msg);

        // converting to network convention:
        u_int32_t pch = htonl(printable_chars);
        // temp vars:
        ssize_t total_sent;
        ssize_t sent_bytes;

        //sending the number of printable chars to the client:
        total_sent = 0;
        char *str_pch = (char*)&pch;
        while(total_sent < sizeof(u_int32_t)){
            sent_bytes = write(connfd, str_pch, sizeof(u_int32_t)-total_sent);
            // error handling:
            char *action_str = "sending the number of printable chars to the client";
            if (check_for_errors(received_bytes, action_str, connfd)){
                break;
            }
            str_pch += sent_bytes; //pointer to the rest of the bytes to send
            total_sent += sent_bytes;
        }
        // in case the error checker detected an error:
        if (! busy_with_client){
            continue;
        }

        // close socket
        close(connfd);

        // if sigint was thrown and the server was busy, now it will be treated:
        if (sigint_thrown){
            print_stat_and_exit();
        }
    }
}

