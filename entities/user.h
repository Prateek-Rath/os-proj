#ifndef USER
#define USER

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdbool.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include<semaphore.h>



#define DUPLICATE_USER 2
#define USER_NOT_FOUND 3
#define INCORRECT_PASSWORD 4


#define usrfile "./files/user.dat"

struct userglobals{
    int cur_usr_id;
    int count;
    int fd;
} ;

extern struct userglobals userglob;
extern sem_t usersem;

struct user{
    int id; //primary key //the id is something that I decide // and I auto increment it....
    char username[20]; // must be unique
    char first_name[30];
    char last_name[30];
    char password[40];
    char phone[20];//check if it is 10 digits if possible
};

char * listUsers();

void structusercpy(struct user *u1, struct user *u2);

bool userequals(struct user u1, struct user u2);

void inituser();

void startuser();

bool existsUser(struct user u1);

void createUser(struct user u1, int *status);

struct user * finduser(char * username, int * offset);

void validateUser(char * username, char * password, int * status);

void updateUser(char *username, struct user newuser, int * status);

void deleteUser(char * username, int * status);

void showuser(struct user u1);
    
void showAllUsers();

void enduser();

#endif