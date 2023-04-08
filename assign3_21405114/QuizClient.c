//
// Created by Colm Ó hAonghusa  on 06/04/2023.
// Student number: 21405114 email: colm.ohaonghusa@ucdconnect.ie
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define BUFSIZE 500

int main(int argc, char* argv[]) {
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP address of server> <port number>.\n",
                argv[0]);
        exit(-1);
    }

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0)
        exit(-1);

    int cfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);

        if (cfd == -1)
            continue;   /* On error, try next address */

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; /* Success */

        /* close() failed, close this socket, try next address */
        close(cfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "No socket to bind...\n");
        exit(-1);
    }

    freeaddrinfo(result);

    //quiz start...

    char buf[BUFSIZE];
    size_t totRead;
    char* bufr = buf;
    for (totRead = 0; totRead < BUFSIZE; ) {
        ssize_t numRead = read(cfd, bufr, BUFSIZE - totRead);
        if (numRead == 0)
            break;
        if (numRead == -1) {
            if (errno == EINTR)
                continue;
            else {
                fprintf(stderr, "Read error, Errno %d.\n", errno);
            }
        }
        totRead += numRead;
        bufr += numRead;
    }
    printf("Received %s\n", buf);

    char* line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    linelen = getline(&line, &linecap, stdin);
    //check for trailing '/n'
    if (line[linelen - 1] == '\n') line[linelen - 1] = '\0';

    size_t totWritten;
    const char* bufw = line;
    for (totWritten = 0; totWritten < BUFSIZE; ) {
        ssize_t numWritten = write(cfd, bufw, BUFSIZE - totWritten);
        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;
            else {
                fprintf(stderr, "Write error. Errno %d.\n", errno);
            }
        }
        totWritten += numWritten;
        bufw += numWritten;
    }


    //quiz end...

    if (close(cfd) == -1) /* Close connection */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}