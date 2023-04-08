cmake_minimum_required(VERSION 3.22)
project(assign3_21405114 C)

set(CMAKE_C_STANDARD 17)

add_executable(assign3_21405114_server QuizServer.c QuizDB.h SocketIO.c SocketIO.h)
add_executable(assign3_21405114_client QuizClient.c QuizDB.h SocketIO.c SocketIO.h)
