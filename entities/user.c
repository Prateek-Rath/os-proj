#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdbool.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include<semaphore.h>


#include"user.h"

struct userglobals userglob;

sem_t usersem;


void structusercpy(struct user *u1, struct user *u2){
    u1->id = u2->id;
    strcpy(u1->first_name, u2->first_name);
    strcpy(u1->last_name, u2->last_name);
    strcpy(u1->phone, u2->phone);
    strcpy(u1->password, u2->password);
    strcpy(u1->username, u2->username);
}


bool userequals(struct user u1, struct user u2){
    if(strcmp(u1.username, u2.username) == 0 || strcmp(u1.phone, u2.phone) == 0){
        return true;
    }
    else {
        // printf("not equal\n");
        // printf("1: %s, 2: %s", u1.username, u2.username);
        return false;
    }

}

void inituser(){ //to be called only once
    userglob.fd  = open(usrfile, O_CREAT|O_WRONLY|O_TRUNC, 0777);
    if(userglob.fd == -1){
        printf("open failed\n");
        printf("errno is %d\n", errno);
        switch( errno )
        {
            case EACCES: // permission issues
            // handle_permission_issue();
            printf("permission issue\n");
            break;

            case EEXIST: // file already exists and you used O_CREAT and O_EXCL
            // handle_already_exists_issue();
            printf("already exists\n");
            break;

            case EFAULT: // bad path
            printf("bad path issue\n");
            break;
            
            default:
            printf("no error is felt\n");
        }
        exit(0);
    }
    userglob.cur_usr_id = 0;
    userglob.count = 0;
    flock(userglob.fd, LOCK_EX); 
    // printf("exclusive locked userfile");
    write(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    write(userglob.fd, &userglob.count, sizeof(int));
    flock(userglob.fd, LOCK_UN); 
    // printf("unlocked userfile\n");
    close(userglob.fd);
    userglob.fd = -1;
    // printf("inituser done\n");
    
    sync();
}


void startuser(){
    //open required files
    userglob.fd = open(usrfile, O_RDWR, 0777);
    sem_init(&usersem, 1, 1);
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    int ret = read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    read(userglob.fd, &userglob.count, sizeof(int));
    
    // printf("cur_user_id is %d and count is %d\n", userglob.cur_usr_id, userglob.count);
    
    lseek(userglob.fd, 0, SEEK_SET);
    if(userglob.fd == -1){
        printf("error 12344534591085918\n");
        flock(userglob.fd, LOCK_UN);
        exit(0);
    }
    // printf("startuser done\n");
    flock(userglob.fd, LOCK_UN);
    sync();
    return ;

}





bool existsUser(struct user u1){
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &(userglob.cur_usr_id), sizeof(int));
    read(userglob.fd, &(userglob.count), sizeof(int));
    if(userglob.count == 0){
        return false;
    }
    else{
        struct user temp;
        for(int i=0; i<userglob.count; i++){
            read(userglob.fd, &temp, sizeof(temp));
            if(userequals(u1 , temp)){
                flock(userglob.fd, LOCK_UN);
                return true;
            }
        }
        flock(userglob.fd, LOCK_UN);
        return false;
    }
    sync();
    flock(userglob.fd, LOCK_UN);
}

#define USERTYPE 1

// types 1- jhj  2- 3 - kj





