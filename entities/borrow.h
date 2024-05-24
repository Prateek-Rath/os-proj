#ifndef BORROW
#define BORROW

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>
#include <sys/file.h>
#include"book.h"
#include<semaphore.h>

#define DUPLICATE_BORROW 2
#define BORROW_NOT_FOUND 3
#define NOT_ENOUGH_COPIES 4
#define BOOK_ALREADY_TAKEN 5


#define borrfile "./files/borrow.dat"


struct borrow{
    int id;
    char username[20];
    char phone[20];
    char title[100];
};

struct borrowglobals{
    int cur_borr_id;
    int count;
    int fd;
};

extern struct borrowglobals borrowglob;
extern sem_t borrowsem;


bool borrowequals(struct borrow u1, struct borrow u2);

void initborrow();

void startborrow();

bool existsBorrow(struct borrow u1);

void borrowBook(struct borrow u1, int *status);

struct borrow * findborrow(char * title, char * username, int * offset);

void returnBook(char * title, char * username, int * status);

void showborrow(struct borrow u1);
    
void showAllBorrows();

char * getallBorrows();

void endborrow();

char * getBorrowList_user(char * username);

bool isBorrowed(struct book b1);//tells if someone has taken this book

#endif
