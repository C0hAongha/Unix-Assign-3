#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include "QuizDB.h"

#define BACKLOG 10
#define BUFSIZE 32

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server IP address> <port number>.\n",
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
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if (getaddrinfo(argv[1], argv[1], &hints, &result) != 0)
        exit(-1);

    int lfd, optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);

        if (lfd == -1)
            continue;   /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) == -1)
            exit(-1);

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        /* bind() failed, close this socket, try next address */
        close(lfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "No socket to bind...\n");
        perror(NULL);
        exit(-1);
    }

    {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        int errnum;
        errnum = getnameinfo((struct sockaddr *) rp->ai_addr, rp->ai_addrlen,
                             host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (errnum == 0)
            fprintf(stdout, "Listening on (%s, %s)\n", host, service);
        else {
            fprintf(stderr, "getnameinfo error: %s", gai_strerror(errnum));
            exit(-1);
        }
    }

    //implement catch of ctrl+c so that program can try to exit itself
    while (1) {
        fprintf(stdout, "<waiting for clients to connect>\n");
        fprintf(stdout, "<ctrl-C to terminate>\n\n");

        struct sockaddr_storage claddr;
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        int cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
        if (cfd == -1) {
            char * errstr = "accept error";
            perror(errstr);
            continue;   /* Print an error message */
        }

        {
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                            host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
                fprintf(stdout, "Connection from (%s, %s)\n", host, service);
            else
                fprintf(stderr, "Connection from (?UNKNOWN?)");
        }

        //quiz begins here...

        //quiz ends here...

        if (close(cfd) == -1) /* Close connection */
        {
            fprintf(stderr, "close error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (close(lfd) == -1) /* Close listening socket */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
