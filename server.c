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
    

    while(1){
        // printf("calling select\n");
        // int ret = select(newfd+1, &read_fds, NULL, NULL, NULL);
        // printf("select returned %d", ret);
        read(newfd, &buf, sizeof(struct message));

        int optodo = buf.operation;
        switch(optodo){
            // printf("opttodo is %d\n");
            case donothing:
                printf("someone asked to do nothing\n");
                close(newfd);
                pthread_exit(NULL);
                break;
            case findbook:
                printf("someone asked to findbook\n");
                sem_wait(&booksem);
                struct reply reply;
                int offset;
                struct book * b1 = findBook(buf.dataptr.findbook.title, &offset);
                if(b1 == NULL){
                    printf("book was not found\n");
                    strcpy(reply.text, "that book is not there\n");
                }
                else{
                    printf("book found!\n");
                    strcpy(reply.text, "here is the book\n");
                    strcat(reply.text, getBook(*b1));
                }
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&booksem);
                break;
                
            case listbooks:
                printf("someone asked to list all the books\n");
                sem_wait(&booksem);
                // printf("get all books will be called\n");
                char * x = getallBooks();
                printf("x is %s\n", x);
                strcpy(reply.text, "");
                strcpy(reply.text, x);
                // printf("get all books done\n");
                structstatecpy(&reply.newstate, &buf.state);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                // memset(x, 0, 100);
                free(x);
                sem_post(&booksem);
                break;
            case addbook:
                // struct reply reply;
                printf("someone asked to add a new book\n");
                sem_wait(&booksem);
                int status;
                createBook(buf.dataptr.addbook.book, &status);
                if(status == DUPLICATE_BOOK){
                    strcpy(reply.text, "that already exists!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else if(status == 0){
                    strcpy(reply.text, "successfully added book!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else{
                    printf("some unknown error");
                    printf("status is %d\n", status);
                }
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&booksem);
                break;  
            case deletebook:
                printf("someone asked to delete a book\n");
                sem_wait(&booksem);
                deleteBook(buf.dataptr.deletebook.title, &status);
                if(status == 0){
                    sprintf(reply.text , "deleted book successfully\n");
                }
                else if(status == NOT_YET_RETURNED){
                    sprintf(reply.text , "could not delete as someone has borrowed this book\n");
                }
                else if(status == BOOK_NOT_FOUND){
                    sprintf(reply.text , "no such book exists\n");
                }
                else{
                    printf("some other error\n");
                    exit(0);
                }
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&booksem);
                break;

            case modifybook:
                printf("someone asked to modify a book\n");
                sem_wait(&booksem);
                updateBook(buf.dataptr.modifybook.oldtitle, buf.dataptr.modifybook.book, &status);
                if(status == DUPLICATE_BOOK){
                    strcpy(reply.text, "that already exists!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else if(status == 0){
                    strcpy(reply.text, "successfully modified book!!");
                    structstatecpy(&reply.newstate, &buf.state);
                }
                else{
                    printf("some unknown error");
                    printf("status is %d\n", status);
                }
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&booksem);
                break;

            case borrowbook:
                printf("someone asked to borrow a book\n");
                sem_wait(&usersem);
                sem_wait(&booksem);
                sem_wait(&borrowsem);
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
                printf("reply sent\n");
                sem_post(&usersem);
                sem_post(&booksem);
                sem_post(&borrowsem);
                break;
            case returnbook:
                printf("someone wanted to return a book");
                sem_wait(&usersem);
                sem_wait(&booksem);
                sem_wait(&borrowsem);
                returnBook(buf.dataptr.returnbook.title, buf.dataptr.returnbook.username, &status);
                if(status == 0){
                    char * str = malloc(50);
                    sprintf(str, "successful return\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == BOOK_NOT_FOUND){
                    char * str = malloc(50);
                    sprintf(str, "you don't have any such book!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == USER_NOT_FOUND){
                    char * str = malloc(50);
                    sprintf(str, "no such user!!\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                else if(status == NOT_ENOUGH_COPIES){
                    char * str = malloc(50);
                    sprintf(str, "you don't have any such book\n");
                    strcpy(reply.text, str);
                    structstatecpy(&reply.newstate, &buf.state);
                    
                }
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&usersem);
                sem_post(&booksem);
                sem_post(&borrowsem);
                break;
            case listusers:
                printf("someone wanted the list of all users\n");
                sem_wait(&usersem);
                x = listUsers();
                strcpy(reply.text, x);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                free(x);
                sem_post(&usersem);
                break;

            case loginuser:
                printf("some user wanted to login\n");
                sem_post(&usersem);
                // printf("user is trying to login!\n");
                // 
                // printf("started validation\n");
                validateUser(buf.dataptr.loginuser.username, buf.dataptr.loginuser.password, &status);
                // printf("validation done\n");
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
                printf("reply sent\n");
                sem_post(&usersem);
                break;

            case loginadmin:
                printf("some admin wanted to login!\n");
                sem_post(&adminsem);
                
                // printf("started validation\n");
                validateAdmin(buf.dataptr.loginadmin.username, buf.dataptr.loginadmin.password, &status);
                // printf("validation done\n");
                // reply;
                if(status == ADMIN_NOT_FOUND){
                    strcpy(reply.text, "no such user!, try again!");
                    reply.newstate.val = admin_login_process;
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else if(status == INCORRECT_PASSWORD){
                    strcpy(reply.text, "wrong password, try again!");
                    reply.newstate.val = admin_login_process;
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else if(status == 0){
                    strcpy(reply.text, "login success\n");
                    reply.newstate.val = admin_logged_in;
                    int offset;
                    buf.state.who.admin = *findadmin(buf.dataptr.loginadmin.username, &offset);
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else{
                    printf("unexpected error\n");
                }
                // printf("going to write the reply\n");
                // printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&adminsem);
                break;
            case registeruser:
                printf("user is trying to register!\n");
                sem_wait(&usersem);
            
                status;
                // printf("started registration\n");
                showuser(buf.dataptr.registeruser.user);
                createUser(buf.dataptr.registeruser.user, &status);
                // printf("register call done\n");
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
                    // printf("after reg we check if the user is there and we get\n");
                    // showuser(buf.state.who.user);
                    structusercpy(&reply.newstate.who.user, &buf.state.who.user);
                }
                else{
                    printf("unexpected error\n");
                }
                // printf("going to write the reply\n");
                // printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent!!\n");
                sem_post(&usersem);
                break;

            case registeradmin:
                printf("admin is trying to register!\n");
                sem_wait(&adminsem);
                // printf("started registration\n");
                showadmin(buf.dataptr.registeradmin.admin);
                createAdmin(buf.dataptr.registeradmin.admin, &status);
                // printf("register call done\n");
                // struct reply reply;
                if(status == DUPLICATE_ADMIN){
                    strcpy(reply.text, "duplicate admin!, try again!");
                    reply.newstate.val = admin_register_process;
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else if(status == 0){
                    strcpy(reply.text, "register success!!\n");
                    // printf("strcpy done\n");
                    reply.newstate.val = start;
                    int offset;
                    struct admin * tempo = findadmin(buf.dataptr.registeradmin.admin.username, &offset);
                    if(tempo == NULL){
                        printf("critical error this can't be true\n");
                    }
                    structadmincpy(&buf.state.who.admin, tempo);
                    // printf("after reg we check if the user is there and we get\n");
                    // showadmin(buf.state.who.admin);
                    structadmincpy(&reply.newstate.who.admin, &buf.state.who.admin);
                }
                else{
                    printf("unexpected error\n");
                }
                // printf("going to write the reply\n");
                // printf("replytext is %s\n", reply.text);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                sem_post(&adminsem);
                break;
            case getuserbooklist:
                // char * x = getBorrowList_user(username);
                // strcpy(response.text, x);
                // request.dataptr.userbooks.username
                printf("some user wants to know which books he borrowed\n");
                sem_wait(&usersem);
                sem_wait(&booksem);
                sem_wait(&borrowsem);
                char * result = getBorrowList_user(buf.dataptr.userbooks.username);
                strcpy(reply.text, result);
                structstatecpy(&reply.newstate, &buf.state);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                free(result);
                sem_post(&usersem);
                sem_post(&booksem);
                sem_post(&borrowsem);
                break;
            
            case listborrows:
                printf("someone wants to see all current borrows\n");
                sem_wait(&borrowsem);
                result = getallBorrows();
                strcpy(reply.text, result);
                structstatecpy(&reply.newstate, &buf.state);
                write(newfd, &reply, sizeof(struct reply));
                printf("reply sent\n");
                free(result);
                sem_post(&borrowsem);
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
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_addr.s_addr = inet_addr("192.168.1.207");

    server.sin_port = htons(PORT);

    int ret = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    
    if(ret < 0){
        printf("bind failed\n");
        printf("error: %s\n", strerror(errno));
        exit(0);
    }

    listen(sockfd, 1);
    printf("listening on port %d\n", PORT);
    int addrlen = sizeof(clientaddr);
    //  inituser();
    startuser();
    //  initadmin();
    startadmin();
    //  initbook();
    startbook();
    //  initborrow();
    startborrow();
    while(1){
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



