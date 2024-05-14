#define DUPLICATE_USER 2
#define USER_NOT_FOUND 2

char * usrfile = "../files/user.dat";

enum types {
    user, book, admin, borrow
} type1;



struct userglobals{
    int cur_usr_id;
    int count;
    int fd;
} userglob;



struct user{
    int id; //primary key //the id is something that I decide // and I auto increment it....
    char username[20]; // must be unique
    char first_name[30];
    char last_name[30];
    char password[40];
    char phone[20];//check if it is 10 digits if possible
};