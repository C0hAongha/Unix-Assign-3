//
// Created by Colm Ó hAonghusa  on 08/04/2023.
//

#ifndef ASSIGN3_21405114_SOCKETIO_H
#define ASSIGN3_21405114_SOCKETIO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define BUFSIZE 500

void socketRead(int sfd, char* retBuf);
void socketWrite(int sfd, const char* input);

#endif //ASSIGN3_21405114_SOCKETIO_H
