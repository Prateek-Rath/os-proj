#include"generic.h"
#include <sys/file.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#define existsUser(u1) 0

void startuser(){
    //open required files
    userglob.fd = open(usrfile, O_RDWR, 0777);
    int ret = read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
    flock(userglob.fd, LOCK_SH);
    read(userglob.fd, &userglob.count, sizeof(int));
    
    // printf("cur_user_id is %d and count is %d\n", userglob.cur_usr_id, userglob.count);
    
    lseek(userglob.fd, 0, SEEK_SET);
    if(userglob.fd == -1){
        printf("error\n");
        flock(userglob.fd, LOCK_UN);
        exit(0);
    }
    printf("startuser done\n");
    flock(userglob.fd, LOCK_UN);
    sync();
    return ;

}



void createentity( enum types type, void * u, int * status )
{
    switch( type )
    {
        case user:{
            struct user u1 = *((struct user *)u);
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

            break;
        }
        default: 
        break;
    }
}







int main(){
    struct user * u;
    startuser();
    u = (struct user *)malloc(sizeof(struct user));
    strcpy(u->first_name, "abc");
    strcpy(u->last_name,"abcdef");
    strcpy(u->password, "a");
    strcpy(u->phone, "999");
    int status;
    createentity(user, u, &status);
    if(status == 0){
        printf("created successfullly\n");
    }
    else{
        printf("status is %d", status);
    }
}