void createUser(struct user u1, int *status){
    //open the file 
    //ensure id and username are unique
    //insert at the end of the file
    // printf("received user: \n");
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    if(existsUser(u1)){
        *status = DUPLICATE_USER;//duplicate user
        flock(userglob.fd, LOCK_UN);
        return;
    }
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    read(userglob.fd, &userglob.count, sizeof(int));
    
    userglob.count++;
    userglob.cur_usr_id++;
    u1.id = userglob.cur_usr_id;

    lseek(userglob.fd, 0, SEEK_SET);
    write(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    write(userglob.fd, &userglob.count, sizeof(int));

    // printf("wrote %d\n", userglob.cur_usr_id);
    // printf("wrote count: %d\n", userglob.count);

    lseek(userglob.fd, 0, SEEK_END);
    write(userglob.fd, &u1, sizeof(u1));

    *status = 0; //success
    sync();
    flock(userglob.fd, LOCK_UN);
}




struct user * finduser(char * username, int * offset){
    // printf("attempting to share lock userfile\n");
    flock(userglob.fd, LOCK_EX);
    // printf("share locked userfile\n");
    *offset = 0;
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    read(userglob.fd, &userglob.count, sizeof(int));
    //count is the number of records
    if(userglob.count == 0){
        flock(userglob.fd, LOCK_UN);
        // printf("unlocked userfile\n");
        return NULL;
    }
    else{
        *offset = lseek(userglob.fd, 0, SEEK_CUR);
        struct user *temp = (struct user *)malloc(sizeof(struct user));
        for(int i=0; i<userglob.count; i++){
            int ret = read(userglob.fd, temp, sizeof(struct user));
            if(ret == 0)
            {
                printf("that shouldn't happend\n");
                exit(0);
            }
            if(strcmp(temp->username, username) == 0){
                sync();
                flock(userglob.fd, LOCK_UN);
                printf("unlocked userfile\n");
                return temp;
            }
            else{
                *offset = lseek(userglob.fd, 0, SEEK_CUR);
                printf("did not match with: \n");
                showuser(*temp);
            }
        }
        sync();
        // printf("unlocked userfile\n");
        flock(userglob.fd, LOCK_UN);
        if(existsUser(*temp)){
            printf("this should never ever happend\n");
        }
        return NULL;
    }
}


char * listUsers(){
    char * ans;
    ans = malloc(100 * sizeof(struct user));
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    read(userglob.fd, &userglob.count, sizeof(int));
    strcpy(ans, "");
    // printf("userglob.count in listUsers is %d\n", userglob.count);
    char * ans1 = malloc(200);
    for(int i=0; i<userglob.count; i++){
        struct user temp;
        read(userglob.fd, &temp, sizeof(struct user));
        sprintf(ans1, "username: %s, phone: %s, first_name: %s, last_name: %s\n", temp.username, temp.phone, temp.first_name, temp.last_name);
        strcat(ans, ans1);
    }
    flock(userglob.fd, LOCK_UN);
    return ans;
}

void validateUser(char * username, char * password, int * status){
    // printf("in validateUser function\n");
    int offset;
    struct user * u1 = finduser(username, &offset);
    if(u1 == NULL){
        // printf("user not found");
        *status = USER_NOT_FOUND;
        return;
    }
    else{
        if(strcmp(u1->password, password) == 0){
            // printf("successful validation\n");
            *status = 0;
        }
        else{
            // printf("wrong password");
            *status = INCORRECT_PASSWORD;
        }
    }
    // printf("doing free\n");
    free(u1);
    // printf("free done\n");
}

void updateUser(char *username, struct user newuser, int * status){
    //first check if the user actually exists
    int offset;
    struct user * u1 = finduser(username, &offset); // it will be better if it returns the desired offset in the file also
    if(u1 == NULL){
        *status = USER_NOT_FOUND;
        return;
    }
    else{
        // u1->first_name = newuser.first_name;
        // u1->last_name = newuser.last_name;
        flock(userglob.fd, LOCK_EX);
        newuser.id = u1->id;
        lseek(userglob.fd, offset, SEEK_SET);
        struct user olduser;
        read(userglob.fd, &olduser, sizeof(struct user));
        lseek(userglob.fd, offset, SEEK_SET);
        if(existsUser(newuser) && (strcmp(newuser.username, olduser.username) != 0)){
            // printf("there is already a user with that phone number or username\n");
            *status = DUPLICATE_USER;
            flock(userglob.fd, LOCK_UN);
            free(u1);
            return;
        }
        else{
            lseek(userglob.fd, offset, SEEK_SET);
            write(userglob.fd, &newuser, sizeof(struct user));
            *status = 0;
            sync();
            flock(userglob.fd, LOCK_EX);
            free(u1);
            return;
        }
        
    }
    free(u1);
    flock(userglob.fd, LOCK_EX);
}


void deleteUser(char * username, int * status){
    int offset;
    struct user * u1 = finduser(username, &offset);
    if(u1 == NULL){
        *status = USER_NOT_FOUND;
        return;
    }
    // struct user deluser = *u1;
    // deluser.id = -1;
    // lseek(userglob.fd, offset, SEEK_SET);
    // write(userglob.fd, &deluser, sizeof(struct user));
    // *status = 0;
    // return;
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    read(userglob.fd, &userglob.count, sizeof(int));


    struct user lastuser;
    lseek(userglob.fd, -sizeof(struct user), SEEK_END);
    read(userglob.fd, &lastuser, sizeof(struct user));

    lseek(userglob.fd, offset, SEEK_SET);
    write(userglob.fd, &lastuser, sizeof(struct user));
    userglob.count--;
    lseek(userglob.fd, sizeof(int), SEEK_SET);
    write(userglob.fd, &userglob.count, sizeof(int));

    *status = 0;
    int pos = lseek(userglob.fd, 0, SEEK_END);
    ftruncate(userglob.fd, pos-sizeof(struct user));
    flock(userglob.fd, LOCK_UN);
    free(u1);
    sync();
}




void showuser(struct user u1){
    if(u1.id == -1) return;
    printf("id: %d\n", u1.id);
    printf("username: %s\n", u1.username);
    printf("fname: %s\n", u1.first_name);
    printf("lname: %s\n", u1.last_name);
    printf("password: %s\n", u1.password);
    printf("phone: %s\n", u1.phone);
    printf("---------------------------\n");
}
    
void showAllUsers(){
    int val;
    flock(userglob.fd, LOCK_EX);
    lseek(userglob.fd, 0, SEEK_SET);
    read(userglob.fd, &val, sizeof(int));
    read(userglob.fd, &val, sizeof(int));
    struct user temp;
    if(val == 0){
        printf("no users right right now!!\n");
        printf("---------------------------\n");
    }
    for(int i=0; i<val; i++){
        read(userglob.fd, &temp, sizeof(temp));
        showuser(temp);
    }
    flock(userglob.fd, LOCK_UN);
}

void enduser(){
    close(userglob.fd);
    userglob.fd = -1;
    printf("enduser done\n");
}



///usr/lib/gcc/x86_64-linux-gnu/11/








