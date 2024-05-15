//    int flock(int fd, int operation);
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/file.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include<unistd.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include"communication.h"
#ifndef ALL
#define ALL
#include"./entities/user.h"
#include"./entities/book.h"
#include"./entities/admin.h"
#include"./entities/borrow.h"
#endif


//we need a format to send and receive messages
struct state state;




//the message is going to be an array of strings
//the first word is going to be one of these:
//-loginuser //-registeruser //-loginadmin //-registeradmin
//-findbook //-borrowbook //-returnbook //-listbooks
 //-addbook //-deletebook //-modifybook
//we don't allow purchases

void showopts(){
    // system("/usr/bin/clear");
    printf("1 to login as user\n2 to register a user\n3 to login as admin\n4 to register an admin\n");
}

void showUserMenu(){
    // system("/usr/bin/clear");
    printf("Hey user!\n");
    printf("-----------------------------------------------------\n");
    printf("Enter 1 to see list of books\n");
    printf("Enter 2 to find a book by it's title\n");
    printf("Enter 3 to borrow a book\n");
    printf("Enter 4 to see list of books that you have");
    printf("Enter 5 to return a book\n");
    printf("Enter 10 to logout\n");
    printf("-----------------------------------------------------\n");
    printf("Enter opt: ");
}

void showAdminMenu(){
    printf("Hey admin!\n");
    printf("-----------------------------------------------------\n");
    printf("Enter 1 to see list of books\n");
    printf("Enter 2 to find a book by it's title\n");
    printf("Enter 3 to add a book\n");
    printf("Enter 4 to modify/update a book\n");
    printf("Enter 5 to delete a book\n");
    printf("Enter 6 to list all users\n");
    printf("Enter 7 to see all borrow transactions\n");
    printf("Enter 10 to logout\n");
    printf("-----------------------------------------------------\n");
    printf("Enter opt: ");
}


void assigndefault(void * ptr){
    memset(ptr, -1, sizeof(*ptr));
}



bool contexec(){//continue execution
    printf("do you want to go continue? (press 'n' to go back and 'y' to continue)\n"); 
    char inp[10];
    scanf("%s", inp);
    if(inp[0] == 'y' || inp[0] == 'Y'){
        return true;
    }
    else{
        return false;
    }
}

