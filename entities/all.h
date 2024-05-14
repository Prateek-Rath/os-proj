// // #ifndef ALL
// // #define ALL


// #ifndef USER
// #define USER

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include<stdbool.h>
// #include<string.h>
// #include<stdio.h>
// #include<stdlib.h>
// #include <sys/file.h>
// #include <errno.h>



// #define DUPLICATE_USER 2
// #define USER_NOT_FOUND 3
// #define INCORRECT_PASSWORD 4


// #define usrfile "../files/user.dat"

// struct userglobals{
//     int cur_usr_id;
//     int count;
//     int fd;
// } userglob;

// struct user{
//     int id; //primary key //the id is something that I decide // and I auto increment it....
//     char username[20]; // must be unique
//     char first_name[30];
//     char last_name[30];
//     char password[40];
//     char phone[20];//check if it is 10 digits if possible
// };

// void structusercpy(struct user u1, struct user u2);

// bool userequals(struct user u1, struct user u2);

// void inituser();

// void startuser();

// bool existsUser(struct user u1);

// void createUser(struct user u1, int *status);

// struct user * finduser(char * username, int * offset);

// void validateUser(char * username, char * password, int * status);

// void updateUser(char *username, struct user newuser, int * status);

// void deleteUser(char * username, int * status);

// void showuser(struct user u1);
    
// void showAllUsers();

// void enduser();

// #endif



// #ifndef ADMIN
// #define ADMIN

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include<stdbool.h>
// #include<string.h>
// #include<stdio.h>
// #include<stdlib.h>
// #include <sys/file.h>



// #define DUPLICATE_ADMIN 2
// #define ADMIN_NOT_FOUND 3

// #define admfile "../files/admin.dat"

// struct adminglobals{
//     int cur_adm_id;
//     int count;
//     int fd;
// } adminglob;

// struct admin{
//     int id; //primary key //the id is something that I decide // and I auto increment it....
//     char username[20]; // must be unique
//     char first_name[30];
//     char last_name[30];
//     char password[40];
//     char phone[20];//check if it is 10 digits if possible
// };

// void structadmincpy(struct admin a1, struct admin a2);

// bool adminequals(struct admin u1, struct admin u2);

// void initadmin();

// void startadmin();

// bool existsAdmin(struct admin u1);

// void createAdmin(struct admin u1, int *status);

// struct admin * findadmin(char * username, int * offset);

// void validateAdmin(char * username, char * password, int * status);

// void updateAdmin(char *username, struct admin newadmin, int * status);


// void deleteAdmin(char * username, int * status);

// void showadmin(struct admin u1);
    
// void showAllAdmins();

// void endadmin();

// #endif



// #ifndef BOOK
// #define BOOK

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include<string.h>
// #include<stdio.h>
// #include<unistd.h>
// #include<stdlib.h>
// #include<stdbool.h>
// #include <sys/file.h>


// #define DUPLICATE_BOOK 2
// #define BOOK_NOT_FOUND 3

// #define bkfile "../files/book.dat"

// struct bookglobals{
//     int cur_bk_id;
//     int count;
//     int fd;
// } bookglob;

// struct book{//we assume one book has only one author
//     int id;
//     char title[100];
//     char author[100];
//     char category[40];
//     int copies_left;
// };

// bool bookequals(struct book u1, struct book u2);

// void initbook();

// void startbook();

// bool existsBook(struct book u1);

// void createBook(struct book u1, int *status);

// struct book * findBook(char * title, int * offset);

// void updateBook(char *title, struct book newbook, int * status);

// void deleteBook(char * title, int * status);

// void showbook(struct book u1);
    
// void showAllBooks();

// void endbook();

// #endif

// #ifndef BORROW
// #define BORROW

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include<string.h>
// #include<stdio.h>
// #include<unistd.h>
// #include<stdlib.h>
// #include<stdbool.h>
// #include <sys/file.h>

// #define DUPLICATE_BORROW 2
// #define BORROW_NOT_FOUND 3
// #define NOT_ENOUGH_COPIES 4


// #define borrfile "../files/borrow.dat"


// struct borrow{
//     int id;
//     char username[20];
//     char phone[20];
//     char title[100];
// };

// struct borrowglobals{
//     int cur_borr_id;
//     int count;
//     int fd;
// } borrowglob;


// bool borrowequals(struct borrow u1, struct borrow u2);

// void initborrow();

// void startborrow();

// bool existsBorrow(struct borrow u1);

// void borrowBook(struct borrow u1, int *status);

// struct borrow * findborrow(char * title, char * username, int * offset);

// void returnBook(char * title, char * username, int * status);

// void showborrow(struct borrow u1);
    
// void showAllBorrows();

// void endborrow();

// #endif




// // #endif



