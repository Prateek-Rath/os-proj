#include "./user.h"
#include<stdio.h>
#include <sys/file.h>
#include<unistd.h>

    
int main()
{
    printf("user one %d\n", getpid());
    inituser();
    startuser();

    int opt;
    while(1){
        //menu driven user crud
        
        printf("1. showAll, 2. insert, 3. update, 4. delete, 5. find_by_username, other to exit\n");
        printf("enter option: ");
        scanf("%d", &opt);
        if(opt < 1 || opt > 5) break;
        if(opt == 1){
            showAllUsers();
        }
        else if(opt == 2){
            struct user u1;
            int status;
            printf("enter username: ");
            scanf("%s", u1.username);
            printf("enter first_name: ");
            scanf("%s", u1.first_name);
            printf("enter last_name: ");
            scanf("%s", u1.last_name);
            printf("Enter password: ");
            scanf("%s", u1.password);
            printf("enter phone_no: ");
            scanf("%s", u1.phone);
            createUser(u1, &status);
            if(status == 0){
                printf("successfully inserted user!!\n");
            }
            else if(status == 2){
                printf("user with that phone no or username already exists");
            }
            else{
                printf("some other error occured");
            }
        }
        else if(opt == 3){  
            char username[20];
            printf("enter username: ");
            scanf("%s", username);
            int offset;
            if(finduser(username, &offset) == NULL) {
                printf("user not found with that username\n");
                continue;
            }
            else{
                int status;
                struct user newuser;
                printf("Enter details of new user\n");
                printf("Enter username: ");
                scanf("%s", newuser.username);
                printf("Enter first_name: ");
                scanf("%s", newuser.first_name);
                printf("Enter last_name: ");
                scanf("%s", newuser.last_name);
                printf("Enter password: ");
                scanf("%s", newuser.password);
                printf("Enter phone number: ");
                scanf("%s", newuser.phone);
                updateUser(username, newuser, &status);
                if(status == 0){
                    printf("updated user sucessfully\n");
                }
                else if(status == DUPLICATE_USER){
                    printf("could not update as there is another user with same username\n");
                }
                else{
                    printf("some other error occured\n");
                }
            }
            
        }
        else if(opt == 4){
            char username[20];
            printf("Enter the username you want to delete: ");
            scanf("%s", username);
            int status;
            deleteUser(username, &status);
            if(status == USER_NOT_FOUND){
                printf("no such user exists!\n");
            }
            else if(status == 0){
                printf("deleted user successfully\n");
            }
        }
        else if(opt == 5){
            char username[20];
            printf("enter the username: ");
            scanf("%s", username);
            int offset;
            struct user * u1 = finduser(username, &offset);
            if(u1 == NULL){
                printf("user not found\n");
            }
            else{
                printf("user found at offset %d", offset);
                showuser(*u1);
            }
        }
    }
    enduser();

    return 0;
}
