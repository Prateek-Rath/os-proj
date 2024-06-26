#ifndef COMMUNICATION
#define COMMUNICATION
#ifndef ALL
#define ALL
// #include"./entities/all.h"
#include"./entities/user.h"
#include"./entities/admin.h"
#include"./entities/book.h"
#include"./entities/borrow.h"
#include<semaphore.h>
#endif

#include<pthread.h>

#define PORT 5544


enum states
{
    start,
    user_login_process, 
    admin_login_process, 
    user_register_process, 
    admin_register_process, 
    user_wrong_username, 
    admin_wrong_username, 
    user_logged_in, 
    admin_logged_in
};

struct state{
    enum states val;//the current state
    // I also might want to know who is logged in
    union who{
        struct user user;
        struct admin admin;
    }who;
};

extern struct state state;


void structstatecpy(struct state* s1, struct state* s2);



void structstatecpy(struct state* s1, struct state* s2){
    s1->val = s2->val;
    structusercpy(&s1->who.user, &s2->who.user);
}

struct comminfo{//communication info
    pthread_t tid;
    int newfd;
};




enum operation{
    findbook,
    listbooks,
    addbook,
    deletebook,
    modifybook,
    borrowbook,
    returnbook,
    loginuser,
    loginadmin,
    registeruser,
    registeradmin,
    deleteadmin,
    deleteuser,
    listusers,
    listadmins,
    listborrows,
    getuserbooklist,
    donothing
};

union uni{
    struct findbook {
        char title[100]; //the current state
    }findbook;
    
    struct listbooks{
        int val; //the current state
    }listbooks;


    struct addbook{
        struct book book;
    }addbook;
    
    struct deletebook{
        char title[100];
    }deletebook;

    struct modifybook{
        char oldtitle[40];
        struct book book;
    }modifybook;

    struct borrowbook{
        struct borrow borrow;
    }borrowbook;

    struct returnbook{
        char title[100];
        char username[100];
    }returnbook;

    
    struct loginuser{
        char username[20];
        char password[40];
    }loginuser;
    
    struct loginadmin{
        char username[20];
        char password[40];
    }loginadmin;
    
    struct registeruser{
        struct user user;
    }registeruser;

    struct registeradmin{
        struct admin admin;
    }registeradmin;

    struct deleteuser{
        char username[20];
    }deleteuser;

    struct deleteadmin{
        char username[20];
    }deleteadmin;

    struct userbooks{
        char username[20];
    }userbooks;


}uni;

//we need a format to send and receive messages
struct message{
    union uni dataptr; //the data the server needs to do it's operation
    struct state state;// the current state value is passed to the server
    enum operation operation; // what the server must do
};

struct reply{
    char text[10000];
    struct state newstate;
};





//the message is going to be an array of strings
//the first word is going to be one of these:
//-loginuser //-registeruser //-loginadmin //-registeradmin
//-findbook //-borrowbook //-returnbook //-listbooks
 //-addbook //-deletebook //-modifybook
//we don't allow purchases


#endif
