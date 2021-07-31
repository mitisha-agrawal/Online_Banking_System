#include <stdio.h>
#include <stdbool.h>

struct Customer_joint{
char username1[1024];
char password1[1024];
char username2[1024];
char password2[1024];
int account_number;
int status;//0 for active,1 for inactive
int type_account;//1 for single ,2 for joint 
};
