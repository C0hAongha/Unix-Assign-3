//
// Created by Colm Ó hAonghusa  on 08/04/2023.
//

#include "SocketIO.h"

void socketRead(int sfd, char* retBuf) {
    char buf[BUFSIZE];
    memset(buf, '\0', BUFSIZE);
    size_t totRead;
    char* bufr = buf;
    for (totRead = 0; totRead < BUFSIZE; ) {
        ssize_t numRead = read(sfd, bufr, BUFSIZE - totRead);
        if (numRead == 0)
            break;
        if (numRead == -1) {
            if (errno == EINTR)
                continue;
            else {
                char* errstr = "read error";
                perror(errstr);
                exit(-1);
            }
        }
        totRead += numRead;
        bufr += numRead;
    }
    char buf2[BUFSIZE];
    strncpy(retBuf, buf, BUFSIZE * sizeof(char*));
}

void socketWrite(int sfd, const char* input) {
    size_t totWritten = 0;
    const char* bufw = input;
    while (totWritten < BUFSIZE) {
        ssize_t numWritten = write(sfd, bufw, BUFSIZE - totWritten);
        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;
            else {
                char* errstr = "write error";
                perror(errstr);
                exit(-1);
            }
        }
        totWritten += numWritten;
        bufw += numWritten;
    }
}
