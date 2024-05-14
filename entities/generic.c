#include"generic.h"
#include <sys/file.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void initSemaphores(){
    sem_init(&sems.semusr, 1, 1);
    sem_init(&sems.semadm, 1, 1);
    sem_init(&sems.sembk, 1, 1);
    sem_init(&sems.semborr, 1, 1);
}


void initEntity(enum types type){
        if(type == user){
            sem_wait(&sems.semusr);
            userglob.fd  = open(usrfile, O_CREAT|O_WRONLY, 0777);
            if(userglob.fd == -1){
                printf("open failed\n");
            }
            userglob.cur_usr_id = 0;
            userglob.count = 0;
            write(userglob.fd, &userglob.cur_usr_id, sizeof(int));
            write(userglob.fd, &userglob.count, sizeof(int));
            close(userglob.fd);
            userglob.fd = -1;
            printf("inituser done\n");
            sync();
            sem_post(&sems.semusr);
        }
        else if(type == admin){
            sem_wait(&sems.semadm);
            adminglob.fd  = open(admfile, O_CREAT|O_WRONLY, 0777);
            if(adminglob.fd == -1){
                printf("open failed\n");
                sem_post(&sems.semusr);
                exit(0);
            }
            adminglob.cur_adm_id = 0;
            adminglob.count = 0;
            write(adminglob.fd, &adminglob.cur_adm_id, sizeof(int));
            write(adminglob.fd, &adminglob.count, sizeof(int));
            close(adminglob.fd);
            adminglob.fd = -1;
            printf("initadmin done\n");
            
            sync();
            sem_post(&sems.semadm);
        }
        else if(type == book){
            sem_wait(&sems.sembk);
            bookglob.fd  = open(bkfile, O_CREAT|O_WRONLY, 0777);
            if(bookglob.fd == -1){
                printf("open failed\n");
                sem_post(&sems.semusr);
                exit(0);
            }
            bookglob.cur_bk_id = 0;
            bookglob.count = 0;
            lseek(bookglob.fd, 0, SEEK_SET);
            write(bookglob.fd, &bookglob.cur_bk_id, sizeof(int));
            write(bookglob.fd, &bookglob.count, sizeof(int));
            close(bookglob.fd);
            bookglob.fd = -1;
            printf("initbook done\n");
            
            sync();
            sem_post(&sems.sembk);
        }
        else if(type == borrow){
            sem_wait(&sems.semborr);
            borrowglob.fd  = open(borrfile, O_CREAT|O_WRONLY, 0777);
            if(borrowglob.fd == -1){
                printf("open failed\n");
                sem_post(&sems.semusr);
                exit(0);
            }
            borrowglob.cur_borr_id = 0;
            borrowglob.count = 0;
            write(borrowglob.fd, &borrowglob.cur_borr_id, sizeof(int));
            write(borrowglob.fd, &borrowglob.count, sizeof(int));
            close(borrowglob.fd);
            borrowglob.fd = -1;
            printf("initborrow done\n");
            
            sync();
            sem_post(&sems.semborr);
        }
        else{
            printf("that type was not found\n");
            exit(0);
        }
}

void startEntity(enum types type){
    switch(type){
        case user:
            sem_wait(&sems.semusr);
            userglob.fd = open(usrfile, O_RDWR, 0777);
            int ret = read(userglob.fd, &userglob.cur_usr_id, sizeof(int));
            read(userglob.fd, &userglob.count, sizeof(int));
            
            // printf("cur_user_id is %d and count is %d\n", userglob.cur_usr_id, userglob.count);
            
            lseek(userglob.fd, 0, SEEK_SET);
            if(userglob.fd == -1){
                printf("error 12344534591085918\n");
                sem_post(&sems.semusr);
                exit(0);
            }
            printf("startuser done\n");
            sync();
            sem_post(&sems.semusr);
            return ;
    }
}

void addEntity( enum types type, void * u, int * status )
{
    switch( type )
    {
        case user:{
            struct user u1 = *((struct user *)u);
            lseek(userglob.fd, 0, SEEK_SET);
            if(existsUser(u1)){
                *status = DUPLICATE_USER;//duplicate user
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

            break;
        }
        default: 
        break;
    }
}


void existsEntity(enum types type, void * u){
    if(type == user){
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
                    return true;
                }
            }
            return false;
        }
        sync();
    }
    else if(type == admin){

    }
    else if(type == book){

    }
    else if(type == borrow){

    }
    else{
        printf("unknown type\n");
        exit(0);
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
    return 0;
}