int main(){
    //initialize state and stuff
    state.val = start;
    assigndefault(&state.who.user);
    
    

    int opt;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr, client;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    int servlen = sizeof(servaddr);
    int ret = connect(sockfd, (struct sockaddr*)&servaddr, (socklen_t)servlen);
    if(ret != 0 ){
        printf("connect failed!\n");
        exit(0);
    }
    printf("WELCOME TO OUR LIBRARY MANAGEMENT SYSTEM\n");
    while(1){
        if(state.val == start){
            printf("we are in the start state\n");
            showopts();
            printf("Enter option: ");
            scanf("%d", &opt);
            printf("opt is %d\n", opt);
            if(opt == 1){
                //loginUser
                state.val = user_login_process;
                continue;
            }
            else if(opt == 2){
                //registerUser
                state.val = user_register_process;
                continue;
            }
            else if(opt == 3){
                //loginAdmin
                state.val = admin_login_process;
                continue;
            }
            else if(opt == 4){
                //reg admin
                state.val = admin_register_process;
                continue;
            }
        }
        else if(state.val == user_login_process){
            // enum states newstate = loginUser();
            printf("we are in the user login process state\n");
            bool ret = contexec();
            if(ret == false){
                state.val = start;
                continue;
            }
            char username[20];
            char password[20];
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);
            printf("here1\n");
            struct message request;
            struct reply response;
            request.operation = loginuser;//what we want to do
            // request.state = state;
            request.state.who.user = state.who.user;
            request.state.val = state.val;
            request.dataptr;
            strcpy((request.dataptr).loginuser.username, username);
            strcpy((request.dataptr).loginuser.password, password);
            printf("about to send\n");
            // send(sockfd, &request, sizeof(struct message),0);
            write(sockfd, &request, sizeof(struct message));
            printf("request sent\n");
            // recv(sockfd, &response, sizeof(struct reply), 0);
            read(sockfd, &response, sizeof(struct reply));
            printf("server sent:\n");
            printf("%s\n", response.text);
            structstatecpy(&state,&response.newstate);
            continue;
        }
        else if(state.val == admin_login_process){
            printf("we are in the admin login process state\n");
            bool ret = contexec();
            if(ret == false){
                state.val = start;
                continue;
            }
            char username[20];
            char password[20];
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);
            printf("here1\n");
            struct message request;
            struct reply response;
            request.operation = loginadmin;//what we want to do
            // request.state = state;
            request.state.who.admin = state.who.admin;
            request.state.val = state.val;
            request.dataptr;
            strcpy((request.dataptr).loginadmin.username, username);
            strcpy((request.dataptr).loginadmin.password, password);
            printf("about to send\n");
            // send(sockfd, &request, sizeof(struct message),0);
            write(sockfd, &request, sizeof(struct message));
            printf("request sent\n");
            // recv(sockfd, &response, sizeof(struct reply), 0);
            read(sockfd, &response, sizeof(struct reply));
            printf("server sent:\n");
            printf("%s\n", response.text);
            structstatecpy(&state,&response.newstate);
            continue;
        }
        else if(state.val == user_register_process){
            printf("we are in the user register process\n");
            bool ret = contexec();
            if(ret == false){
                state.val = start;
                continue;
            }
            struct user u1;
            printf("Enter username: ");
            scanf("%s", u1.username);
            printf("Enter first_name: ");
            scanf("%s", u1.first_name);
            printf("Enter last_name: ");
            scanf("%s", u1.last_name);
            printf("Enter password: ");
            scanf("%s", u1.password);
            printf("Enter phone: ");
            scanf("%s", u1.phone);
            struct message request;
            struct reply response;
            request.operation = registeruser;//what we want to do
            structusercpy(&(request.state.who.user), &(state.who.user));
            request.state.val = state.val;

            printf("b4 copy user is :\n");
            showuser(u1); 
            structusercpy(&(request.dataptr.registeruser.user), &u1);
            printf("b4 send after copy\n accor to client user is\n");
            showuser(request.dataptr.registeruser.user);
            printf("about to send\n");
            // send(sockfd, &request, sizeof(struct message),0);
            write(sockfd, &request, sizeof(struct message));
            printf("request sent\n");
            // recv(sockfd, &response, sizeof(struct reply), 0);
            read(sockfd, &response, sizeof(struct reply));
            printf("server sent:\n");
            printf("%s\n", response.text);
            structstatecpy(&state,&response.newstate);
        }
        else if(state.val == admin_register_process){
            printf("we are in the admin register process\n");
            bool ret = contexec();
            if(ret == false){
                state.val = start;
                continue;
            }
            struct admin a1;
            printf("Enter username: ");
            scanf("%s", a1.username);
            printf("Enter first_name: ");
            scanf("%s", a1.first_name);
            printf("Enter last_name: ");
            scanf("%s", a1.last_name);
            printf("Enter password: ");
            scanf("%s", a1.password);
            printf("Enter phone: ");
            scanf("%s", a1.phone);
            struct message request;
            struct reply response;
            request.operation = registeradmin;//what we want to do
            structadmincpy(&(request.state.who.admin), &(state.who.admin));
            request.state.val = state.val;

            printf("b4 copy admin is :\n");
            showadmin(a1); 
            structadmincpy(&(request.dataptr.registeradmin.admin), &a1);
            printf("b4 send after copy\n accor to client user is\n");
            showadmin(request.dataptr.registeradmin.admin);
            printf("about to send\n");
            // send(sockfd, &request, sizeof(struct message),0);
            write(sockfd, &request, sizeof(struct message));
            printf("request sent\n");
            // recv(sockfd, &response, sizeof(struct reply), 0);
            read(sockfd, &response, sizeof(struct reply));
            printf("server sent:\n");
            printf("%s\n", response.text);
            structstatecpy(&state,&response.newstate);
        }
        else if(state.val == user_logged_in){
            printf("we are now in the user logged in state!!!\n");
            showUserMenu();
            scanf("%d", &opt);
            switch (opt)
            {
            case 1:
                printf("we want to list all the books\n");
                struct message request;
                struct reply response;
                request.operation = listbooks;
                request.state.val = state.val;
                write(sockfd, &request, sizeof(struct message));
                read(sockfd, &response, sizeof(struct reply));
                printf("%s", response.text);
                printf("---------------------\n");
                break;

            case 2:
                printf("we want to find book by title\n");
                // struct message request;
                // struct reply response;
                printf("Enter book title: ");
                char title[40];
                request.operation = findbook;
                scanf("%s", request.dataptr.findbook.title);
                request.state.val = state.val;
                
                write(sockfd, &request, sizeof(struct message));
                read(sockfd, &response, sizeof(struct reply));
                printf("%s", response.text);
                
                printf("---------------------\n");
                break;

            case 3:
                printf("we want to borrow a book\n");
                printf("Enter book title: ");
                scanf("%s", request.dataptr.borrowbook.borrow.title);
                printf("Enter another/your own phone number: ");
                scanf("%s", request.dataptr.borrowbook.borrow.phone);
                strcpy(request.dataptr.borrowbook.borrow.username, state.who.user.username);
                request.operation = borrowbook;
                strcpy(request.dataptr.borrowbook.borrow.username, state.who.user.phone);
                write(sockfd,&request, sizeof(struct message));
                printf("request sent to server\n");
                read(sockfd, &response, sizeof(struct reply));
                printf("server sent: \n");
                printf("%s", response.text);
                break;

            case 5:
                printf("we want to return a book\n");
                printf("enter book title: ");
                scanf("%s", request.dataptr.returnbook.title);
                request.operation = returnbook;
                strcpy(request.dataptr.returnbook.username, state.who.user.username);
                write(sockfd, &request, sizeof(struct message));
                printf("request sent to server\n");
                read(sockfd, &response, sizeof(struct reply));
                printf("server sent:\n");
                printf("%s", response.text);
                break;
            
            case 10:
                printf("logging out\n");
                char username[20];
                state.who.user.id = -1;
                strcpy(state.who.user.username, "nobody");
                strcpy(state.who.user.password, "whattodo");
                strcpy(state.who.user.first_name, "yash");
                strcpy(state.who.user.first_name, "ranjan");
                state.val = start;
                break;

            default:
                break;
            }

            // enum states newstate = userWork();
        }
        else if(state.val == admin_logged_in){
            printf("we are now in the admin logged in state!!!\n");
            showAdminMenu();
            scanf("%d", &opt);
            switch (opt){
                case 1:
                    printf("we want to list all the books\n");
                    struct message request;
                    struct reply response;
                    request.operation = listbooks;
                    request.state.val = state.val;
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct reply));
                    printf("%s", response.text);
                    printf("---------------------\n");
                    break;

                case 2:
                    printf("we want to find book by title\n");
                    // struct message request;
                    // struct reply response;
                    printf("Enter book title: ");
                    char title[40];
                    request.operation = findbook;
                    scanf("%s", request.dataptr.findbook.title);
                    request.state.val = state.val;
                    
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct reply));
                    printf("%s", response.text);
                    
                    printf("---------------------\n");
                    break;
                
                case 3:
                    printf("we want to add book\n");
                    // struct message request;
                    // struct reply response;
                    request.operation = addbook;
                    request.state.val = state.val;
                    printf("Enter title: ");
                    scanf("%s", request.dataptr.addbook.book.title);
                    printf("Enter author: ");
                    scanf("%s", request.dataptr.addbook.book.author);
                    printf("Enter category: ");
                    scanf("%s", request.dataptr.addbook.book.category);
                    printf("Enter number of copies available: ");
                    scanf("%d", &request.dataptr.addbook.book.copies_left);
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct reply));
                    printf("%s", response.text);
                    printf("---------------------\n");
                    break;
                case 4:
                    printf("we want to modify a book\n");
                    // struct message request;
                    // struct reply response;
                    request.operation = modifybook;
                    request.state.val = state.val;
                    printf("Enter title: ");
                    scanf("%s", request.dataptr.modifybook.oldtitle);
                    printf("Enter new title: ");
                    scanf("%s", request.dataptr.modifybook.book.title);
                    printf("Enter new author: ");
                    scanf("%s", request.dataptr.modifybook.book.author);
                    printf("Enter new category: ");
                    scanf("%s", request.dataptr.modifybook.book.category);
                    printf("Enter new number of copies available: ");
                    scanf("%d", &request.dataptr.modifybook.book.copies_left);
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct reply));
                    printf("%s", response.text);
                    printf("---------------------\n");
                    break;
                case 5:
                    printf("we want to delete a book\n");
                    //we can only delete if it has not been borrowed
                    request.operation = deletebook;
                    request.state.val = state.val;
                    printf("Enter title: ");
                    scanf("%s", request.dataptr.deletebook.title);
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct message));
                    printf("%s", response.text);
                    printf("---------------------\n");
                    break;

                case 6:
                    printf("we want to list all users\n");
                    request.operation = listusers;
                    write(sockfd, &request, sizeof(struct message));
                    read(sockfd, &response, sizeof(struct reply));
                    printf("server sent: \n");
                    printf("%s", response.text);
                
                case 10:
                    printf("logging out\n");
                    char username[20];
                    state.who.user.id = -1;
                    strcpy(state.who.user.username, "nobody");
                    strcpy(state.who.user.password, "whattodo");
                    strcpy(state.who.user.first_name, "yash");
                    strcpy(state.who.user.first_name, "ranjan");
                    state.val = start;
                    break;
            }
        }
        else{
            break;
        }
    }
    
    return 0;
}