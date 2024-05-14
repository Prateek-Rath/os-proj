#include "./admin.h"
#include "./admin.h"
#include<stdio.h>


    
int main()
{
    printf("admin one %d\n", getpid());
    initadmin();
    startadmin();

    int opt;
    while(1){
        //menu driven admin crud
        
        printf("1. showAll, 2. insert, 3. update, 4. delete, 5. find_by_username, other to exit\n");
        printf("enter option: ");
        scanf("%d", &opt);
        if(opt < 1 || opt > 5) break;
        if(opt == 1){
            showAllAdmins();
        }
        else if(opt == 2){
            struct admin u1;
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
            createAdmin(u1, &status);
            if(status == 0){
                printf("successfully inserted admin!!\n");
            }
            else if(status == 2){
                printf("admin with that phone no or username already exists");
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
            if(findadmin(username, &offset) == NULL) {
                printf("admin not found with that username\n");
                continue;
            }
            else{
                int status;
                struct admin newadmin;
                printf("Enter details of new admin\n");
                printf("Enter username: ");
                scanf("%s", newadmin.username);
                printf("Enter first_name: ");
                scanf("%s", newadmin.first_name);
                printf("Enter last_name: ");
                scanf("%s", newadmin.last_name);
                printf("Enter password: ");
                scanf("%s", newadmin.password);
                printf("Enter phone number: ");
                scanf("%s", newadmin.phone);
                updateAdmin(username, newadmin, &status);
                if(status == 0){
                    printf("updated admin sucessfully\n");
                }
                else if(status == DUPLICATE_ADMIN){
                    printf("could not update as there is another admin with same username\n");
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
            deleteAdmin(username, &status);
            if(status == ADMIN_NOT_FOUND){
                printf("no such admin exists!\n");
            }
            else if(status == 0){
                printf("deleted admin successfully\n");
            }
        }
        else if(opt == 5){
            char username[20];
            printf("enter the username: ");
            scanf("%s", username);
            int offset;
            struct admin * u1 = findadmin(username, &offset);
            if(u1 == NULL){
                printf("admin not found\n");
            }
            else{
                printf("admin found at offset %d", offset);
                showadmin(*u1);
            }
        }
    }
    endadmin();

    return 0;
}
