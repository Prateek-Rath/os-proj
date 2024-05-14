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
#ifndef ALL
#define ALL
#include"borrow.h"
#include"user.h"
#include"book.h"
#endif

struct borrowglobals borrowglob;
// extern struct userglobals userglob;
// extern struct adminglobals adminglob;
// extern struct bookglobals bookglob;


bool borrowequals(struct borrow u1, struct borrow u2){
    if((strcmp(u1.title, u2.title) == 0) && (strcmp(u1.username, u2.username) == 0)){
        return true;
    }
    else {
        // printf("not equal\n");
        // printf("1: %s, 2: %s", u1.title, u2.title);
        return false;
    }
}

void initborrow(){ //to be called only once
    borrowglob.fd  = open(borrfile, O_CREAT|O_WRONLY, 0777);
    if(borrowglob.fd == -1){
        printf("open failed\n");
        exit(0);
    }
    borrowglob.cur_borr_id = 0;
    borrowglob.count = 0;
    flock(borrowglob.fd, LOCK_EX); printf("ex locked borrowfile\n");
    write(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    write(borrowglob.fd, &borrowglob.count, sizeof(int));
    flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
    close(borrowglob.fd);
    borrowglob.fd = -1;
    printf("initborrow done\n");
    
    sync();
}


void startborrow(){
    //open required files
    borrowglob.fd = open(borrfile, O_RDWR, 0777);
    userglob.fd = open(usrfile, O_RDWR, 0777);
    bookglob.fd = open(bkfile, O_RDWR, 0777);
    flock(borrowglob.fd, LOCK_SH); printf("share locked borrowfile\n");
    int ret  = read(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    if(ret == -1){
        printf("error in read\n");
    }
    read(borrowglob.fd, &borrowglob.count, sizeof(int));
    
    // printf("cur_borrow_id is %d and count is %d\n", borrowglob.cur_borr_id, borrowglob.count);
    
    if(borrowglob.fd == -1){
        printf("error\n");
        exit(0);
    }
    lseek(borrowglob.fd, 0, SEEK_SET);
    printf("startborrow done\n");
    sync();
    flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
    return ;

}




bool existsBorrow(struct borrow u1){
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    lseek(borrowglob.fd, 0, SEEK_SET);
    read(borrowglob.fd, &(borrowglob.cur_borr_id), sizeof(int));
    read(borrowglob.fd, &(borrowglob.count), sizeof(int));
    if(borrowglob.count == 0){
        flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
        return false;
    }
    else{
        struct borrow temp;
        for(int i=0; i<borrowglob.count; i++){
            read(borrowglob.fd, &temp, sizeof(temp));
            if(borrowequals(u1 , temp)){
                sync();
                flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
                return true;
            }
        }
        flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
        return false;
    }
    flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");

}

void borrowBook(struct borrow u1, int *status){
    printf("in borrow u1.title passed is %s\n", u1.title);
    printf("in borrow u1.username passed is %s\n", u1.username);
    int bkoffset, usroffset, offset;
    
    struct book * bptr = findBook(u1.title, &bkoffset);
    struct user * uptr = finduser(u1.username, &usroffset);

    if(bptr == NULL){
        printf("critical error, book doesn't exist!!\n");
        *status =  BOOK_NOT_FOUND;
        flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
        return;
    }
    else if(uptr  == NULL){
        printf("critical error, user doesn't exist!!\n");
        *status =  BOOK_NOT_FOUND;
        flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
        return;
    }
    printf("we found the book and the user\n");
    struct book b1 = *bptr;
    //open the file 
    //ensure id and title are unique
    //insert at the end of the file
    flock(borrowglob.fd, LOCK_EX); printf("ex locked borrowfile\n");
    lseek(borrowglob.fd, 0, SEEK_SET);
    if(existsBorrow(u1)){
        *status = DUPLICATE_BORROW;//duplicate borrow
        flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
        return;
    }
    lseek(borrowglob.fd, 0, SEEK_SET);
    read(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    read(borrowglob.fd, &borrowglob.count, sizeof(int));
    
    borrowglob.count++;
    borrowglob.cur_borr_id++;
    u1.id = borrowglob.cur_borr_id;

    lseek(borrowglob.fd, 0, SEEK_SET);
    write(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    write(borrowglob.fd, &borrowglob.count, sizeof(int));

    // printf("wrote %d\n", borrowglob.cur_borr_id);
    // printf("wrote count: %d\n", borrowglob.count);
    flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");
    

    //we also need to go to the book file and update there
    //we have the offset
    flock(bookglob.fd, LOCK_EX); printf("ex locked bookfile\n");
    lseek(bookglob.fd, bkoffset, SEEK_SET);
    if(b1.copies_left > 0){
        b1.copies_left -=1;
        write(bookglob.fd, &b1, sizeof(struct book));
        *status = 0; //success
        lseek(borrowglob.fd, 0, SEEK_END);
        printf("u1.title is now %s++\n", u1.title);
        write(borrowglob.fd, &u1, sizeof(u1));
    }
    else{
        printf("b1.copies is %d\n", b1.copies_left);
        printf("b1.title is %s\n", b1.title);
        *status = NOT_ENOUGH_COPIES;
    }
    

    sync();
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    if(uptr != NULL) free(uptr);
    if(bptr != NULL) free(bptr);
}




struct borrow * findborrow(char * title, char * username, int * offset){
    int count;
    *offset = 0;
    flock(borrowglob.fd, LOCK_SH); printf("share locked borrowfile\n");
    lseek(borrowglob.fd, 0, SEEK_SET);
    read(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    read(borrowglob.fd, &borrowglob.count, sizeof(int));
    //count is the number of records
    if(count == 0){
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return NULL;
    }
    else{
        *offset = lseek(borrowglob.fd, 0, SEEK_CUR);
        struct borrow *temp = (struct borrow *)malloc(sizeof(struct borrow));
        for(int i=0; i<count; i++){
            read(borrowglob.fd, temp, sizeof(struct borrow));
            if((strcmp(temp->title, title) == 0) && (strcmp(temp->username, username) == 0)){
                flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
                return temp;
            }
            else{
                *offset = lseek(borrowglob.fd, 0, SEEK_CUR);
                // printf("title1: %s", temp->title);
                // printf(" title2: %s\n", title);
            }
        }
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");    
        return NULL;
    }
    sync();
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    
}


void returnBook(char * title, char * username, int * status){
    
    int bkoffset;
    struct book * bptr = findBook(title, &bkoffset);
    struct book b1 = *bptr;
    if(bptr == NULL){
        printf("critical error, book doesn't exist!!\n");
        *status =  BOOK_NOT_FOUND;
        return;
    }
    
    
    int offset;
    struct borrow * u1 = findborrow(title, username, &offset);
    if(u1 == NULL){
        *status = BORROW_NOT_FOUND;
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return;
    }

    flock(borrowglob.fd, LOCK_EX); printf("ex locked borrowfile\n");
    lseek(borrowglob.fd, 0, SEEK_SET);
    read(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
    read(borrowglob.fd, &borrowglob.count, sizeof(int));


    struct borrow lastborrow;
    lseek(borrowglob.fd, -sizeof(struct borrow), SEEK_END);
    read(borrowglob.fd, &lastborrow, sizeof(struct borrow));

    lseek(borrowglob.fd, offset, SEEK_SET);
    write(borrowglob.fd, &lastborrow, sizeof(struct borrow));
    borrowglob.count--;
    lseek(borrowglob.fd, sizeof(int), SEEK_SET);
    write(borrowglob.fd, &borrowglob.count, sizeof(int));
    flock(borrowglob.fd, LOCK_UN); printf("unlocked borrowfile\n");

    //we also need to go to the book file and update there
    //we have the offset
    flock(bookglob.fd, LOCK_EX); printf("ex locked bookfile\n");
    lseek(bookglob.fd, bkoffset, SEEK_SET);
    b1.copies_left +=1;
    write(bookglob.fd, &b1, sizeof(struct book));
    *status = 0; //success
    sync();
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    if(bptr != NULL) free(bptr);
    if(u1 != NULL) free(u1);
}




void showborrow(struct borrow u1){
    if(u1.id == -1) return;
    printf("id: %d\n", u1.id);
    printf("title: %s\n", u1.title);
    printf("username: %s\n", u1.username);
    printf("phone: %s\n", u1.phone);
    printf("---------------------------\n");
}
    
void showAllBorrows(){
    int val;
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    lseek(borrowglob.fd, 0, SEEK_SET);
    read(borrowglob.fd, &val, sizeof(int));
    read(borrowglob.fd, &val, sizeof(int));
    struct borrow temp;
    if(val == 0){
        printf("no borrows right right now!!\n");
        printf("---------------------------\n");
    }
    for(int i=0; i<val; i++){
        read(borrowglob.fd, &temp, sizeof(temp));
        printf("temp.title is %s--\n", temp.title);
        showborrow(temp);
    }
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
}

void endborrow(){
    close(borrowglob.fd);
    borrowglob.fd = -1;
    close(bookglob.fd);
    bookglob.fd = -1;
    close(userglob.fd);
    userglob.fd = -1;
    printf("endborrow done\n");
}