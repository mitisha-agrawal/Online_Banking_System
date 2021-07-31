#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8008

void welcome(int sock);
void menu_customer(int sock,char username[1024],int type_account);
void menu_admin(int sock);
void deposit(int sock,char username[1024],int type_account);
void withdraw(int sock,char username[1024],int type_account);
void balance(int sock,char username[1024],int type_account);
void change_password(int sock,char username[1024],int type_account);
void account_statement(int sock,char username[1024],int type_account);
void view_details(int sock,char username[1024],int type_account);
void logout(int sock);
void add_account(int sock);
void delete_account(int sock);
void modify_account(int sock);
void search_account(int sock);

int main(int argc, char * argv[])
{
	char *ip = "127.0.0.1";
	/*if(argc==2){
		ip = argv[1];
	}*/
	int customer_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(customer_fd == -1){
		//printf("socket creation failed\n");
		exit(0);
	}
	struct sockaddr_in ca;
	ca.sin_family=AF_INET;
	ca.sin_port= htons(PORT);
	ca.sin_addr.s_addr = inet_addr(ip);
	if(connect(customer_fd, (struct sockaddr *)&ca, sizeof(ca))==-1){
		//printf("connect failed\n");
		exit(0);
	}
	//printf("connection established\n");

    welcome(customer_fd);

	//while(irctc(customer_fd)!=3);
	//close(customer_fd);

	return 0;
}
void welcome(int sock)
{   
    int buff,buff1[1];
    int opt;
    system("clear");
    printf("--------------------Welcome to banking system-------------------\n");
    printf("**OPTIONS**\n");
	printf("1. Single User Account \n");
	printf("2. Joint Account\n");
	printf("3. Admin\n");
    printf("4. Exit\n");
    printf("Enter type of Login by pressing corresponding number\n");
    scanf("%d", &buff);
    //printf("%d\n",buff);
    //printf("writing on socket\n");
    write(sock,&buff,sizeof(int));
    //printf("written on socket\n");
    if(buff==4)
    {   
        printf("------------------Thankyou for banking with us------------------\n ");
        exit(0);
    }
    char username[1024],password[1024];
    printf("Enter Your Username: \n");
	scanf("%s", username);
	strcpy(password,getpass("Enter password: \n"));
    write(sock, &username, sizeof(username));
	write(sock, &password, sizeof(password));

    read(sock,&opt,sizeof(int));
    if(opt==1)
    {   
        printf("Login Successful\n");
        printf("Please press enter to go forward\n");
        getchar();
        getchar();
        system("clear");
        write(1,"---------------------Welcome Customer---------------------\n",sizeof("---------------------Welcome Customer---------------------\n"));
        menu_customer(sock,username,buff);
    }
    else if(opt==0)
    {   
        printf("Login unsuccessful\n");
        printf("Press Enter to go back to login\n");
        getchar();
        getchar();
        welcome(sock);
    }
    else if(opt==2)
    {   
        printf("Login Successful\n");
        printf("Please press enter to go forward\n");
        getchar();
        getchar();
        system("clear");
        write(1,"---------------------Welcome Admin---------------------\n",sizeof("---------------------Welcome Admin---------------------\n"));
        menu_admin(sock);
    }
    else
    {
        printf("Login unsuccessful\n");
        printf("Press Enter to go back to login\n");
        getchar();
        getchar();
        welcome(sock);

    }
}
void menu_customer(int sock,char username[1024],int type_account)
{   
    int option;
    //system("clear");
    //write(1,"-----------Welcome Customer-------\n",sizeof("-----------Welcome Customer-------\n"));
    printf("\n");
    printf("**OPTIONS**\n");
    write(1,"1.Deposit\n",sizeof("1.Deposit\n"));
    write(1,"2.Withdraw\n",sizeof("2.Withdraw\n"));
    write(1,"3.Balance Enquiry\n",sizeof("3.Balance Enquiry\n"));
    write(1,"4.Password Change\n",sizeof("4.Password Change\n"));
    write(1,"5.View Details\n",sizeof("5.View Details\n"));
    write(1,"6.Account Statement\n",sizeof("6.Account Statement\n"));
    write(1,"7.Logout\n",sizeof("7.Logout\n"));
    printf("Enter the number corresponding to the action you want to take\n");
    scanf("%d",&option);
    write(sock,&option,sizeof(int));

    if(option==1) {deposit(sock,username,type_account);}
    else if(option==2) {withdraw(sock,username,type_account);}
    else if(option==3) {balance(sock,username,type_account);}
    else if(option==4) {change_password(sock,username,type_account);}
    else if(option==5) {view_details(sock,username,type_account);}
    else if(option==6) {account_statement(sock,username,type_account);}
    else if(option==7) {logout(sock);}
    else 
    {   
        printf("Invalid Entry\n");
        printf("Press Enter to try again\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }

}

void menu_admin(int sock)
{   
    int option;
    //write(1,"-----------Welcome Admin-------\n",sizeof("-----------Welcome Admin-------\n"));
    printf("\n");
    printf("**OPTIONS**\n");
    write(1,"1.Add Account\n",sizeof("1.Add Account\n"));
    write(1,"2.Delete Account\n",sizeof("2.Delete Account\n"));
    write(1,"3.Modify Account\n",sizeof("3.Modify Account\n"));
    write(1,"4.Search Account\n",sizeof("4.Search Account\n"));
    write(1,"5.Logout\n",sizeof("5.Logout\n"));
    printf("Enter the number corresponding to the action you want to take\n");
    scanf("%d",&option);
    write(sock,&option,sizeof(int));

    if(option==1) {add_account(sock);}
    else if(option==2) {delete_account(sock);}
    else if(option==3) {modify_account(sock);}
    else if(option==4) {search_account(sock);}
    else if(option==5) {logout(sock);}
    else 
    {   
        printf("Invalid Entry\n");
        printf("Press Enter to try again\n");
        getchar();
        getchar();
        system("clear");
        menu_admin(sock);
    }
}

void deposit(int sock,char username[1024],int type_account)
{   double dep;
    int check;
    write(sock,&type_account,sizeof(int));
    write(1,"Enter the amount to be deposited\n",sizeof("Enter the amount to be deposited\n"));
    scanf("%lf",&dep);
    printf("You are depositing amount=Rs%2.3f\n",dep);
    write(sock,&dep,sizeof(double));
    
    read(sock,&check,sizeof(int));
    if(check==1)
    {
        printf("Transaction Success\n");
        printf("Press Enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
    else if(check==0)
    {
        printf("Transaction failed\n");
        printf("Press Enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
}
void withdraw(int sock,char username[1024],int type_account)
{
    double with;
    int check;
    write(sock,&type_account,sizeof(int));
    write(1,"Enter the amount to be withdrawn\n",sizeof("Enter the amount to be withdrawn\n"));
    scanf("%lf",&with);
    printf("You are withdrawing amount=Rs%2.3f\n",with);
    write(sock,&with,sizeof(double));
    
    read(sock,&check,sizeof(int));
    if(check==1)
    {
        printf("Transaction Success\n");
        printf("Press Enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
    else if(check==0)
    {
        printf("Transaction failed Insufficient balance\n");
        printf("Press Enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
}
void balance(int sock,char username[1024],int type_account)
{
    double bal;
    write(sock,&type_account,sizeof(int));
    //printf("seeing balance\n");
    read(sock,&bal,sizeof(double));
    printf("The Balance is :Rs%2.3f\n",bal);
    printf("Press Enter to go back to Options\n");
    getchar();
    getchar();
    system("clear");
    menu_customer(sock,username,type_account);
}
void change_password(int sock,char username[1024],int type_account)
{   
    char password_new[1024];
    int check;
    write(sock,&type_account,sizeof(int));
    strcpy(password_new,getpass("Type the new password: \n"));
    write(sock, &password_new, sizeof(password_new));
    read(sock,&check,sizeof(int));
    if(check==1)
    {
        printf("Password updation Success\n");
        printf("User press enter and login with new password\n");
        getchar();
        getchar();
        system("clear");
        welcome(sock);
        //exit(0);
    }
    else if(check==0)
    {
        printf("Password updation failed\n");
        printf("Press enter to go back to Options and try again\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
}
void view_details(int sock,char username[1024],int type_account)
{
    write(sock,&type_account,sizeof(int));
    char view[10000];
    read(sock,&view,sizeof(view));
    printf("Your Account Details are-\n");
    printf("%s \n",view);
    printf("Press enter to go back to Options\n");
    getchar();
    getchar();
    system("clear");
    menu_customer(sock,username,type_account);
}
void account_statement(int sock,char username[1024],int type_account)
{
    write(sock,&type_account,sizeof(int));
    int f;
    read(sock,&f,sizeof(int));
    if(f==1)
    {
        char stm[10000];
        read(sock,&stm,sizeof(stm));
        printf("Your Account Details are-\n");
        printf("%s \n",stm);
        printf("Press enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account);
    }
    else if(f==0)
    {
        char stm[10000];
        read(sock,&stm,sizeof(stm));
        printf("Your Account Details are-\n");
        printf("You have not done any transactions yet!!!\n");
        printf("Press enter to go back to Options\n");
        getchar();
        getchar();
        system("clear");
        menu_customer(sock,username,type_account); 
    }
}
void logout(int sock)
{
    printf("------------------Thankyou for banking with us------------------\n ");
    exit(0);    
}
void add_account(int sock)
{
    int op_add;
    write(1,"Which type of account you want to add \n",sizeof("Which type of account you want to add \n"));
    write(1,"1.Single Account\n",sizeof("1.Single Account\n"));
    write(1,"2.Joint Account\n",sizeof("2.Joint Account\n"));
    write(1,"3.Exit\n",sizeof("3.Exit\n"));
    scanf("%d",&op_add);
    write(sock,&op_add,sizeof(int));

    

    if(op_add==1)
    {
        char username[1024],password[1024];
        int type_account=1;
        write(1,"Enter Username:\n",sizeof("Enter Username:\n"));
        scanf("%s", username);
	    strcpy(password,getpass("Enter password: \n"));
        
        write(sock, &username, sizeof(username));
	    write(sock, &password, sizeof(password));
        
        int flag_add;
        read(sock,&flag_add,sizeof(int));
        if(flag_add==1)
        {
            write(1,"Username Exists try another one\n",sizeof("Username Exists try another one\n"));
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            add_account(sock);
        }
        int j;
        read(sock,&j,sizeof(int));
        if(j==0)
        {
            printf("Addition Successful\n");
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(j==1)
        {
            printf("Addition Failed");
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
       
    }
    else if(op_add==2)
    {
        char username1[1024],password1[1024];
        char username2[1024],password2[1024];
        write(1,"Enter Username1:\n",sizeof("Enter Username1:\n"));
        scanf("%s", username1);
	    strcpy(password1,getpass("Enter password1: \n"));
        write(1,"Enter Username2:\n",sizeof("Enter Username2:\n"));
        scanf("%s", username2);
	    strcpy(password2,getpass("Enter password2: \n"));
        write(sock, &username1, sizeof(username1));
	    write(sock, &password1, sizeof(password1));
        write(sock, &username2, sizeof(username2));
	    write(sock, &password2, sizeof(password2));
        int flag_add;
        read(sock,&flag_add,sizeof(int));
        if(flag_add==1)
        {
            write(1,"Username Exists try another one\n",sizeof("Username Exists try another one\n"));
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            add_account(sock);
        }
        int j;
        read(sock,&j,sizeof(int));
        if(j==0)
        {
            printf("Addition Successful\n");
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(j==1)
        {
            printf("Addition Failed");
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
    }
    else if(op_add==3)
    {   
        printf("Press enter to go back to Options and try again\n");
        getchar();
        getchar();
        system("clear");
        menu_admin(sock);
    }
    else
    {
        printf("Invalid Entry\n");
    }
}
void delete_account(int sock)
{
    int type_account;
    write(1,"Which type of account you want to delete \n",sizeof("Which type of account you want to delete \n"));
    write(1,"1.Single Account\n",sizeof("1.Single Account\n"));
    write(1,"2.Joint Account\n",sizeof("2.Joint Account\n"));
    write(1,"3.Exit\n",sizeof("3.Exit\n"));
    scanf("%d",&type_account);
    write(sock,&type_account,sizeof(int));

    if(type_account==1)
    {       
        char username[1024];
        write(1,"Enter Username:\n",sizeof("Enter Username:\n"));
        scanf("%s", username);
	    write(sock, &username, sizeof(username));
        int flag_del;
        read(sock,&flag_del,sizeof(int));
        if(flag_del==1)
        {
            write(1,"Account Deleted Successfully\n",sizeof("Account Deleted Successfully\n"));
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(flag_del==0)
        {
            write(1,"Account not Deleted \n",sizeof("Account not Deleted \n"));
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
    }
    else if(type_account==2)
    {
        char username[1024];
        write(1,"Enter Username:\n",sizeof("Enter Username:\n"));
        scanf("%s", username);
	    write(sock, &username, sizeof(username));
	    int flag_del1;
        read(sock,&flag_del1,sizeof(int));
        if(flag_del1==1)
        {
            write(1,"Account Deleted Successfully\n",sizeof("Account Deleted Successfully\n"));
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(flag_del1==0)
        {
            write(1,"Account not Deleted \n",sizeof("Account not Deleted \n"));
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        
    }
    else if(type_account==3)
    {   
        system("clear");
        menu_admin(sock);
    }
    else
    {
        printf("Invalid Entry\n");
    }
}
void modify_account(int sock)
{   
    int type_account;
    write(1,"Which type of account you want to delete \n",sizeof("Which type of account you want to delete \n"));
    write(1,"1.Single Account\n",sizeof("1.Single Account\n"));
    write(1,"2.Joint Account\n",sizeof("2.Joint Account\n"));
    write(1,"3.Exit\n",sizeof("3.Exit\n"));
    scanf("%d",&type_account);
    write(sock,&type_account,sizeof(int));

    if(type_account==1)
    {       
        char username[1024];
        write(1,"Enter Username for which you want to change password:\n",sizeof("Enter Username for which you want to change password:\n"));
        scanf("%s", username);
	    write(sock, &username, sizeof(username));
        char password_new[1024];
        strcpy(password_new,getpass("Type the new password: \n"));
        write(sock, &password_new, sizeof(password_new));
        int flag_del;
        read(sock,&flag_del,sizeof(int));
        if(flag_del==1)
        {
            write(1,"Password Changed Successfully\n",sizeof("Password Changed Successfully\n"));
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(flag_del==0)
        {
            write(1,"Password change unsuccess \n",sizeof("Password change unsuccess \n"));
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
    }
    else if(type_account==2)
    {
        char username[1024];
        write(1,"Enter Username:\n",sizeof("Enter Username:\n"));
        scanf("%s", username);
	    write(sock, &username, sizeof(username));
        char password_new[1024];
        strcpy(password_new,getpass("Type the new password: \n"));
        write(sock, &password_new, sizeof(password_new));

	    int flag_del1;
        read(sock,&flag_del1,sizeof(int));
        if(flag_del1==1)
        {
            write(1,"Password Changed Successfully\n",sizeof("Password Changed Successfully\n"));
            printf("Press enter to go back to Options\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
        else if(flag_del1==0)
        {
            write(1,"Password change unsuccess \n",sizeof("Password change unsuccess \n"));
            printf("Press enter to go back to Options and try again\n");
            getchar();
            getchar();
            system("clear");
            menu_admin(sock);
        }
    }
    else if(type_account==3)
    {
        system("clear");
        menu_admin(sock);
    }
    else
    {
        printf("Invalid Entry\n");
    }
}
void search_account(int sock)
{
    char username[1024];
    write(1,"Enter Username of which you want to search account:\n",sizeof("Enter Username of which you want to search account:\n"));
    scanf("%s", username);
	write(sock, &username, sizeof(username));
    char store[10000];
    read(sock,&store,sizeof(store));
    printf("Your Account Details are-\n");
    printf("%s \n",store);
    printf("Press enter to go back to menu");
    getchar();
    getchar();
    system("clear");
    menu_admin(sock);
}