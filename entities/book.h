#ifndef BOOK
#define BOOK

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
#include<semaphore.h>


#define DUPLICATE_BOOK 2
#define BOOK_NOT_FOUND 3
#define NOT_YET_RETURNED 13

#define bkfile "./files/book.dat"

struct bookglobals{
    int cur_bk_id;
    int count;
    int fd;
};

extern struct bookglobals bookglob;
extern sem_t booksem;

struct book{//we assume one book has only one author
    int id;
    char title[100];
    char author[100];
    char category[40];
    int copies_left;
};

bool bookequals(struct book u1, struct book u2);

void initbook();

void startbook();

bool existsBook(struct book u1);

char * getallBooks();

char * getBook(struct book b1);

void createBook(struct book u1, int *status);

struct book * findBook(char * title, int * offset);

void updateBook(char *title, struct book newbook, int * status);

void deleteBook(char * title, int * status);

void showbook(struct book u1);
    
void showAllBooks();

void endbook();

#endif