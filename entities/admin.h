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



#define DUPLICATE_ADMIN 2
#define ADMIN_NOT_FOUND 2

char * admfile = "../files/admin.dat";


struct admin{
    int id; //primary key //the id is something that I decide // and I auto increment it....
    char username[20]; // must be unique
    char first_name[30];
    char last_name[30];
    char password[40];
    char phone[20];//check if it is 10 digits if possible
};


void structadmincpy(struct admin a1, struct admin a2){
    a1.id = a2.id;
    strcpy(a1.first_name, a2.first_name);
    strcpy(a1.last_name, a2.last_name);
    strcpy(a1.phone, a2.phone);
    strcpy(a1.password, a2.password);
    strcpy(a1.username, a2.username);
}


struct adminglobals{
    int cur_adm_id;
    int count;
    int fd;
} adminglob;


bool adminequals(struct admin u1, struct admin u2){
    if(strcmp(u1.username, u2.username) == 0 || strcmp(u1.phone, u2.phone) == 0){
        return true;
    }
    else {
        // printf("not equal\n");
        // printf("1: %s, 2: %s", u1.username, u2.username);
        return false;
    }

}

void initadmin(){ //to be called only once
    adminglob.fd  = open(admfile, O_CREAT|O_WRONLY, 0777);
    if(adminglob.fd == -1){
        printf("open failed\n");
        exit(0);
    }
    adminglob.cur_adm_id = 0;
    adminglob.count = 0;
    flock(adminglob.fd, LOCK_EX); printf("exclusive locked adminfile");
    write(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    write(adminglob.fd, &adminglob.count, sizeof(int));
    flock(adminglob.fd, LOCK_UN); printf("unlocked adminfile\n");
    close(adminglob.fd);
    adminglob.fd = -1;
    printf("initadmin done\n");
    
    sync();
}


void startadmin(){
    //open required files
    adminglob.fd = open(admfile, O_RDWR, 0777);
    int ret = read(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    flock(adminglob.fd, LOCK_SH);
    read(adminglob.fd, &adminglob.count, sizeof(int));
    
    // printf("cur_admin_id is %d and count is %d\n", adminglob.cur_adm_id, adminglob.count);
    
    lseek(adminglob.fd, 0, SEEK_SET);
    if(adminglob.fd == -1){
        printf("error\n");
        flock(adminglob.fd, LOCK_UN);
        exit(0);
    }
    printf("startadmin done\n");
    flock(adminglob.fd, LOCK_UN);
    sync();
    return ;

}


bool existsAdmin(struct admin u1){
    flock(adminglob.fd, LOCK_SH);
    lseek(adminglob.fd, 0, SEEK_SET);
    read(adminglob.fd, &(adminglob.cur_adm_id), sizeof(int));
    read(adminglob.fd, &(adminglob.count), sizeof(int));
    if(adminglob.count == 0){
        return false;
    }
    else{
        struct admin temp;
        for(int i=0; i<adminglob.count; i++){
            read(adminglob.fd, &temp, sizeof(temp));
            if(adminequals(u1 , temp)){
                flock(adminglob.fd, LOCK_UN);
                return true;
            }
        }
        flock(adminglob.fd, LOCK_UN);
        return false;
    }
    sync();
    flock(adminglob.fd, LOCK_UN);
}

void createAdmin(struct admin u1, int *status){
    //open the file 
    //ensure id and username are unique
    //insert at the end of the file
    flock(adminglob.fd, LOCK_EX);
    lseek(adminglob.fd, 0, SEEK_SET);
    if(existsAdmin(u1)){
        *status = DUPLICATE_ADMIN;//duplicate admin
        flock(adminglob.fd, LOCK_UN);
        return;
    }
    lseek(adminglob.fd, 0, SEEK_SET);
    read(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    read(adminglob.fd, &adminglob.count, sizeof(int));
    
    adminglob.count++;
    adminglob.cur_adm_id++;
    u1.id = adminglob.cur_adm_id;

    lseek(adminglob.fd, 0, SEEK_SET);
    write(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    write(adminglob.fd, &adminglob.count, sizeof(int));

    // printf("wrote %d\n", adminglob.cur_adm_id);
    // printf("wrote count: %d\n", adminglob.count);

    lseek(adminglob.fd, 0, SEEK_END);
    write(adminglob.fd, &u1, sizeof(u1));

    *status = 0; //success
    sync();
    flock(adminglob.fd, LOCK_UN);
}

struct admin * findadmin(char * username, int * offset){
    flock(adminglob.fd, LOCK_SH);
    *offset = 0;
    lseek(adminglob.fd, 0, SEEK_SET);
    read(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    read(adminglob.fd, &adminglob.count, sizeof(int));
    //count is the number of records
    if(adminglob.count == 0){
        flock(adminglob.fd, LOCK_UN);
        return NULL;
    }
    else{
        *offset = lseek(adminglob.fd, 0, SEEK_CUR);
        struct admin *temp = (struct admin *)malloc(sizeof(struct admin));
        for(int i=0; i<adminglob.count; i++){
            read(adminglob.fd, temp, sizeof(struct admin));
            if(strcmp(temp->username, username) == 0){
                sync();
                flock(adminglob.fd, LOCK_UN);
                return temp;
            }
            else{
                *offset = lseek(adminglob.fd, 0, SEEK_CUR);
                // printf("username1: %s", temp->username);
                // printf(" username2: %s\n", username);
            }
        }
        sync();
        flock(adminglob.fd, LOCK_UN);
        return NULL;
    }
}


void updateAdmin(char *username, struct admin newadmin, int * status){
    //first check if the admin actually exists
    int offset;
    struct admin * u1 = findadmin(username, &offset); // it will be better if it returns the desired offset in the file also
    if(u1 == NULL){
        *status = ADMIN_NOT_FOUND;
        return;
    }
    else{
        // u1->first_name = newadmin.first_name;
        // u1->last_name = newadmin.last_name;
        flock(adminglob.fd, LOCK_EX);
        newadmin.id = u1->id;
        lseek(adminglob.fd, offset, SEEK_SET);
        struct admin oldadmin;
        read(adminglob.fd, &oldadmin, sizeof(struct admin));
        lseek(adminglob.fd, offset, SEEK_SET);
        if(existsAdmin(newadmin) && (strcmp(newadmin.username, oldadmin.username) != 0)){
            printf("there is already a admin with that phone number or username\n");
            *status = DUPLICATE_ADMIN;
            flock(adminglob.fd, LOCK_UN);
            return;
        }
        else{
            lseek(adminglob.fd, offset, SEEK_SET);
            write(adminglob.fd, &newadmin, sizeof(struct admin));
            *status = 0;
            sync();
            flock(adminglob.fd, LOCK_EX);
            return;
        }
        
    }
    flock(adminglob.fd, LOCK_EX);
}


void deleteAdmin(char * username, int * status){
    int offset;
    struct admin * u1 = findadmin(username, &offset);
    if(u1 == NULL){
        *status = ADMIN_NOT_FOUND;
        return;
    }
    // struct admin deladmin = *u1;
    // deladmin.id = -1;
    // lseek(adminglob.fd, offset, SEEK_SET);
    // write(adminglob.fd, &deladmin, sizeof(struct admin));
    // *status = 0;
    // return;
    flock(adminglob.fd, LOCK_EX);
    lseek(adminglob.fd, 0, SEEK_SET);
    read(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
    read(adminglob.fd, &adminglob.count, sizeof(int));


    struct admin lastadmin;
    lseek(adminglob.fd, -sizeof(struct admin), SEEK_END);
    read(adminglob.fd, &lastadmin, sizeof(struct admin));

    lseek(adminglob.fd, offset, SEEK_SET);
    write(adminglob.fd, &lastadmin, sizeof(struct admin));
    adminglob.count--;
    lseek(adminglob.fd, sizeof(int), SEEK_SET);
    write(adminglob.fd, &adminglob.count, sizeof(int));

    *status = 0;
    int pos = lseek(adminglob.fd, 0, SEEK_END);
    ftruncate(adminglob.fd, pos-sizeof(struct admin));
    flock(adminglob.fd, LOCK_UN);
    sync();
}




void showadmin(struct admin u1){
    if(u1.id == -1) return;
    printf("id: %d\n", u1.id);
    printf("username: %s\n", u1.username);
    printf("fname: %s\n", u1.first_name);
    printf("lname: %s\n", u1.last_name);
    printf("password: %s\n", u1.password);
    printf("phone: %s\n", u1.phone);
    printf("---------------------------\n");
}
    
void showAllAdmins(){
    int val;
    flock(adminglob.fd, LOCK_SH);
    lseek(adminglob.fd, 0, SEEK_SET);
    read(adminglob.fd, &val, sizeof(int));
    read(adminglob.fd, &val, sizeof(int));
    struct admin temp;
    if(val == 0){
        printf("no admins right right now!!\n");
        printf("---------------------------\n");
    }
    for(int i=0; i<val; i++){
        read(adminglob.fd, &temp, sizeof(temp));
        showadmin(temp);
    }
    flock(adminglob.fd, LOCK_UN);
}

void endadmin(){
    close(adminglob.fd);
    adminglob.fd = -1;
    printf("endadmin done\n");
}



///adm/lib/gcc/x86_64-linux-gnu/11/

#endif