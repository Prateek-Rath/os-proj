#include "./book.h"
#include<stdio.h>


    
int main()
{
    printf("book one %d\n", getpid());
    initbook();
    startbook();
    flock(bookglob.fd, LOCK_UN);    

    int opt;
    while(1){
        //menu driven book crud
        
        printf("1. showAll, 2. insert, 3. update, 4. delete, 5. find_by_title, other to exit\n");
        printf("enter option: ");
        scanf("%d", &opt);
        
        if(opt < 1 || opt > 5) break;
        if(opt == 1){
            showAllBooks();
        }
        else if(opt == 2){
            struct book u1;
            int status;
            printf("enter title: ");
            scanf("%s", u1.title);
            printf("enter author: ");
            scanf("%s", u1.author);
            printf("enter category: ");
            scanf("%s", u1.category);
            printf("Enter no_of_copies: ");
            scanf("%d", &u1.copies_left);
            createBook(u1, &status);
            if(status == 0){
                printf("successfully inserted book!!\n");
            }
            else if(status == 2){
                printf("book with that phone no or title already exists\n");
            }
            else{
                printf("some other error occured");
            }
        }
        else if(opt == 3){  
            char title[20];
            printf("enter title: ");
            scanf("%s", title);
            int offset;
            if(findbook(title, &offset) == NULL) {
                printf("book not found with that title\n");
                continue;
            }
            else{
                int status;
                struct book newbook;
                printf("Enter details of new book\n");
                printf("Enter title: ");
                scanf("%s", newbook.title);
                printf("Enter author: ");
                scanf("%s", newbook.author);
                printf("Enter category: ");
                scanf("%s", newbook.category);
                printf("Enter no_of_copies: ");
                scanf("%d", &newbook.copies_left);
                updateBook(title, newbook, &status);
                if(status == 0){
                    printf("updated book sucessfully\n");
                }
                else if(status == DUPLICATE_BOOK){
                    printf("could not update as there is another book with same title\n");
                }
                else{
                    printf("some other error occured\n");
                }
                printf("status after update is %d\n", status);
            }
            
        }
        else if(opt == 4){
            char title[20];
            printf("Enter the title you want to delete: ");
            scanf("%s", title);
            int status;
            deleteBook(title, &status);
            if(status == BOOK_NOT_FOUND){
                printf("no such book exists!\n");
            }
            else if(status == 0){
                printf("deleted book successfully\n");
            }
        }
        else if(opt == 5){
            char title[20];
            printf("enter the title: ");
            scanf("%s", title);
            int offset;
            struct book * u1 = findbook(title, &offset);
            if(u1 == NULL){
                printf("book not found\n");
            }
            else{
                printf("book found at offset %d\n", offset);
                showbook(*u1);
            }
        }
    }
    endbook();

    return 0;
}