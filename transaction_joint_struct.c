#include <stdio.h>
#include <stdbool.h>

struct transaction_joint{
char username1[1024];
char username2[1024];
int account_number;
long int date;
double credit;
double debit;
double rem_balance;
int type_account;
};
