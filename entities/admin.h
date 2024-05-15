#ifndef ADMIN
#define ADMIN

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdbool.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/file.h>
#include<semaphore.h>



#define DUPLICATE_ADMIN 2
#define ADMIN_NOT_FOUND 3
#define INCORRECT_PASSWORD 4

#define admfile "./files/admin.dat"


struct adminglobals{
    int cur_adm_id;
    int count;
    int fd;
} ;

extern struct adminglobals adminglob;
extern sem_t adminsem;

struct admin{
    int id; //primary key //the id is something that I decide // and I auto increment it....
    char username[20]; // must be unique
    char first_name[30];
    char last_name[30];
    char password[40];
    char phone[20];//check if it is 10 digits if possible
};


void structadmincpy(struct admin *a1, struct admin *a2);

bool adminequals(struct admin u1, struct admin u2);

void initadmin();

void startadmin();

bool existsAdmin(struct admin u1);

void createAdmin(struct admin u1, int *status);

struct admin * findadmin(char * username, int * offset);

void validateAdmin(char * username, char * password, int * status);

void updateAdmin(char *username, struct admin newadmin, int * status);


void deleteAdmin(char * username, int * status);

void showadmin(struct admin u1);
    
void showAllAdmins();

void endadmin();

#endif