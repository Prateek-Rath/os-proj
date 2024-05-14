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

struct bookglobals bookglob;
bool bookequals(struct book u1, struct book u2){
    if(strcmp(u1.title, u2.title) == 0){
        return true;
    }
    else {
        // printf("not equal\n");
        // printf("1: %s, 2: %s", u1.title, u2.title);
        return false;
    }
}

void initbook(){ //to be called only once
    bookglob.fd  = open(bkfile, O_CREAT|O_WRONLY, 0777);
    if(bookglob.fd == -1){
        printf("open failed\n");
        exit(0);
    }
    bookglob.cur_bk_id = 0;
    bookglob.count = 0;
    flock(bookglob.fd, LOCK_EX); printf("exclusive locked bookfile\n");
    lseek(bookglob.fd, 0, SEEK_SET);
    write(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
    write(bookglob.fd, &bookglob.count, sizeof(int));
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    close(bookglob.fd);
    bookglob.fd = -1;
    printf("initbook done\n");
    
    sync();
}


void startbook(){
    //open required files
    bookglob.fd = open(bkfile, O_RDWR, 0777);
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    int ret = read(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
    read(bookglob.fd, &bookglob.count, sizeof(int));
    
    printf("cur_book_id is %d and count is %d\n", bookglob.cur_bk_id, bookglob.count);
    
    lseek(bookglob.fd, 0, SEEK_SET);
    if(bookglob.fd == -1){
        printf("error\n");
        exit(0);
    }
    printf("startbook done\n");
    sync();
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    return ;

}




bool existsBook(struct book u1){
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    lseek(bookglob.fd, 0, SEEK_SET);
    read(bookglob.fd, &(bookglob.cur_bk_id), sizeof(int));
    read(bookglob.fd, &(bookglob.count), sizeof(int));
    if(bookglob.count == 0){
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return false;
    }
    else{
        struct book temp;
        for(int i=0; i<bookglob.count; i++){
            read(bookglob.fd, &temp, sizeof(temp));
            if(bookequals(u1 , temp)){
                sync();
                flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
                return true;
            }
        }
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return false;
    }

}

void createBook(struct book u1, int *status){
    //open the file 
    //ensure id and title are unique
    //insert at the end of the file
    flock(bookglob.fd, LOCK_EX); printf("exclusive locked bookfile\n");
    lseek(bookglob.fd, 0, SEEK_SET);
    if(existsBook(u1)){
        *status = DUPLICATE_BOOK;//duplicate book
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return;
    }
    lseek(bookglob.fd, 0, SEEK_SET);
    read(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
    read(bookglob.fd, &bookglob.count, sizeof(int));
    
    bookglob.count++;
    bookglob.cur_bk_id++;
    u1.id = bookglob.cur_bk_id;

    lseek(bookglob.fd, 0, SEEK_SET);
    write(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
    write(bookglob.fd, &bookglob.count, sizeof(int));

    // printf("wrote %d\n", bookglob.cur_bk_id);
    // printf("wrote count: %d\n", bookglob.count);

    lseek(bookglob.fd, 0, SEEK_END);
    write(bookglob.fd, &u1, sizeof(u1));

    *status = 0; //success
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    sync();
}




struct book * findBook(char * title, int * offset){
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    *offset = 0;
    lseek(bookglob.fd, 0, SEEK_SET);
    int ret = read(bookglob.fd, &(bookglob.cur_bk_id), sizeof(int));
    // printf("%d bytes were read\n", ret);
    ret = read(bookglob.fd, &(bookglob.count), sizeof(int));
    // printf("%d bytes were read\n", ret);
    lseek(bookglob.fd, 2*sizeof(int), SEEK_SET);
    //count is the number of records
    if(bookglob.count == 0){
        printf("count is 0!!!!!!!\n");
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return NULL;
    }
    else{
        *offset = lseek(bookglob.fd, 0, SEEK_CUR);
        struct book *temp = (struct book *)malloc(sizeof(struct book));
        // printf("loop start\n");
        for(int i=0; i<bookglob.count; i++){
            // printf("loop %d\n+++++", i );
            read(bookglob.fd, temp, sizeof(struct book));
            if(strcmp(temp->title, title) == 0){
                sync();
                flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
                return temp;
            }
            else{
                *offset = lseek(bookglob.fd, 0, SEEK_CUR);
                printf("title1: %s", temp->title);
                printf(" title2: %s\n", title);
            }
        }
        // printf("loop end\n");
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
        return NULL;
    }
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
}


void updateBook(char *title, struct book newbook, int * status){
    //first check if the book actually exists
    
    int offset;
    struct book * u1 = findBook(title, &offset); // it will be better if it returns the desired offset in the file also
    if(u1 == NULL){
        *status = BOOK_NOT_FOUND;
        // printf("BOOK NOT FOUND====================================================\n");
        // printf("title is %s\n", title);
        return;
    }
    else{
        // u1->author = newbook.author;
        // u1->category = newbook.category;
        flock(bookglob.fd, LOCK_EX); printf("exclusive locked bookfile\n");
        newbook.id = u1->id;
        lseek(bookglob.fd, offset, SEEK_SET);
        struct book oldbook;
        read(bookglob.fd, &oldbook, sizeof(struct book));
        lseek(bookglob.fd, offset, SEEK_SET);
        if(existsBook(newbook) && strcmp(newbook.title, oldbook.title)){
            // printf("t1: %s, t2: %s\n", oldbook.title, newbook.title);
            printf("there is already a book with that title\n");
            *status = DUPLICATE_BOOK;
            flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
            return;
        }
        else{
            lseek(bookglob.fd, offset, SEEK_SET);
            write(bookglob.fd, &newbook, sizeof(struct book));
            *status = 0;
            sync();
            flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
            return;
        }
        flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");   
    }
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    if(u1 != NULL)
        free(u1);
}


void deleteBook(char * title, int * status){
    int offset;
    printf("title is %s\n", title);
    struct book * u1 = findBook(title, &offset);
    if(u1 == NULL){
        *status = BOOK_NOT_FOUND;
        return;
    }
    // struct book delbook = *u1;
    // delbook.id = -1;
    // lseek(bookglob.fd, offset, SEEK_SET);
    // write(bookglob.fd, &delbook, sizeof(struct book));
    // *status = 0;
    // return;
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    lseek(bookglob.fd, 0, SEEK_SET);
    read(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
    read(bookglob.fd, &bookglob.count, sizeof(int));


    struct book lastbook;
    lseek(bookglob.fd, -sizeof(struct book), SEEK_END);
    read(bookglob.fd, &lastbook, sizeof(struct book));
    lseek(bookglob.fd, offset, SEEK_SET);
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    
    flock(bookglob.fd, LOCK_EX); printf("exclusive locked bookfile\n");
    write(bookglob.fd, &lastbook, sizeof(struct book));
    printf("bookglob.count is %d\n", bookglob.count);
    bookglob.count--;
    lseek(bookglob.fd, sizeof(int), SEEK_SET);
    write(bookglob.fd, &bookglob.count, sizeof(int));
    int pos = lseek(bookglob.fd, 0, SEEK_END);
    ftruncate(bookglob.fd, pos-sizeof(struct book));
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
    *status = 0;
    sync();
    if(u1 != NULL) free(u1);
}




void showbook(struct book u1){
    if(u1.id == -1) return;
    printf("id: %d\n", u1.id);
    printf("title: %s\n", u1.title);
    printf("fname: %s\n", u1.author);
    printf("lname: %s\n", u1.category);
    printf("copies_left: %d\n", u1.copies_left);
    

    printf("---------------------------\n");
}
    
void showAllBooks(){
    int val;
    flock(bookglob.fd, LOCK_SH); printf("share locked bookfile\n");
    lseek(bookglob.fd, 0, SEEK_SET);
    read(bookglob.fd, &val, sizeof(int));
    read(bookglob.fd, &val, sizeof(int));
    struct book temp;
    if(val == 0){
        printf("no books right right now!!\n");
        printf("---------------------------\n");
    }
    for(int i=0; i<val; i++){
        read(bookglob.fd, &temp, sizeof(temp));
        showbook(temp);
    }
    flock(bookglob.fd, LOCK_UN); printf("unlocked bookfile\n");
}

void endbook(){
    close(bookglob.fd);
    bookglob.fd = -1;
    printf("endbook done\n");
}

