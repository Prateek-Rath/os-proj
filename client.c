//    int flock(int fd, int operation);
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/file.h>
#include"./entities/user.h"
#include"./entities/admin.h"
#include"./entities/book.h"
#include"./entities/borrow.h"
#include <sys/types.h>          
#include <sys/socket.h>
#include<unistd.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include"communication.h"





//we need a format to send and receive messages





//the message is going to be an array of strings
//the first word is going to be one of these:
//-loginuser //-registeruser //-loginadmin //-registeradmin
//-findbook //-borrowbook //-returnbook //-listbooks
 //-addbook //-deletebook //-modifybook
//we don't allow purchases












void showopts(){
    // system("/usr/bin/clear");
    printf("1 to login as user\n2 to register a user\n3 to login as admin\n4 to register an admin\n100 to go back\n");
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


void assigndefault(void * ptr){
    memset(ptr, -1, sizeof(*ptr));
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
            showopts();
            printf("Enter option: ");
            scanf("%d", &opt);
            printf("opt is %d\n", opt);
            if(opt == 1){
                //loginUser
                char username[20];
                char password[20];
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);
                printf("here1\n");
                state.val = user_login_process;
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
                printf("%s", response.text);
                structstatecpy(state,response.newstate);
                
            }
            else if(opt == 2){
                //registerUser
                state.val = user_register_process;
                // registerUser();
            }
            else if(opt == 3){
                //loginAdmin
                state.val = admin_login_process;
                // loginAdmin();
            }
            else if(opt == 4){
                //reg admin
                state.val = admin_register_process;
            }
        }
        else if(state.val == user_login_process){
            // enum states newstate = loginUser();
            struct message request;

        }
        else if(state.val == admin_login_process){
            // enum states newstate = loginAdmin();
        }
        else if(state.val == user_register_process){
            // enum states newstate = registerUser();
        }
        else if(state.val == admin_register_process){
            // enum states newstate = registerAdmin();
        }
        else if(state.val == user_logged_in){
            // enum states newstate = userWork();
        }
        else if(state.val == admin_logged_in){
            // enum states newstate = adminWork();
        }
    }
    
    
}