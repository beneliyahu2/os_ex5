cmake_minimum_required(VERSION 3.20)
project(os_ex5 C)

set(CMAKE_C_STANDARD 11)

add_executable(os_ex5 pcc_server.c pcc_client.c)
