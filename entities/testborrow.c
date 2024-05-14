#include "./borrow.h"
#include<stdio.h>


    
int main()
{
    printf("borrow one %d\n", getpid());
    initborrow();
    startborrow();
    

    int opt;
    while(1){
        //menu driven borrow crud
        
        printf("1. showAll, 2. borrow, 4. return, 5. find_by_title, 6. show all books, other to exit\n");
        printf("enter option: ");
        scanf("%d", &opt);
        if(opt == 1){
            showAllBorrows();
        }
        else if(opt == 2){
            struct borrow u1;
            int status;
            printf("enter title: ");
            scanf("%s", u1.title);
            printf("enter username: ");
            scanf("%s", u1.username);
            printf("enter phone: ");
            scanf("%s", u1.phone);
            borrowBook(u1, &status);
            if(status == 0){
                printf("successfully inserted borrow!!\n");
            }
            else if(status == 2){
                printf("borrow with that phone no or title already exists\n");
            }
            else if (status == 4){
                printf("not enough copies\n");
            }
            else{
                printf("some other error occured");
            }
        }
        else if(opt == 3){  
            
            
        }
        else if(opt == 4){
            char title[20];
            char username[20];
            printf("Enter the title you want to return: ");
            scanf("%s", title);
            printf("Enter your username: ");
            scanf("%s", username);
            int status;
            returnBook(title, username, &status);
            if(status == BORROW_NOT_FOUND){
                printf("the user doesn't own that book!\n");
            }
            else if(status == 0){
                printf("returned successfully\n");
            }
            else{
                printf("other error\n");
            }
        }
        else if(opt == 5){
            char title[20];
            char username[20];
            printf("enter the title: ");
            scanf("%s", title);
            printf("enter the username: ");
            scanf("%s", username);
            int offset;
            struct borrow * u1 = findborrow(title, username, &offset);
            if(u1 == NULL){
                printf("borrow not found\n");
            }
            else{
                printf("borrow found at offset %d\n", offset);
                showborrow(*u1);
            }
        }
        else if(opt == 6){
            
            showAllBooks();
        }
        else{
            break;
        }
    }
    endborrow();

    return 0;
}