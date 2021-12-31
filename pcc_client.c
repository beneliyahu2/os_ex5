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

    if (argc != 3){
        fprintf(stderr, "wrong number of arguments.\n");
        exit(1);
    }
    char *ip_add = argv[1];
    ushort port = strtol(argv[2], NULL, NULL);
    char *file_path = argv[3];



}


