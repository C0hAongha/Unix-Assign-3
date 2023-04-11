//
// Created by Colm Ó hAonghusa  on 06/04/2023.
// Student number: 21405114 email: colm.ohaonghusa@ucdconnect.ie
//

#include "SocketIO.h"

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

    int errnum;
    if ((errnum = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: err num %d: %s\n", errnum, gai_strerror(errnum));
        exit(-1);
    }

    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);

        if (sfd == -1)
            continue;   /* On error, try next address */

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; /* Success */

        /* close() failed, close this socket, try next address */
        close(sfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "No socket to bind...\n");
        exit(-1);
    }

    freeaddrinfo(result);

    //quiz start...

    char* buf = malloc(BUFSIZE * sizeof(char*));
    socketRead(sfd, buf);
    printf("%s\n>", buf);

    char* line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    linelen = getline(&line, &linecap, stdin);
    //check for trailing '/n'
    if (line[linelen - 1] == '\n') line[linelen - 1] = '\0';

    socketWrite(sfd, line);

    if (strcasecmp(line, "q") == 0)
        goto close_sfd;

    for (int i = 0; i < 5; i++) {
        socketRead(sfd, buf);
        printf("%s\n>", buf);

        line = NULL;
        linecap = 0;
        linelen = getline(&line, &linecap, stdin);
        //check for trailing '/n'
        if (line[linelen - 1] == '\n') line[linelen - 1] = '\0';

        socketWrite(sfd, line);

        //get answer
        socketRead(sfd, buf);
        printf("\n%s\n\n", buf);
    }

    //get final score
    socketRead(sfd, buf);
    printf("\n%s\n", buf);

    //quiz end...

    close_sfd:
    if (close(sfd) == -1) /* Close connection */
    {
        char* errstr = "close error";
        perror(errstr);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}