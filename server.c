//    int flock(int fd, int operation);
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/file.h>
#include"./entities/user.h"
#include"./entities/admin.h"
#include"./entities/book.h"
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <pthread.h>
#include"./entities/borrow.h"
#include"./entities/user.h"
#include"./entities/admin.h"
#include"./entities/book.h"
#include"communication.h"
#include <sys/select.h>

void * handleconn(void *comminfo){
    printf("we just accepted a connection!!!");
    struct comminfo *inf = (struct comminfo*) comminfo;
    int newfd = inf->newfd;
    printf("newfd is %d\n", newfd);
    struct message buf;
    // fd_set read_fds;
    // FD_ZERO(&read_fds);
    // FD_SET(newfd, &read_fds);
    
    inituser();
    startuser();
    while(1){
        // printf("calling select\n");
        // int ret = select(newfd+1, &read_fds, NULL, NULL, NULL);
        // printf("select returned %d", ret);
        read(newfd, &buf, sizeof(struct message));
        printf("received from client:  \n");
        printf("client is in state: %d", buf.state.val);

        int optodo = buf.operation;
        switch(optodo){
            case findbook:
            break;
            case listbooks:
            break;
            case addbook:
            break;
            case deletebook:
            break;
            case modifybook:
            break;
            case borrowbook:
            break;
            case returnbook:
            break;
            case loginuser:
                printf("user is trying to login!\n");
                int status;
                printf("started validation\n");
                validateUser(buf.dataptr.loginuser.username, buf.dataptr.loginuser.password, &status);
                printf("validation done\n");
                struct reply reply;
                if(status == USER_NOT_FOUND){
                    strcpy(reply.text, "no such user!, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(reply.newstate.who.user, buf.state.who.user);
                }
                else if(status == INCORRECT_PASSWORD){
                    strcpy(reply.text, "wrong password, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(reply.newstate.who.user, buf.state.who.user);
                }
                else if(status == 0){
                    strcpy(reply.text, "login success\n");
                    reply.newstate.val = user_logged_in;
                    int offset;
                    buf.state.who.user = *finduser(buf.dataptr.loginuser.username, &offset);
                    structusercpy(reply.newstate.who.user, buf.state.who.user);
                }
                else{
                    printf("unexpected error\n");
                }
                printf("going to write the reply\n");
                write(newfd, &reply, sizeof(struct reply));
                printf("replied!!\n");
                
            break;
            case loginadmin:
            break;
            case registeruser:
            break;
            case registeradmin:
            break;
            case deleteadmin:
            break;
            case deleteuser:
            break;
            default:
            printf("no such operation\n");
            break;
        }
    }
    
    enduser();
}


int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct comminfo *comminfo1;
    if(sockfd == -1){
        printf("socket call failed\n");
        exit(0);
    }
    struct sockaddr_in clientaddr, server;
    int newfd;
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int ret = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    if(ret < 0){
        printf("bind failed\n");
        exit(0);
    }

    listen(sockfd, 1);
    printf("listening on port %d\n", PORT);
    int addrlen = sizeof(clientaddr);
    while(1){
        printf("in while loop\n");
        int newfd = accept(sockfd, (struct sockaddr*)&clientaddr, (socklen_t*)&addrlen);
        if(newfd == -1){
            printf("connection failed\n");
        }
        else{
            if(fork() == 0){//child
                comminfo1 = malloc(sizeof(struct comminfo));
                comminfo1->newfd = newfd;
                pthread_create(&comminfo1->tid, NULL, handleconn, comminfo1);
            }
            else {
                continue;
            }
            
        }
    }

    
    return 0;
}



// int ret = open(usrfile, O_CREAT|O_WRONLY, 0777);
//     if(ret != -1){
//         printf("what is going on?\n");
//         getchar();
//     }
//     else{
//         printf("nothing is wrong I'm left\n");
//         getchar();
//     }