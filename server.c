//    int flock(int fd, int operation);
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/file.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <pthread.h>
#include"communication.h"
// #ifndef ALL
// #define ALL
#include"./entities/user.h"
#include"./entities/admin.h"
#include"./entities/book.h"
#include"./entities/borrow.h"
// #endif

#include <sys/select.h>
 #include <signal.h>
 #include <errno.h>

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
    initadmin();
    startadmin();
    initbook();
    startbook();
    initborrow();
    startborrow();
    while(1){
        // printf("calling select\n");
        // int ret = select(newfd+1, &read_fds, NULL, NULL, NULL);
        // printf("select returned %d", ret);
        read(newfd, &buf, sizeof(struct message));
        printf("received from client:  \n");
        printf("client is in state: %d\n", buf.state.val);

        int optodo = buf.operation;
        switch(optodo){
            case findbook:
                struct reply reply;
                int offset;
                struct book * b1 = findBook(buf.dataptr.findbook.title, &offset);
                if(b1 == NULL){
                    printf("book was not found\n");
                }
                else{
                    strcpy(reply.text, "here is the book\n");
                    strcat(reply.text, getBook(*b1));
                }
                write(newfd, &reply, sizeof(struct reply));
                break;
            case listbooks:
                // reply;
                strcpy(reply.text, getallBooks());
                structstatecpy(&reply.newstate, &buf.state);
                write(newfd, &reply, sizeof(struct reply));
                break;
            case addbook:
                // struct reply reply;
                int status;
                createBook(buf.dataptr.addbook.book, &status);
                if(status == DUPLICATE_BOOK){
                    strcpy(reply.text, "that already exists!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else if(status == 0){
                    strcpy(reply.text, "successfully created book!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else{
                    printf("some unknown error");
                    printf("status is %d\n", status);
                }
                write(newfd, &reply, sizeof(struct reply));
            break;  
            case deletebook:
                
                deleteBook(buf.dataptr.deletebook.title, &status);
                if(status == 0){
                    sprintf(reply.text , "deleted book successfully\n");
                    
                }
                else if(status == NOT_YET_RETURNED){
                    sprintf(reply.text , "could not delete as someone has borrowed this book\n");
                }
                else {
                    printf("some other error\n");
                    exit(0);
                }
                write(newfd, &reply, sizeof(struct reply));
                break;

            case modifybook:
                
                updateBook(buf.dataptr.modifybook.oldtitle, buf.dataptr.modifybook.book, &status);
                if(status == DUPLICATE_BOOK){
                    strcpy(reply.text, "that already exists!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else if(status == 0){
                    strcpy(reply.text, "successfully created book!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else{
                    printf("some unknown error");
                    printf("status is %d\n", status);
                }
                write(newfd, &reply, sizeof(struct reply));
                break;

            case borrowbook:
                
                borrowBook(buf.dataptr.borrowbook.borrow, &status);
                if(status == 0){
                    char * str = malloc(50);
                    sprintf(str, "successful borrow\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == BOOK_NOT_FOUND){
                    char * str = malloc(50);
                    sprintf(str, "no such book!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == NOT_ENOUGH_COPIES){
                    char * str = malloc(50);
                    sprintf(str, "no copies of that book are available!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == DUPLICATE_BORROW){
                    char * str = malloc(50);
                    sprintf(str, "you already have that book!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else{
                    printf("some other errror\n");
                    exit(0);
                }
                write(newfd, &reply, sizeof(struct reply));
                break;
            case returnbook:
                
                returnBook(buf.dataptr.returnbook.borrow.title, buf.dataptr.returnbook.borrow.username, &status);
                if(status == 0){
                    char * str = malloc(50);
                    sprintf(str, "successful return\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    write(newfd, &reply, sizeof(struct reply));
                }
                else if(status == BOOK_NOT_FOUND){
                    char * str = malloc(50);
                    sprintf(str, "no such book!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    write(newfd, &reply, sizeof(struct reply));
                }
                else if(status == USER_NOT_FOUND){
                    char * str = malloc(50);
                    sprintf(str, "no such user!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    write(newfd, &reply, sizeof(struct reply));
                }
                else if(status == NOT_ENOUGH_COPIES){
                    char * str = malloc(50);
                    sprintf(str, "you don't have copies of that book\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    write(newfd, &reply, sizeof(struct reply));
                }
                break;
            case listusers:
                
                break;

            case loginuser:
                printf("user is trying to login!\n");
                // 
                printf("started validation\n");
                validateUser(buf.dataptr.loginuser.username, buf.dataptr.loginuser.password, &status);
                printf("validation done\n");
                // struct reply reply;
                if(status == USER_NOT_FOUND){
                    strcpy(reply.text, "no such user!, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else if(status == INCORRECT_PASSWORD){
                    strcpy(reply.text, "wrong password, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else if(status == 0){
                    strcpy(reply.text, "login success\n");
                    reply.newstate.val = user_logged_in;
                    int offset;
                    buf.state.who.user = *finduser(buf.dataptr.loginuser.username, &offset);
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else{
                    printf("unexpected error\n");
                }
                printf("going to write the reply\n");
                printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("replied!!\n");
                
                break;

            case loginadmin:
                printf("admin is trying to login!\n");
                printf("started validation\n");
                validateAdmin(buf.dataptr.loginadmin.username, buf.dataptr.loginadmin.password, &status);
                printf("validation done\n");
                // reply;
                if(status == ADMIN_NOT_FOUND){
                    strcpy(reply.text, "no such user!, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else if(status == INCORRECT_PASSWORD){
                    strcpy(reply.text, "wrong password, try again!");
                    reply.newstate.val = user_login_process;
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else if(status == 0){
                    strcpy(reply.text, "login success\n");
                    reply.newstate.val = admin_logged_in;
                    int offset;
                    buf.state.who.admin = *findadmin(buf.dataptr.loginuser.username, &offset);
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else{
                    printf("unexpected error\n");
                }
                printf("going to write the reply\n");
                printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("replied!!\n");
                break;
            case registeruser:
                printf("user is trying to register!\n");
                status;
                printf("started registration\n");
                showuser(buf.dataptr.registeruser.user);
                createUser(buf.dataptr.registeruser.user, &status);
                printf("register call done\n");
                // struct reply reply;
                if(status == DUPLICATE_USER){
                    strcpy(reply.text, "duplicate user!, try again!");
                    reply.newstate.val = user_register_process;
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else if(status == 0){
                    strcpy(reply.text, "register success!!\n");
                    reply.newstate.val = start;
                    int offset;
                    struct user * tempo = finduser(buf.dataptr.registeruser.user.username, &offset);
                    structusercpy(&buf.state.who.user, tempo);
                    printf("after reg we check if the user is there and we get\n");
                    showuser(buf.state.who.user);
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else{
                    printf("unexpected error\n");
                }
                printf("going to write the reply\n");
                printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("replied!!\n");
                break;

            case registeradmin:
                printf("admin is trying to register!\n");
                status;
                printf("started registration\n");
                showadmin(buf.dataptr.registeradmin.admin);
                createAdmin(buf.dataptr.registeradmin.admin, &status);
                printf("register call done\n");
                // struct reply reply;
                if(status == DUPLICATE_ADMIN){
                    printf("here1\n");
                    strcpy(reply.text, "duplicate admin!, try again!");
                    reply.newstate.val = admin_register_process;
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else if(status == 0){
                    printf("here2\n");
                    strcpy(reply.text, "register success!!\n");
                    printf("strcpy done\n");
                    reply.newstate.val = start;
                    int offset;
                    struct admin * tempo = findadmin(buf.dataptr.registeradmin.admin.username, &offset);
                    if(tempo == NULL){
                        printf("critical error this can't be true\n");
                    }
                    printf("findadmin done\n");
                    structadmincpy(&buf.state.who.admin, tempo);
                    printf("after reg we check if the user is there and we get\n");
                    showadmin(buf.state.who.admin);
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else{
                    printf("unexpected error\n");
                }
                printf("going to write the reply\n");
                printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("replied!!\n");
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

int sockfd;

void signal_callback_handler(){
    printf("caught something\n");
    close(sockfd);
    close(userglob.fd);
    close(adminglob.fd);
    close(bookglob.fd);
    close(borrowglob.fd);
    exit(0);
}   



int main(){
    signal(SIGSTOP, signal_callback_handler);
    signal(SIGINT, signal_callback_handler);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
        printf("error: %s\n", strerror(errno) );
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
            comminfo1 = malloc(sizeof(struct comminfo));
            comminfo1->newfd = newfd;
            pthread_create(&comminfo1->tid, NULL, handleconn, comminfo1);
            // if(fork() == 0){//child
            //     comminfo1 = malloc(sizeof(struct comminfo));
            //     comminfo1->newfd = newfd;
            //     pthread_create(&comminfo1->tid, NULL, handleconn, comminfo1);
            // }
            // else {
            //     continue;
            // }
            
        }
    }

    
    return 0;
}



