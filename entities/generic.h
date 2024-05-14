#ifndef GENERIC
#define GENERIC

#include<semaphore.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include"user.h"
#include"book.h"
#include"borrow.h"
#include"admin.h"

#define DUPLICATE_USER 2
#define USER_NOT_FOUND 2

char * usrfile = "../files/user.dat";

enum types {
    user, book, admin, borrow
} type1;


struct semaphores{
    sem_t semusr;
    sem_t semadm;
    sem_t sembk;
    sem_t semborr;
}sems;

void initSemaphores();

void initentity(enum types type);

void startentity(enum types type);

void createentity(enum types type);






#endif