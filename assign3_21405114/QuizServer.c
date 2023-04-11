//
// Created by Colm Ó hAonghusa  on 06/04/2023.
// Student number: 21405114 email: colm.ohaonghusa@ucdconnect.ie
//

#include <time.h>
#include "SocketIO.h"
#include "QuizDB.h"

#define BACKLOG 10

int userStartQuiz(int cfd);

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

    int errnum;
    if ((errnum = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: err num %d: %s\n", errnum, gai_strerror(errnum));
        exit(-1);
    }

    int lfd, optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);

        if (lfd == -1)
            continue;   /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval)) == -1) {
            char* errstr = "setsockopt error";
            perror(errstr);
            exit(-1);
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        /* bind() failed, close this socket, try next address */
        close(lfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "No socket to bind...\n");
        exit(-1);
    }

    {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        errnum = getnameinfo((struct sockaddr *) rp->ai_addr, rp->ai_addrlen,
                             host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (errnum == 0)
            fprintf(stdout, "Listening on (%s, %s)\n", host, service);
        else {
            fprintf(stderr, "getnameinfo error: %s", gai_strerror(errnum));
            exit(-1);
        }
    }

    freeaddrinfo(result);

    if (listen(lfd, BACKLOG) == -1) {
        char* errstr = "listen error";
        perror(errstr);
        exit(-1);
    }

    fprintf(stdout, "<waiting for clients to connect>\n");
    fprintf(stdout, "<ctrl-C to terminate>\n\n");

    //implement catch of ctrl+c so that program can try to exit itself
    while (1) {
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

        int userPlay = userStartQuiz(cfd);

        switch (userPlay) {
            case 1:
                //play quiz
                printf("play\n");
                break;
            case 0:
                //goto close and accept next connection
                printf("close\n");
                goto close_cfd;
            default:
                fprintf(stderr, "Invalid user input\n");
                goto close_cfd;
        }

        unsigned int qIndex[5];
        srandom(time(NULL));
        for (int i = 0; i < 5; i++) {
            qIndex[i] = random() % 43;
            for (int j = 0; j < i; j++) {
                if (i == j) {
                    qIndex[i] = random() % 43;
                    j = 0;
                }
            }
        }

        int score = 0;
        for (int i = 0; i < 5; i++) {
            socketWrite(cfd, QuizQ[qIndex[i]]);
            char* answer = malloc(BUFSIZE * sizeof(char*));
            socketRead(cfd, answer);

            if (strcmp(answer, QuizA[qIndex[i]]) == 0) {
                socketWrite(cfd, "Right Answer.");
                score++;
            } else {
                char* sol = malloc(BUFSIZE * sizeof(char*));
                snprintf(sol, BUFSIZE, "Wrong Answer. Right answer is %s.", QuizA[qIndex[i]]);
                socketWrite(cfd, sol);
            }
        }

        char* quizResult = malloc(BUFSIZE * sizeof(char*));
        snprintf(quizResult, BUFSIZE, "Your quiz score is %d/5. Goodbye.", score);

        socketWrite(cfd, quizResult);

        //quiz ends here...

        close_cfd:
        if (close(cfd) == -1) /* Close connection */
        {
            fprintf(stderr, "close error.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int userStartQuiz(int cfd) {
    char* intro = "Welcome to Unix Programming Quiz!\n"
                  "The quiz comprises five questions posed to you one after the other.\n"
                  "You have only one attempt to answer a question.\n"
                  "Your final score will be sent to you after conclusion of the quiz.\n"
                  "To start the quiz, press Y and <enter>.\n"
                  "To quit the quiz, press q and <enter>.\n";

    socketWrite(cfd, intro);

    char* buf = malloc(BUFSIZE * sizeof(char*));
    socketRead(cfd, buf);

    printf("%s\n", buf);

    if (strcasecmp(buf, "Y") == 0)
        return 1;
    else if (strcasecmp(buf, "q") == 0)
        return 0;
    else
        return -1;
}
