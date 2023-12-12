/* This is the server part of the project
 * with server port 9877
 *
 * The starting point will be focused on
 * the center of the checkerboard
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <

// network necessary headers
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include "mydefine.h"

typedef struct playerInfo {
    // userID can be set to be the connfd
    int userID, win, lose, draw;
} p;

void handle_alarm(int signo) {}

// arg will be in type playerInfo (or p)
void * gameStart(void * arg) {
    pthread_detach(pthread_self());
    p thisPlayer = *((p *) arg);
    thisPlayer.win = thisPlayer.lose = thisPlayer.draw = 0;
    free(arg);

    while (1) {

    }
}

int main(int argc, char *argv[]) {
    int listenfd, connfd, listenFlags, nUsers = 0;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    listenFlags = fcntl(listen, F_GETFL);
    if ( fcntl(listenfd, F_GETFL, listenFlags | O_NONBLOCK) == -1 ) { 
        printf("fcntl error!\n");
        return 0;
    }

    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT); 
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    pthread_t *threads;
    signal(SIGALRM, handle_alarm);
    while (1) {
        clilen = sizeof(cliaddr);
        alarm(5);
        connfd = accept(listenfd, (struct sockaddr_in *) &cliaddr, &clilen);
        if ( connfd == -1 && errno == EWOULDBLOCK ) { continue; }
        else { 
            printf("Unexpected Error!\n");
            break; 
        }
        
        threads = (pthread_t *)malloc(sizeof(pthread_t));
        p *player = (p *)malloc(sizeof(p));
        pthread_create(threads, NULL, gameStart, (void *) player);
    }

    return 0;
}
