#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "customer_joint_struct.c"
#include "customer_single_struct.c"
#include "account_number_joint_increment.c"
#include "account_number_single_increment.c"
#include "transaction_single_struct.c"
#include "transaction_joint_struct.c"

#define PORT 8008


void signin(int sock);
void service_login_customer(int sock,char username[1024]);
void service_admin(int sock);
void deposit(int sock,char username[1024]);
void withdraw(int sock,char username[1024]);
void balance(int sock,char username[1024]);
void change_password(int sock,char username[1024]);
void view_details(int sock,char username[1024]);
void account_statement(int sock,char username[1024]);
void logout(int sock);
void add_account(int sock);
void delete_account(int sock);
void modify_account(int sock);
void search_account(int sock);


int main()
{   
    
    printf("Initializing connection...\n");
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1)
    {
		printf("Socket creation failed\n");
		perror("");
	}
	int optval = 1;
	int optlen = sizeof(optval);
	
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, optlen)==-1){
		printf("Set socket options failed\n");
		perror("");
	}
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(PORT);
	
	printf("Binding socket...\n");
	if(bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr))==-1){
		printf("Binding port failed\n");
		perror("");
	}
	
	if(listen(sockfd, 100)==-1){
		printf("Listen failed\n");
		perror("");
	}
	printf("Listening...\n");
	while(1){
		int fd_connected_socket;
		if((fd_connected_socket = accept(sockfd, (struct sockaddr *)NULL, NULL))==-1){
			printf("Connection error\n");
			perror("");
		}
		
		if(fork()==0){
            printf("Forked\n");
			signin(fd_connected_socket);
			exit(1);
		}
	}
	close(sockfd);
	printf("Connection closed!\n");
	return 0;
} 

void signin(int sock)
{   //enter the credentials is on client screen
    //enter type of account number 1 single 2 joint
    int buff2; 
    printf("Waiting for type of login to be selected\n");
    read(sock,&buff2,sizeof(int));
    //printf("written on socket\n");
    printf("Type of login selected is %d\n",buff2);
    if(buff2==4)
    {   
        main();
    }
    struct Customer_joint cjs,temp1;
    struct Customer_single css,temp2;

    //int type_account=buff2;
    int flag=0,fd;
    char username[1024],password[1024];

    //read(sock, &type_account, sizeof(type_account));
    if(buff2==1)
    {   
        //search in css
        read(sock, &username, sizeof(username));
	    read(sock, &password, sizeof(password));
        if((fd=open("db_customer_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        char buffer1_signin[1024],buffer2_signin[1024],buffer3_signin[1024],buffer4_signin[1024];
        strcpy(buffer1_signin,username);
        strcpy(buffer2_signin,password);
        while((read(fd, &temp2, sizeof(temp2)))>0) 
	    {   
            strcpy(buffer3_signin,temp2.username);
            strcpy(buffer4_signin,temp2.password);
		    if((strcmp(buffer3_signin,buffer1_signin)==0) && (strcmp(buffer4_signin,buffer2_signin)==0))
            {   
                if(temp2.status==0)
                {
                printf("Username and password matched and account is active\n");
                flag=1;
                break;
                }
                else
                {   
                    printf("Account does not exist it is inactive\n");
                    break;
                }
                
            }
            else
            {   
                //printf("%s\n",temp2.username);
                printf("Entries did not match\n");
            }
        }
        if(flag==1)
        {   
            int i=1;
            write(sock,&i,sizeof(int));
            close(fd);
            service_login_customer(sock,username);
            
        }
        else
        {
            if(flag==0)
            {
                int i=0;
                write(sock,&i,sizeof(int));
                close(fd);
                signin(sock);
            }
        }
    }
    else if(buff2==2)
    {
        //search in cjs
        read(sock, &username, sizeof(username));
	    read(sock, &password, sizeof(password));
        if((fd=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        char buffer1_signin[1024],buffer2_signin[1024],buffer3_signin[1024],buffer4_signin[1024],buffer5_signin[1024],buffer6_signin[1024];
        strcpy(buffer1_signin,username);
        strcpy(buffer2_signin,password);
        while((read(fd, &temp1, sizeof(temp1)))>0)
	    {
            strcpy(buffer3_signin,temp1.username1);
            strcpy(buffer4_signin,temp1.password1);
            strcpy(buffer5_signin,temp1.username2);
            strcpy(buffer6_signin,temp1.password2);
		    if((strcmp(buffer3_signin,buffer1_signin)==0)&& (strcmp(buffer4_signin,buffer2_signin)==0))
            {   
                if(temp1.status==0)
                {
                printf("Username and password matched and account is active\n");
                flag=1;
                break;
                }
                else
                {
                    printf("Account does not exist it is inactive\n");
                    break;
                }
                
            }
            else if((strcmp(buffer5_signin,buffer1_signin)==0)&& (strcmp(buffer6_signin,buffer2_signin)==0))
            {
                if(temp1.status==0)
                {
                printf("Username and password matched and account is active\n");
                flag=1;
                break;
                }
                else
                {
                    printf("Account does not exist it is inactive\n");
                    break;
                }
            }
            else
            {
                printf("User name does not exist\n");
            }
        }
        if(flag==1)
        {   
            int i=1;
            write(sock,&i,sizeof(int));
            close(fd);
            service_login_customer(sock,username);
            
        }
        else
        {
            if(flag==0)
            {
                int i=0;
                write(sock,&i,sizeof(int));
                close(fd);
                signin(sock);
            }
        }
        
        
    }
    else if(buff2==3)
    {
        //search in admin
        read(sock, &username, sizeof(username));
        read(sock, &password, sizeof(password));
        printf("%s\n",username);
        printf("%s\n",password);
	    
         if((strcmp(username,"admin")==0) && (strcmp(password,"12345")==0))
        {   
            int i=2;
            write(sock,&i,sizeof(int));
            printf("Admin has logged in\n");
            service_admin(sock);
        }
    }
    
    else
    {
        //print that it is not valid exit
        printf("Invalid Details\n");
        signin(sock);
    }
}
void service_login_customer(int sock,char username[1024])
{
    int buff1; 
    read(sock,&buff1,sizeof(int));
    if(buff1==1) {deposit(sock,username);}
    else if(buff1==2) {withdraw(sock,username);}
    else if(buff1==3) {balance(sock,username);}
    else if(buff1==4) {change_password(sock,username);}
    else if(buff1==5) {view_details(sock,username);}
    else if(buff1==6) {account_statement(sock,username);}
    else if(buff1==7) {logout(sock);}
    else {printf("Invalid Entry\n");}
}
void service_admin(int sock)
{
    int buff1; 
    read(sock,&buff1,sizeof(int));
    if(buff1==1) {add_account(sock);}
    else if(buff1==2) {delete_account(sock);}
    else if(buff1==3) {modify_account(sock);}
    else if(buff1==4) {search_account(sock);}
    else if(buff1==5) {logout(sock);}
    else {printf("Invalid Entry\n");}
    
}
void deposit(int sock,char username[1024])
{   int type_account;
    double dep;
    char buff1_dep[1024];
    read(sock,&type_account,sizeof(int));
    printf("Type_account is %d\n",type_account);
    read(sock,&dep,sizeof(double));
    printf("Amount to be deposited is %2.3f\n",dep);
    strcpy(buff1_dep,username);
    
    
    
    if(type_account==1)
    {   int fd_deposit,fd_all_deposit;
        char buff2_dep[1024];
        struct flock lock;
        struct transaction_single dep_single;
        if((fd_deposit=open("transaction_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_deposit,0,SEEK_SET);
        fd_all_deposit=open("transaction_all_single.txt",O_RDWR);
        lseek(fd_all_deposit,0,SEEK_END);
        int p=1,check=0;
        while((read(fd_deposit,&dep_single,sizeof(dep_single)))>0)
        {
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(dep_single);        
            lock.l_len    = sizeof(dep_single);        
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_deposit,F_SETLKW,&lock);
            fcntl(fd_all_deposit,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_dep,dep_single.username);
            //printf("Reached if statement\n");
            //printf("The username checking right now is %s\n",buff2_dep);
            if(strcmp(buff2_dep,buff1_dep)==0)
            {
                dep_single.credit=dep;
                printf("Deposited amount is%2.3f\n",dep_single.credit);
                //sleep(15);
                dep_single.debit=0.0;
                dep_single.rem_balance+=dep;
                printf("Updated Balance is%2.3f\n",dep_single.rem_balance);
                time_t now_dep= time(NULL);
                if (now_dep == -1) 
                {
                    puts("The time() function failed");
                }
                //printf("%ld\n", now_dep);
                dep_single.date=now_dep;
                //printf("%ld\n", dep_single.date);
                lseek(fd_deposit,-1*sizeof(dep_single),SEEK_CUR);
                int c=write(fd_deposit,&dep_single,sizeof(dep_single));
                write(fd_all_deposit,&dep_single,sizeof(dep_single));
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_deposit,F_SETLK,&lock);
                fcntl(fd_all_deposit,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c>0)
                {
                    check=1;
                    printf("Trancastion updated successfully\n");
                    write(sock,&check,sizeof(int));
                }
                break;
            }
            else
            {
                p++;
            }
        }
        if(check==0)
        {   
            write(sock,&check,sizeof(int));
            lock.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_deposit,F_SETLK,&lock);
            fcntl(fd_all_deposit,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_deposit);
            close(fd_all_deposit);
            service_login_customer(sock,username);
        }
        else if(check==1)
        {
            close(fd_deposit);
            close(fd_all_deposit);
            service_login_customer(sock,username);
        }
        
    }
    else if(type_account==2)
    {   
        int fd_deposit1,fd_all_deposit1;
        char buff3_dep[1024],buff4_dep[1024];
        struct flock lk;
        struct transaction_joint dep_joint;
        if((fd_deposit1=open("transaction_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_deposit1,0,SEEK_SET);
        fd_all_deposit1=open("transaction_all_joint.txt",O_RDWR);
        lseek(fd_all_deposit1,0,SEEK_END);
        int pp=1,check1=0;
        while((read(fd_deposit1,&dep_joint,sizeof(dep_joint)))>0)
        {
            lk.l_type   = F_WRLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(dep_joint);        
            lk.l_len    = sizeof(dep_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_deposit1,F_SETLKW,&lk);
            fcntl(fd_all_deposit1,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_dep,dep_joint.username1);
            strcpy(buff4_dep,dep_joint.username2);
            //printf("Reached if statement\n");
            //printf("The username1 checking right now is %s\n",buff3_dep);
            //printf("The username2 checking right now is %s\n",buff4_dep);

            if((strcmp(buff3_dep,buff1_dep)==0) || (strcmp(buff4_dep,buff1_dep)==0))
            {
                dep_joint.credit=dep;
                dep_joint.rem_balance+=dep;
                dep_joint.debit=0.0;
                time_t now_dep1 = time(NULL);
                if (now_dep1 == -1) 
                {
                    puts("The time() function failed");
                }
                //printf("%ld\n", now_dep1);
                dep_joint.date=now_dep1;
                //printf("%ld\n", dep_joint.date);
                lseek(fd_deposit1,-1*sizeof(dep_joint),SEEK_CUR);
                int c1=write(fd_deposit1,&dep_joint,sizeof(dep_joint));
                write(fd_all_deposit1,&dep_joint,sizeof(dep_joint));
                
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_deposit1,F_SETLK,&lk);
                fcntl(fd_all_deposit1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Trancastion updated successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        if(check1==0)
        {   printf("Trancastion updated unsuccess\n");
            write(sock,&check1,sizeof(int));
            lk.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_deposit1,F_SETLK,&lk);
            fcntl(fd_all_deposit1,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_deposit1);
            close(fd_all_deposit1);
            service_login_customer(sock,username);
        }
        else if(check1==1)
        {
            close(fd_deposit1);
            close(fd_all_deposit1);
            service_login_customer(sock,username);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void withdraw(int sock,char username[1024])
{
    int type_account;
    double with;
    char buff1_with[1024];
    read(sock,&type_account,sizeof(int));
    printf("Type_account is %d\n",type_account);
    read(sock,&with,sizeof(double));
    printf("Amount to be withdrawn is %2.3f\n",with);
    strcpy(buff1_with,username);
    
    
    
    if(type_account==1)
    {   int fd_withdraw,fd_all_withdraw;
        char buff2_with[1024];
        struct flock lock;
        struct transaction_single with_single;
        if((fd_withdraw=open("transaction_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_withdraw,0,SEEK_SET);
        fd_all_withdraw=open("transaction_all_single.txt",O_RDWR);
        lseek(fd_all_withdraw,0,SEEK_END);
        int p=1,check=0;
        while((read(fd_withdraw,&with_single,sizeof(with_single)))>0)
        {
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(with_single);        
            lock.l_len    = sizeof(with_single);        
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_withdraw,F_SETLKW,&lock);
            fcntl(fd_all_withdraw,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_with,with_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_with);
            if(strcmp(buff2_with,buff1_with)==0)
            {   
                if(with_single.rem_balance>with)
                {
                    with_single.debit=with;
                    printf("Withdrawn amount is%2.3f\n",with_single.debit);
                    with_single.rem_balance-=with;
                    printf("Updated Balance is%2.3f\n",with_single.rem_balance);
                    with_single.credit=0.0;
                    time_t now_with = time(NULL);
                    if (now_with == -1) 
                    {
                        puts("The time() function failed");
                    }
                    //printf("%ld\n", now_with);
                    with_single.date=now_with;
                    //printf("%ld\n", with_single.date);
                    lseek(fd_withdraw,-1*sizeof(with_single),SEEK_CUR);
                    int c=write(fd_withdraw,&with_single,sizeof(with_single));
                    write(fd_all_withdraw,&with_single,sizeof(with_single));
                    if(c>0)
                    {
                        check=1;
                        printf("Trancastion updated successfully\n");
                        write(sock,&check,sizeof(int));
                    }
                }
                else
                {   write(sock,&check,sizeof(int));
                    printf("Balance Insufficient\n");
                }
                
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_withdraw,F_SETLK,&lock);
                fcntl(fd_all_withdraw,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else
            {
                p++;
            }
        }
        if(check==0)
        {   
            //write(sock,&check,sizeof(int));
            lock.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_withdraw,F_SETLK,&lock);
            fcntl(fd_all_withdraw,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_withdraw);
            close(fd_all_withdraw);
            service_login_customer(sock,username);
        }
        else if(check==1)
        {
            close(fd_withdraw);
            close(fd_all_withdraw);
            service_login_customer(sock,username);
        }
        
    }
    else if(type_account==2)
    {   
        int fd_withdraw1,fd_all_withdraw1;
        char buff3_with[1024],buff4_with[1024];
        struct flock lk;
        struct transaction_joint with_joint;
        if((fd_withdraw1=open("transaction_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_withdraw1,0,SEEK_SET);
        fd_all_withdraw1=open("transaction_all_joint.txt",O_RDWR);
        lseek(fd_all_withdraw1,0,SEEK_END);
        int pp=1,check1=0;
        while((read(fd_withdraw1,&with_joint,sizeof(with_joint)))>0)
        {
            lk.l_type   = F_WRLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(with_joint);        
            lk.l_len    = sizeof(with_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_withdraw1,F_SETLKW,&lk);
            fcntl(fd_all_withdraw1,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_with,with_joint.username1);
            strcpy(buff4_with,with_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username1 checking right now is %s\n",buff3_with);
            //printf("the username2 checking right now is %s\n",buff4_with);

            if((strcmp(buff3_with,buff1_with)==0) || (strcmp(buff4_with,buff1_with)==0))
            {   
                if(with_joint.rem_balance>with)
                {
                    with_joint.debit=with;
                    printf("Withdrawn amount is%2.3f\n",with_joint.debit);
                    with_joint.rem_balance-=with;
                    printf("Updated Balance is%2.3f\n",with_joint.rem_balance);
                    with_joint.credit=0.0;
                    time_t now_with1= time(NULL);
                    if (now_with1 == -1) 
                    {
                        puts("The time() function failed");
                    }
                    //printf("%ld\n", now_with1);
                    with_joint.date=now_with1;
                    //printf("%ld\n", with_joint.date);
                    lseek(fd_withdraw1,-1*sizeof(with_joint),SEEK_CUR);
                    int c1=write(fd_withdraw1,&with_joint,sizeof(with_joint));
                    write(fd_all_withdraw1,&with_joint,sizeof(with_joint));
                    if(c1>0)
                {
                    check1=1;
                    printf("Trancastion updated successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                }
                else
                {   write(sock,&check1,sizeof(int));
                    printf("Balance Insufficient\n");
                }
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_withdraw1,F_SETLK,&lk);
                fcntl(fd_all_withdraw1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                
                break;
            }
            else
            {
                pp++;
            }
        }
        if(check1==0)
        {
            //write(sock,&check1,sizeof(int));
            lk.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_withdraw1,F_SETLK,&lk);
            fcntl(fd_all_withdraw1,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_withdraw1);
            close(fd_all_withdraw1);
            service_login_customer(sock,username);
        }
        else if(check1==1)
        {
            close(fd_withdraw1);
            close(fd_all_withdraw1);
            service_login_customer(sock,username);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void balance(int sock,char username[1024])
{   
    int type_account;
    read(sock,&type_account,sizeof(int));
    printf("Type_acount is %d\n",type_account);
    char buff1_bal[1024];
    strcpy(buff1_bal,username);
    if(type_account==1)
    {   
        int fd_bal;
        char buff2_bal[1024];
        struct flock lk_bal;
        struct transaction_single bal_single;
        if((fd_bal=open("transaction_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_bal,0,SEEK_SET);
        int p_bal=1,check_bal=0;
        while((read(fd_bal,&bal_single,sizeof(bal_single)))>0)
        {
            lk_bal.l_type   = F_WRLCK;  
            lk_bal.l_whence = SEEK_SET; 
            lk_bal.l_start  = (p_bal-1)*sizeof(bal_single);        
            lk_bal.l_len    = sizeof(bal_single);        
            lk_bal.l_pid    = getpid(); 
            fcntl(fd_bal,F_SETLKW,&lk_bal);
            strcpy(buff2_bal,bal_single.username);
            //printf("reached if statement\n");
            //printf("%s\n",buff2_bal);
            if((strcmp(buff2_bal,buff1_bal))==0)
            {   
                double bal;
                bal=bal_single.rem_balance;
                printf("Balance is %2.3f\n",bal);
                int c=write(sock,&bal,sizeof(double));
                lk_bal.l_type=F_UNLCK;
                fcntl(fd_bal,F_SETLK,&lk_bal);
                if(c>0)
                {
                    check_bal=1;
                    printf("Balance sent successfully\n");
                    //write(sock,&check_bal,sizeof(int));
                }
                break;
            }
            else
            {   
                //printf("Not found username yet\n");
                p_bal++;
            }
        }
        if(check_bal==0)
        {   
            //printf("Reached check_bal=0\n");
            //write(sock,&check_bal,sizeof(int));
            lk_bal.l_type=F_UNLCK;
            fcntl(fd_bal,F_SETLK,&lk_bal);
            close(fd_bal);
            service_login_customer(sock,username);
        }
        else if(check_bal==1)
        {   
            //printf("Reached check_bal=1\n");
            close(fd_bal);
            service_login_customer(sock,username);
        }
        
    }
    else if(type_account==2)
    {   
        int fd_bal1;
        char buff3_bal[1024],buff4_bal[1024];
        struct flock lk_bal1;
        struct transaction_joint bal_joint;
        if((fd_bal1=open("transaction_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_bal1,0,SEEK_SET);
        int pp_bal=1,check1_bal=0;
        while((read(fd_bal1,&bal_joint,sizeof(bal_joint)))>0)
        {
            lk_bal1.l_type   = F_WRLCK;  
            lk_bal1.l_whence = SEEK_SET; 
            lk_bal1.l_start  = (pp_bal-1)*sizeof(bal_joint);        
            lk_bal1.l_len    = sizeof(bal_joint);        
            lk_bal1.l_pid    = getpid(); 
            fcntl(fd_bal1,F_SETLKW,&lk_bal1);
            strcpy(buff3_bal,bal_joint.username1);
            strcpy(buff4_bal,bal_joint.username2);
            //printf("reached if statement\n");
            if(((strcmp(buff3_bal,buff1_bal))==0) || ((strcmp(buff4_bal,buff1_bal))==0))
            {   
                double bal1;
                bal1=bal_joint.rem_balance;
                int c1=write(sock,&bal1,sizeof(double));
                printf("Balance is %2.3f\n",bal1);
                lk_bal1.l_type=F_UNLCK;
                fcntl(fd_bal1,F_SETLK,&lk_bal1);
                if(c1>0)
                {
                    check1_bal=1;
                    printf("Balance sent successfully\n");
                    //write(sock,&check1,sizeof(int));
                }
                break;
            }
            else
            {
                pp_bal++;
            }
        }
        if(check1_bal==0)
        {
            //write(sock,&check1,sizeof(int));
            lk_bal1.l_type=F_UNLCK;
            fcntl(fd_bal1,F_SETLK,&lk_bal1);
            close(fd_bal1);
            service_login_customer(sock,username);
        }
        else if(check1_bal==1)
        {
            close(fd_bal1);
            service_login_customer(sock,username);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void change_password(int sock,char username[1024])
{   
    int type_account;
    char pass_new[1024];
    char buff1_pass[1024];
    read(sock,&type_account,sizeof(int));
    printf("Type_Account is:%d\n",type_account);
    read(sock, &pass_new, sizeof(pass_new));
    printf("New password is %s\n",pass_new);
    strcpy(buff1_pass,username);
    if(type_account==1)
    {   
        int fd_pass;
        char buff2_pass[1024];
        struct flock lock;
        struct Customer_single pass_single;
        if((fd_pass=open("db_customer_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_pass,0,SEEK_SET);
        int p=1,check=0;
        while((read(fd_pass,&pass_single,sizeof(pass_single)))>0)
        {
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(pass_single);        
            lock.l_len    = sizeof(pass_single);        
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_pass,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_pass,pass_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_pass);
            if(strcmp(buff2_pass,buff1_pass)==0)
            {
                strcpy(pass_single.password,pass_new);
                printf("New password set is %s\n",pass_single.password);
                lseek(fd_pass,-1*sizeof(pass_single),SEEK_CUR);
                int c=write(fd_pass,&pass_single,sizeof(pass_single));
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_pass,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c>0)
                {
                    check=1;
                    printf("Password updated successfully\n");
                    write(sock,&check,sizeof(int));
                }
                break;
            }
            else
            {
                p++;
            }
        }
        if(check==0)
        {   
            write(sock,&check,sizeof(int));
            printf("Password not changed\n");
            lock.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_pass,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_pass);
            service_login_customer(sock,username);
        }
        else if(check==1)
        {
            close(fd_pass);
            //main();
            signin(sock);
        }
    }
    else if(type_account==2)
    {   
        int fd_pass1;
        char buff3_pass[1024],buff4_pass[1024];
        struct flock lk;
        struct Customer_joint pass_joint;
        if((fd_pass1=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_pass1,0,SEEK_SET);
        int pp=1,check1=0;
        while((read(fd_pass1,&pass_joint,sizeof(pass_joint)))>0)
        {
            lk.l_type   = F_WRLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(pass_joint);        
            lk.l_len    = sizeof(pass_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_pass1,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_pass,pass_joint.username1);
            strcpy(buff4_pass,pass_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff3_pass);
            //printf("the username checking right now is %s\n",buff4_pass);

            if(strcmp(buff3_pass,buff1_pass)==0)
            {
                strcpy(pass_joint.password1,pass_new);
                printf("New password set is%s\n",pass_joint.password1);
                lseek(fd_pass1,-1*sizeof(pass_joint),SEEK_CUR);
                int c1=write(fd_pass1,&pass_joint,sizeof(pass_joint));
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_pass1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Password updated successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else if(strcmp(buff4_pass,buff1_pass)==0)
            {
                strcpy(pass_joint.password2,pass_new);
                printf("New password set is%s\n",pass_joint.password2);
                lseek(fd_pass1,-1*sizeof(pass_joint),SEEK_CUR);
                int c1=write(fd_pass1,&pass_joint,sizeof(pass_joint));
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_pass1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Password updated successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        if(check1==0)
        {
            write(sock,&check1,sizeof(int));
            lk.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_pass1,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_pass1);
            service_login_customer(sock,username);
        }
        else if(check1==1)
        {
            close(fd_pass1);
            signin(sock);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void view_details(int sock,char username[1024])
{   
    int type_account;
    char buff1_view[1024];
    read(sock,&type_account,sizeof(int));
    printf("Type_Account is:%d\n",type_account);
    strcpy(buff1_view,username);
    if(type_account==1)
    {   
        int fd_view,fd_view1;
        char buff2_view[1024];
        char store[10000];
        struct flock lock;
        struct Customer_single view_single;
        struct transaction_single view1_single;
        if((fd_view=open("db_customer_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_view,0,SEEK_SET);
        fd_view1=open("transaction_single.txt",O_RDWR);
        lseek(fd_view1,0,SEEK_SET);

        int p=1,check=0;
        while((read(fd_view,&view_single,sizeof(view_single)))>0)
        {
            lock.l_type   = F_RDLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(view_single);        
            lock.l_len    = sizeof(view_single);       
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_view,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_view,view_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_view);
            if(strcmp(buff2_view,buff1_view)==0)
            {   
                strcat(store,"*Username is-");
                strcat(store,username);
                strcat(store,"\n");
                strcat(store,"*Password is-");
                strcat(store,view_single.password);
                strcat(store,"\n");
                char charray_acc[20];
                sprintf(charray_acc, "%d", view_single.account_number);
                strcat(store,"*Account Number is-");
                strcat(store,charray_acc);
                strcat(store,"\n");
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else
            {
                p++;
            }
        }
        while((read(fd_view1,&view1_single,sizeof(view1_single)))>0)
        {
            lock.l_type   = F_RDLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(view1_single);        
            lock.l_len    = sizeof(view1_single);       
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_view1,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_view,view1_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_view);
            if(strcmp(buff2_view,buff1_view)==0)
            {   
                char charray[20];
                sprintf(charray, "%2.3f", view1_single.rem_balance);
                strcat(store,"*Balance is- Rs");
                strcat(store,charray);
                strcat(store,"\n");
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view1,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else
            {
                p++;
            }
        }
        write(sock,&store,sizeof(store));
        lock.l_type=F_UNLCK;
        printf("Going for unlock\n");
        int fc=fcntl(fd_view,F_SETLK,&lock);
        fcntl(fd_view1,F_SETLK,&lock);
        if(fc==0)
        {
            printf("Lock released\n");
        }
            close(fd_view);
            close(fd_view1);
            service_login_customer(sock,username);
    }
    else if(type_account==2)
    {   
        int fd_view2,fd_view3;
        char buff3_view[1024],buff4_view[1024];
        char store1[10000];
        struct flock lk;
        struct Customer_joint view_joint;
        struct transaction_joint view1_joint;
        if((fd_view2=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_view2,0,SEEK_SET);
        fd_view3=open("transaction_joint.txt",O_RDWR);
        lseek(fd_view3,0,SEEK_SET);

        int pp=1,check1=0;
        while((read(fd_view2,&view_joint,sizeof(view_joint)))>0)
        {
            lk.l_type   = F_RDLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(view_joint);        
            lk.l_len    = sizeof(view_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_view2,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_view,view_joint.username1);
            strcpy(buff4_view,view_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff3_pass);
            //printf("the username checking right now is %s\n",buff4_pass);

            if(strcmp(buff3_view,buff1_view)==0)
            {   
                strcat(store1,"*Username is-");
                strcat(store1,view_joint.username1);
                strcat(store1,"\n");
                strcat(store1,"*Password is-");
                strcat(store1,view_joint.password1);
                strcat(store1,"\n");
                char charray1_acc[20];
                sprintf(charray1_acc, "%d", view_joint.account_number);
                strcat(store1,"*Account Number is-");
                strcat(store1,charray1_acc);
                strcat(store1,"\n");
                
                
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else if(strcmp(buff4_view,buff1_view)==0)
            {
                strcat(store1,"*Username is-");
                strcat(store1,view_joint.username2);
                strcat(store1,"\n");
                strcat(store1,"*Password is-");
                strcat(store1,view_joint.password2);
                strcat(store1,"\n");
                char charray1_acc[20];
                sprintf(charray1_acc, "%d", view_joint.account_number);
                strcat(store1,"*Account Number is-");
                strcat(store1,charray1_acc);
                strcat(store1,"\n");
                
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        while((read(fd_view3,&view1_joint,sizeof(view1_joint)))>0)
        {
            lk.l_type   = F_RDLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(view1_joint);        
            lk.l_len    = sizeof(view1_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_view3,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_view,view1_joint.username1);
            strcpy(buff4_view,view1_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff3_pass);
            //printf("the username checking right now is %s\n",buff4_pass);

            if(strcmp(buff3_view,buff1_view)==0)
            {   
                char charray1[20];
                sprintf(charray1, "%2.3f", view1_joint.rem_balance);
                strcat(store1,"*Balance is- Rs");
                strcat(store1,charray1);
                strcat(store1,"\n");
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view3,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else if(strcmp(buff4_view,buff1_view)==0)
            {
                char charray1[20];
                sprintf(charray1, "%2.3f", view1_joint.rem_balance);
                strcat(store1,"*Balance is- Rs");
                strcat(store1,charray1);
                strcat(store1,"\n");
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_view3,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        write(sock,&store1,sizeof(store1));
        lk.l_type=F_UNLCK;
        printf("Going for unlock\n");
        int fc=fcntl(fd_view2,F_SETLK,&lk);
        fcntl(fd_view3,F_SETLK,&lk);
        if(fc==0)
        {
            printf("Lock released\n");
        }
        close(fd_view2);
        close(fd_view3);
        service_login_customer(sock,username);
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void account_statement(int sock,char username[1024])
{   
    int flag_check=0;
    int type_account;
    char buff1_st[1024];
    read(sock,&type_account,sizeof(int));
    printf("Type_Account is:%d\n",type_account);
    strcpy(buff1_st,username);
    if(type_account==1)
    {   
        int fd_st;
        char buff2_st[1024];
        char store[10000];
        struct flock lock;
        struct transaction_single st_single;
        fd_st=open("transaction_all_single.txt",O_RDWR);
        lseek(fd_st,0,SEEK_SET);

        int p=1,check=0;
        while((read(fd_st,&st_single,sizeof(st_single)))>0)
        {
            lock.l_type   = F_RDLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(st_single);        
            lock.l_len    = sizeof(st_single);       
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_st,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_st,st_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_view);
            if(strcmp(buff2_st,buff1_st)==0)
            {   
                flag_check=1;
                strcat(store,"*Transaction- \n");
                strcat(store,"Timestamp-  ");
                struct tm *ptm = localtime(&st_single.date);
                char t[10];
                sprintf(t, "%02d", ptm->tm_mday);
                strcat(store,t);
                strcat(store,"/");
                int tm=ptm->tm_mon +1;
                sprintf(t, "%02d", tm);
                strcat(store,t);
                strcat(store,"/");
                int y=ptm->tm_year+1900;
                sprintf(t, "%04d", y);
                strcat(store,t);
                
                strcat(store,"  ");
                sprintf(t, "%02d", ptm->tm_hour);
                strcat(store,t);
                strcat(store,":");
                sprintf(t, "%02d", ptm->tm_min);
                strcat(store,t);
                strcat(store,":");
                sprintf(t, "%02d", ptm->tm_sec);
                strcat(store,t);
                
                strcat(store,"\n");
                char charray_acc[20];
                sprintf(charray_acc, "%2.3f", st_single.credit);
                strcat(store,"Amount Credited is-");
                strcat(store,charray_acc);
                strcat(store,"\n");
                sprintf(charray_acc, "%2.3f", st_single.debit);
                strcat(store,"Amount debited is-");
                strcat(store,charray_acc);
                strcat(store,"\n");
                sprintf(charray_acc, "%2.3f", st_single.rem_balance);
                strcat(store,"Balance is-");
                strcat(store,charray_acc);
                strcat(store,"\n");
                strcat(store,"\n");
                
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_st,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
            }
            else
            {
                p++;
            }
        }
        
        write(sock,&flag_check,sizeof(int));
        write(sock,&store,sizeof(store));
        close(fd_st);
        service_login_customer(sock,username);
    }
    else if(type_account==2)
    {   
        int fd_st2;
        char buff3_st[1024],buff4_st[1024];
        char store1[10000];
        struct flock lk;
        struct transaction_joint st_joint;
        fd_st2=open("transaction_all_joint.txt",O_RDWR);
        lseek(fd_st2,0,SEEK_SET);

        int pp=1,check1=0;
        while((read(fd_st2,&st_joint,sizeof(st_joint)))>0)
        {
            lk.l_type   = F_RDLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(st_joint);        
            lk.l_len    = sizeof(st_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_st2,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_st,st_joint.username1);
            strcpy(buff4_st,st_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff3_pass);
            //printf("the username checking right now is %s\n",buff4_pass);

            if(strcmp(buff3_st,buff1_st)==0)
            {  
                flag_check=1; 
                strcat(store1,"*Transaction- \n");
                strcat(store1,"Timestamp-  ");
                struct tm *ptm = localtime(&st_joint.date);
                char t[10];
                sprintf(t, "%02d", ptm->tm_mday);
                strcat(store1,t);
                strcat(store1,"/");
                int tm=ptm->tm_mon +1;
                sprintf(t, "%02d", tm);
                strcat(store1,t);
                strcat(store1,"/");
                int y=ptm->tm_year+1900;
                sprintf(t, "%04d", y);
                strcat(store1,t);
                
                strcat(store1,"  ");
                sprintf(t, "%02d", ptm->tm_hour);
                strcat(store1,t);
                strcat(store1,":");
                sprintf(t, "%02d", ptm->tm_min);
                strcat(store1,t);
                strcat(store1,":");
                sprintf(t, "%02d", ptm->tm_sec);
                strcat(store1,t);
                
                strcat(store1,"\n");
                char charray_acc[20];
                sprintf(charray_acc, "%2.3f", st_joint.credit);
                strcat(store1,"Amount Credited is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                sprintf(charray_acc, "%2.3f", st_joint.debit);
                strcat(store1,"Amount debited is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                sprintf(charray_acc, "%2.3f", st_joint.rem_balance);
                strcat(store1,"Balance is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                strcat(store1,"\n");

                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_st2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
            }
            else if(strcmp(buff4_st,buff1_st)==0)
            {   
                flag_check=1;
                strcat(store1,"*Transaction- \n");
                strcat(store1,"Timestamp-  ");
                struct tm *ptm = localtime(&st_joint.date);
                char t[10];
                sprintf(t, "%02d", ptm->tm_mday);
                strcat(store1,t);
                strcat(store1,"/");
                int tm=ptm->tm_mon +1;
                sprintf(t, "%02d", tm);
                strcat(store1,t);
                strcat(store1,"/");
                int y=ptm->tm_year+1900;
                sprintf(t, "%04d", y);
                strcat(store1,t);
                
                strcat(store1,"  ");
                sprintf(t, "%02d", ptm->tm_hour);
                strcat(store1,t);
                strcat(store1,":");
                sprintf(t, "%02d", ptm->tm_min);
                strcat(store1,t);
                strcat(store1,":");
                sprintf(t, "%02d", ptm->tm_sec);
                strcat(store1,t);
                
                strcat(store1,"\n");
                char charray_acc[20];
                sprintf(charray_acc, "%2.3f", st_joint.credit);
                strcat(store1,"Amount Credited is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                sprintf(charray_acc, "%2.3f", st_joint.debit);
                strcat(store1,"Amount debited is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                sprintf(charray_acc, "%2.3f", st_joint.rem_balance);
                strcat(store1,"Balance is-");
                strcat(store1,charray_acc);
                strcat(store1,"\n");
                strcat(store1,"\n");
                
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_st2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
            }
            else
            {
                pp++;
            }
        }
        
        write(sock,&flag_check,sizeof(int));
        write(sock,&store1,sizeof(store1));
        close(fd_st2);
        service_login_customer(sock,username);
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void logout(int sock)
{   
    printf("Current user logged of\n");
    main();
}
void add_account(int sock)
{
    int op_add;
    read(sock,&op_add,sizeof(int));
    printf("option chose= %d\n",op_add);

    if(op_add==1)
    {   
        char username[1024],password[1024];
        int type_account=1;
        read(sock, &username, sizeof(username));
        read(sock, &password, sizeof(password));
        

        printf("%s\n",username);
        printf("%s\n",password);
        //printf("%d",type_account);
        int fd_add = open("db_customer_single.txt", O_RDWR,0777);
        int fd_master = open("db_customer.txt", O_RDWR,0777);
        int fd_joint = open("db_customer_joint.txt", O_RDWR,0777);
        struct Customer_single single_add,temp_add;
        struct Customer_joint temp1_add;
        struct transaction_single transact;
        int flag_add=0;

        char buffer1[1024],buffer2[1024],buffer3[1024],buffer4[1024];
        strcpy(buffer1,username);
        printf("Printing buffer1 %s\n",buffer1);

        while((read(fd_add, &temp_add, sizeof(temp_add)))>0) 
	    {
            strcpy(buffer2,temp_add.username);
		    if(strcmp(buffer2,buffer1)==0)
            {
                printf("username exist\n");
                flag_add=1;
                printf("%d",flag_add);
                printf("%s",buffer2);
                write(sock,&flag_add,sizeof(int));
                break;
            }
            else
            {   
                printf("%s",buffer2);
                //write(1,&temp_add.username,sizeof(temp_add.username));
                printf("user name does not exist\n");
            }
        }
        if(flag_add!=1)
        {
            while((read(fd_joint, &temp1_add, sizeof(temp1_add)))>0) 
	        {
                strcpy(buffer3,temp1_add.username1);
                strcpy(buffer4,temp1_add.username2);
		        if((strcmp(buffer3,buffer1)==0) || (strcmp(buffer4,buffer1)==0))
                {
                    printf("username exist\n");
                    flag_add=1;
                    printf("%d\n",flag_add);
                    printf("%s\n",buffer3);
                    printf("%s\n",buffer3);
                    write(sock,&flag_add,sizeof(int));
                    break;
                }
                else
                {   
                    //printf("%s",buffer3);
                    //write(1,&temp_add.username,sizeof(temp_add.username));
                    printf("user name does not exist\n");
                }
            }
        }    
        if(flag_add==1)
        {   
            printf("%d",flag_add);
            add_account(sock);
        }
        else if(flag_add==0)
        {   
            write(sock,&flag_add,sizeof(int));
            printf("%d",flag_add);
            struct account_number_single account_add;
            struct flock fl;
            int fd_add_account= open("account_increment_single.txt", O_RDWR);
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_SET; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(account_add);        
            fl.l_pid    = getpid(); 
            printf("Before Entering The critical section\n");
            fcntl(fd_add_account, F_SETLKW, &fl); 
            printf("got lock on account number\n");
            read(fd_add_account,&account_add,sizeof(account_add));
            printf("current account number is :%d\n",account_add.account_number_single);
            account_add.account_number_single++;
            printf("updated acoount number is :%d\n",account_add.account_number_single);
            lseek(fd_add_account,0,SEEK_SET);
            write(fd_add_account,&account_add,sizeof(account_add));
            printf("release lock \n");
            fl.l_type   = F_UNLCK;
            fcntl(fd_add_account, F_SETLK, &fl);
            printf("completed\n");
            close(fd_add_account);
            memcpy(single_add.username,username,sizeof(username));
            memcpy(single_add.password,password,sizeof(password));
            single_add.type_account=1;
            single_add.status=0;
            single_add.account_number=account_add.account_number_single;
            int ab1=write(fd_add,&single_add,sizeof(single_add));
            //int ab2=write(fd_master,&single_add,sizeof(single_add));
            memcpy(transact.username,username,sizeof(username));
            transact.account_number=account_add.account_number_single;
            transact.credit=0.0;
            transact.debit=0.0;
            transact.rem_balance=0.0;
            time_t now = time(NULL);
            if (now == -1) 
            {
                puts("The time() function failed");
            }
            printf("%ld\n", now);
            transact.date=now;
            printf("%ld\n", transact.date);
            transact.type_account=1;
            int fd_transact=open("transaction_single.txt",O_RDWR);
            lseek(fd_transact,0,SEEK_END);
            int t=write(fd_transact,&transact,sizeof(transact));
            if(t>0)
            {
                printf("Transaction writing done\n");
            }
            if(ab1==-1 /*|| ab2==-1*/)
            {
                printf("Failed");
                int j=1;
                write(sock,&j,sizeof(int));
                service_admin(sock);
            }
            else
            {
                write(1,"Add Success\n",sizeof("Add Success\n"));
                int j=0;
                write(sock,&j,sizeof(int));
                service_admin(sock);
            }
        }
     }
    else if(op_add==2)
    {
        char username1[1024],password1[1024];
        char username2[1024],password2[1024];
        read(sock, &username1, sizeof(username1));
        read(sock, &password1, sizeof(password1));
        read(sock, &username2, sizeof(username2));
        read(sock, &password2, sizeof(password2));
        printf("%s\n",username1);
        printf("%s\n",password1);
        printf("%s\n",username2);
        printf("%s\n",password2);
        int fd_add = open("db_customer_joint.txt", O_RDWR,0777);
        int fd_master = open("db_customer.txt", O_RDWR,0777);
        int fd_single = open("db_customer_single.txt", O_RDWR,0777);
        struct Customer_joint joint_add,temp_add;
        struct Customer_single tempo_add;
        struct transaction_joint transact1;
        int flag_add=0;
        char buff1_add[1024],buff2_add[1024],buff3_add[1024],buff4_add[1024],buff5_add[1024],buff6_add[1024];
        strcpy(buff1_add,username1);
        strcpy(buff2_add,username2);
        while((read(fd_add, &temp_add, sizeof(temp_add)))>0)
	    {
            strcpy(buff3_add,temp_add.username1);
            strcpy(buff4_add,temp_add.username2);
            
		    if((strcmp(buff3_add,buff1_add)==0) || (strcmp(buff4_add,buff1_add)==0))
            {
                printf("username1 already exist\n");
                flag_add=1;
                write(sock,&flag_add,sizeof(int));
                break;
            }
            else if((strcmp(buff3_add,buff2_add)==0) || (strcmp(buff4_add,buff2_add)==0))
            {
                printf("username2 already exist\n");
                flag_add=1;
                write(sock,&flag_add,sizeof(int));
                break;
            }
            else
            {
                printf("user name does not exist\n");
            }
        }
        if(flag_add!=1)
        {
            char buff7_add[1024];
            while((read(fd_single, &tempo_add, sizeof(tempo_add)))>0) 
	        {
                strcpy(buff7_add,tempo_add.username);
                if((strcmp(buff7_add,buff1_add)==0) || (strcmp(buff7_add,buff2_add)==0))
                {
                    printf("username exist\n");
                    flag_add=1;
                    printf("%d\n",flag_add);
                    //printf("%s\n",buffer3);
                    //printf("%s\n",buffer3);
                    write(sock,&flag_add,sizeof(int));
                    break;
                }
                else
                {   
                    //printf("%s",buffer3);
                    //write(1,&temp_add.username,sizeof(temp_add.username));
                    printf("user name does not exist\n");
                }
            }
        }    
        if(flag_add==1)
        {
            printf("%d",flag_add);
            add_account(sock);
        }
        else if(flag_add==0)
        {
            write(sock,&flag_add,sizeof(int));
            printf("%d",flag_add);
            struct account_number_joint account_joint_add;
            struct flock fl;
            int fd_add_account= open("account_increment_joint.txt", O_RDWR);
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_SET; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(account_joint_add);        
            fl.l_pid    = getpid(); 
            printf("Before Entering The critical section\n");
            fcntl(fd_add_account, F_SETLKW, &fl); 
            printf("got lock on account number\n");
            read(fd_add_account,&account_joint_add,sizeof(account_joint_add));
            printf("current account number is :%d\n",account_joint_add.account_number_joint);
            account_joint_add.account_number_joint++;
            printf("updated acoount number is :%d\n",account_joint_add.account_number_joint);
            lseek(fd_add_account,0,SEEK_SET);
            write(fd_add_account,&account_joint_add,sizeof(account_joint_add));
            printf("release lock \n");
            fl.l_type   = F_UNLCK;
            fcntl(fd_add_account, F_SETLK, &fl);
            printf("completed\n");
            close(fd_add_account);
            memcpy(joint_add.username1, username1,sizeof(username1));
            memcpy(joint_add.password1, password1,sizeof(password1));
            memcpy(joint_add.username2, username2,sizeof(username2));
            memcpy(joint_add.password2, password2,sizeof(password2));
            joint_add.type_account=2;
            joint_add.status=0;
            joint_add.account_number=account_joint_add.account_number_joint;
            int ab3=write(fd_add,&joint_add,sizeof(joint_add));
            //int ab4=write(fd_master,&joint_add,sizeof(joint_add));
            write(1,"Add Success\n",sizeof("Add Success\n"));
            memcpy(transact1.username1,username1,sizeof(username1));
            memcpy(transact1.username2,username2,sizeof(username2));
            transact1.account_number=account_joint_add.account_number_joint;
            transact1.credit=0.0;
            transact1.debit=0.0;
            transact1.rem_balance=0.0;
            time_t now1 = time(NULL);
            if (now1 == -1) 
            {
                puts("The time() function failed");
            }
            printf("%ld\n", now1);
            transact1.date=now1;
            printf("%ld\n", transact1.date);
            transact1.type_account=2;
            int fd_transact1=open("transaction_joint.txt",O_RDWR);
            lseek(fd_transact1,0,SEEK_END);
            int t=write(fd_transact1,&transact1,sizeof(transact1));
            if(t>0)
            {
                printf("Transaction writing done\n");
            }
            if(ab3==-1 /*|| ab4==-1*/)
            {
                printf("Failed");
                int j=1;
                write(sock,&j,sizeof(int));
                service_admin(sock);
            }
            else
            {
                write(1,"Add Success\n",sizeof("Add Success\n"));
                int j=0;
                write(sock,&j,sizeof(int));
                service_admin(sock);
            }
            
        }
    }
    else if(op_add==3)
    {
        service_admin(sock);
    }
    else
    {
        printf("INvalid Entry");
    }
    

}
void delete_account(int sock)
{   
    int type_account;
    read(sock,&type_account,sizeof(int));
    printf("Type_Account is:%d\n",type_account);
    
    if(type_account==1)
    {   
        int choice,fd_del;
        char buf1_del[1024],buf2_del[1024];
        char username[1024];
        read(sock, &username, sizeof(username));
        if((fd_del=open("db_customer_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        strcpy(buf1_del,username);
        struct flock lock;
        struct Customer_single del_single;
        lseek(fd_del,0,SEEK_SET);
        int p=1,check=0;
        while((read(fd_del,&del_single,sizeof(del_single)))>0)
        {
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(del_single);        
            lock.l_len    = sizeof(del_single);        
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_del,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buf2_del,del_single.username);
            printf("Reached if statement\n");
            printf("the username checking right now is %s\n",buf2_del);
            if(strcmp(buf2_del,buf1_del)==0)
            {
                del_single.status=1;
                lseek(fd_del,-1*sizeof(del_single),SEEK_CUR);
                int c=write(fd_del,&del_single,sizeof(del_single));
                lock.l_type=F_UNLCK;
                printf("going for unlock\n");
                int fc=fcntl(fd_del,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c>0)
                {
                    check=1;
                    printf("Account deleted successfully\n");
                    write(sock,&check,sizeof(int));
                }
                break;
            }
            else
            {
                p++;
            }
        }
        if(check==0)
        {   
            write(sock,&check,sizeof(int));
            printf("Account not deleted\n");
            lock.l_type=F_UNLCK;
            printf("going for unlock\n");
            int fc=fcntl(fd_del,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_del);
            service_admin(sock);
        }
        else if(check==1)
        {
            close(fd_del);
            service_admin(sock);
        }
    }
    else if(type_account==2)
    {   
        int fd_del1;
        char buff3_del[1024],buff4_del[1024],buff5_del[1024];
        char username[1024];
        read(sock, &username, sizeof(username));
        strcpy(buff3_del,username);
        struct flock lk;
        struct Customer_joint del_joint;
        if((fd_del1=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_del1,0,SEEK_SET);
        int pp=1,check1=0;
        while((read(fd_del1,&del_joint,sizeof(del_joint)))>0)
        {
            lk.l_type   = F_WRLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(del_joint);        
            lk.l_len    = sizeof(del_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_del1,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff4_del,del_joint.username1);
            strcpy(buff5_del,del_joint.username2);
            printf("Reached if statement\n");
            printf("the username checking right now is %s\n",buff4_del);
            printf("the username checking right now is %s\n",buff5_del);

            if(strcmp(buff4_del,buff3_del)==0)
            {
                del_joint.status=1;
                lseek(fd_del1,-1*sizeof(del_joint),SEEK_CUR);
                int c1=write(fd_del1,&del_joint,sizeof(del_joint));
                lk.l_type=F_UNLCK;
                printf("going for unlock\n");
                int fc=fcntl(fd_del1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Account Deleted successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else if(strcmp(buff5_del,buff3_del)==0)
            {
                del_joint.status=1;
                lseek(fd_del1,-1*sizeof(del_joint),SEEK_CUR);
                int c1=write(fd_del1,&del_joint,sizeof(del_joint));
                lk.l_type=F_UNLCK;
                printf("going for unlock\n");
                int fc=fcntl(fd_del1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Account Deleted successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        if(check1==0)
        {
            write(sock,&check1,sizeof(int));
            lk.l_type=F_UNLCK;
            printf("going for unlock\n");
            int fc=fcntl(fd_del1,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_del1);
            service_admin(sock);
        }
        else if(check1==1)
        {
            close(fd_del1);
            service_admin(sock);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
        service_admin(sock);
    }
    else
    {
        printf("Invalid Type\n");
    }   
    
}
void modify_account(int sock)
{   
    int type_account;
    read(sock,&type_account,sizeof(int));
    printf("Type_Account is:%d\n",type_account);
    
    if(type_account==1)
    {   
        int choice,fd_mod;
        char buf1_mod[1024],buf2_mod[1024];
        char username[1024];
        read(sock, &username, sizeof(username));
        char pass_new[1024];
        char buf1_pass[1024];
        read(sock, &pass_new, sizeof(pass_new));
        printf("New password is %s\n",pass_new);
        if((fd_mod=open("db_customer_single.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        strcpy(buf1_mod,username);
        struct flock lock;
        struct Customer_single mod_single;
        lseek(fd_mod,0,SEEK_SET);
        int p=1,check=0;
        while((read(fd_mod,&mod_single,sizeof(mod_single)))>0)
        {
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(mod_single);        
            lock.l_len    = sizeof(mod_single);        
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_mod,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buf2_mod,mod_single.username);
            //printf("Reached if statement\n");
            //printf("The username checking right now is %s\n",buf2_del);
            if(strcmp(buf2_mod,buf1_mod)==0)
            {
               
                strcpy(mod_single.password,pass_new);
                printf("New password set is %s\n",mod_single.password);
                lseek(fd_mod,-1*sizeof(mod_single),SEEK_CUR);
                int c=write(fd_mod,&mod_single,sizeof(mod_single));
                lock.l_type=F_UNLCK;
                printf("going for unlock\n");
                int fc=fcntl(fd_mod,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c>0)
                {
                    check=1;
                    printf("Password changed successfully\n");
                    write(sock,&check,sizeof(int));
                }
                break;
            }
            else
            {
                p++;
            }
        }
        if(check==0)
        {   
            write(sock,&check,sizeof(int));
            printf("Password not changed\n");
            lock.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_mod,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_mod);
            service_admin(sock);
        }
        else if(check==1)
        {
            close(fd_mod);
            service_admin(sock);
        }
    }
    else if(type_account==2)
    {   
        int fd_mod1;
        char buff3_mod[1024],buff4_mod[1024],buff5_mod[1024];
        char username[1024];
        read(sock, &username, sizeof(username));
        strcpy(buff3_mod,username);
        char pass_new[1024];
        char buf1_pass[1024];
        read(sock, &pass_new, sizeof(pass_new));
        printf("New password is %s\n",pass_new);
        struct flock lk;
        struct Customer_joint mod_joint;
        if((fd_mod1=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_mod1,0,SEEK_SET);
        int pp=1,check1=0;
        while((read(fd_mod1,&mod_joint,sizeof(mod_joint)))>0)
        {
            lk.l_type   = F_WRLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(mod_joint);        
            lk.l_len    = sizeof(mod_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_mod1,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff4_mod,mod_joint.username1);
            strcpy(buff5_mod,mod_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff4_mod);
            //printf("the username checking right now is %s\n",buff5_mod);

            if(strcmp(buff4_mod,buff3_mod)==0)
            {
                strcpy(mod_joint.password1,pass_new);
                printf("New password set is %s\n",mod_joint.password1);
                lseek(fd_mod1,-1*sizeof(mod_joint),SEEK_CUR);
                int c1=write(fd_mod1,&mod_joint,sizeof(mod_joint));
                lk.l_type=F_UNLCK;
                printf("going for unlock\n");
                int fc=fcntl(fd_mod1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Password changed successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else if(strcmp(buff5_mod,buff3_mod)==0)
            {
                strcpy(mod_joint.password2,pass_new);
                printf("New password set is %s\n",mod_joint.password2);
                lseek(fd_mod1,-1*sizeof(mod_joint),SEEK_CUR);
                int c1=write(fd_mod1,&mod_joint,sizeof(mod_joint));
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_mod1,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                if(c1>0)
                {
                    check1=1;
                    printf("Password changed successfully\n");
                    write(sock,&check1,sizeof(int));
                }
                break;
            }
            else
            {
                pp++;
            }
        }
        if(check1==0)
        {
            write(sock,&check1,sizeof(int));
            printf("Password not changed\n");
            lk.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_mod1,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_mod1);
            service_admin(sock);
        }
        else if(check1==1)
        {
            close(fd_mod1);
            service_admin(sock);
        }
    }
    else if(type_account==3)
    {
        printf("Invalid access you are user \n");
        service_admin(sock);
    }
    else
    {
        printf("Invalid Type\n");
    }
}
void search_account(int sock)
{
    char username[1024];
    read(sock, &username, sizeof(username));
    char buff1_search[1024];
    strcpy(buff1_search,username);
    int fd_search,fd_search1;
    int flag_single=0,flag_joint=0;
    char buff2_search[1024];
    char store_search[10000];
    struct flock lock;
    struct Customer_single search_single;
    struct transaction_single search1_single;
    if((fd_search=open("db_customer_single.txt",O_RDWR))==-1)
    {
        printf("File Error");
    }
    lseek(fd_search,0,SEEK_SET);
    fd_search1=open("transaction_single.txt",O_RDWR);
    lseek(fd_search1,0,SEEK_SET);
    int p=1,check=0;
    while((read(fd_search,&search_single,sizeof(search_single)))>0)
    {
        lock.l_type   = F_RDLCK;  
        lock.l_whence = SEEK_SET; 
        lock.l_start  = (p-1)*sizeof(search_single);        
        lock.l_len    = sizeof(search_single);       
        lock.l_pid    = getpid(); 
        printf("Before Critical section\n");
        int f=fcntl(fd_search,F_SETLKW,&lock);
        if(f==0)
        {
            printf("Lock Aquired\n");
        }
        strcpy(buff2_search,search_single.username);
        //printf("Reached if statement\n");
        //printf("the username checking right now is %s\n",buff2_view);
        if(strcmp(buff2_search,buff1_search)==0)
        {   
            flag_single=1;
            strcat(store_search,"*Username is-");
            strcat(store_search,username);
            strcat(store_search,"\n");
            strcat(store_search,"*Password is-");
            strcat(store_search,search_single.password);
            strcat(store_search,"\n");
            char charray_acc[20];
            sprintf(charray_acc, "%d", search_single.account_number);
            strcat(store_search,"*Account Number is-");
            strcat(store_search,charray_acc);
            strcat(store_search,"\n");
            if(search_single.status==0)
            {
                strcat(store_search,"*Status of Account is-");
                strcat(store_search,"Active");
                strcat(store_search,"\n");
            }
            else
            {
                strcat(store_search,"*Status of Account is-");
                strcat(store_search,"Inactive");
                strcat(store_search,"\n");
            }
            strcat(store_search,"*Account Type is is-");
            strcat(store_search,"Single");
            strcat(store_search,"\n");
            lock.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_search,F_SETLK,&lock);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_search);
            break;
        }
        else
        {
            p++;
        }
    }
    if(flag_single==1)
    {
        while((read(fd_search1,&search1_single,sizeof(search1_single)))>0)
        {
            lock.l_type   = F_RDLCK;  
            lock.l_whence = SEEK_SET; 
            lock.l_start  = (p-1)*sizeof(search1_single);        
            lock.l_len    = sizeof(search1_single);       
            lock.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int f=fcntl(fd_search1,F_SETLKW,&lock);
            if(f==0)
            {
                printf("Lock Aquired\n");
            }
            strcpy(buff2_search,search1_single.username);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff2_view);
            if(strcmp(buff2_search,buff1_search)==0)
            {   
                char charray[20];
                sprintf(charray, "%2.3f", search1_single.rem_balance);
                strcat(store_search,"*Balance is- Rs");
                strcat(store_search,charray);
                strcat(store_search,"\n");
                lock.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_search1,F_SETLK,&lock);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                close(fd_search1);
                break;
            }
            else
            {
                p++;
            }
        }
        write(sock,&store_search,sizeof(store_search));
        service_admin(sock);
    }
    else if(flag_single==0)
    {   
        lock.l_type=F_UNLCK;
        printf("Going for unlock\n");
        int fc=fcntl(fd_search,F_SETLK,&lock);
        if(fc==0)
        {
            printf("Lock released\n");
        }
        close(fd_search);
        
        int fd_search2,fd_search3;
        char buff3_search[1024],buff4_search[1024];
        char store1_search[10000];
        struct flock lk;
        struct Customer_joint search_joint;
        struct transaction_joint search1_joint;
        if((fd_search2=open("db_customer_joint.txt",O_RDWR))==-1)
        {
            printf("File Error");
        }
        lseek(fd_search2,0,SEEK_SET);
        fd_search3=open("transaction_joint.txt",O_RDWR);
        lseek(fd_search3,0,SEEK_SET);

        int pp=1,check1=0;
        while((read(fd_search2,&search_joint,sizeof(search_joint)))>0)
        {
            lk.l_type   = F_RDLCK;  
            lk.l_whence = SEEK_SET; 
            lk.l_start  = (pp-1)*sizeof(search_joint);        
            lk.l_len    = sizeof(search_joint);        
            lk.l_pid    = getpid(); 
            printf("Before Critical section\n");
            int fc=fcntl(fd_search2,F_SETLKW,&lk);
            if(fc==0)
            {
                printf("Lock Aquired \n");
            }
            strcpy(buff3_search,search_joint.username1);
            strcpy(buff4_search,search_joint.username2);
            //printf("Reached if statement\n");
            //printf("the username checking right now is %s\n",buff3_pass);
            //printf("the username checking right now is %s\n",buff4_pass);

            if(strcmp(buff3_search,buff1_search)==0)
            {   
                flag_joint=1;
                strcat(store1_search,"*Username is-");
                strcat(store1_search,search_joint.username1);
                strcat(store1_search,"\n");
                strcat(store1_search,"*Password is-");
                strcat(store1_search,search_joint.password1);
                strcat(store1_search,"\n");
                char charray1_acc[20];
                sprintf(charray1_acc, "%d", search_joint.account_number);
                strcat(store1_search,"*Account Number is-");
                strcat(store1_search,charray1_acc);
                strcat(store1_search,"\n");
                if(search_joint.status==0)
                {
                    strcat(store1_search,"*Status of Account is-");
                    strcat(store1_search,"Active");
                    strcat(store1_search,"\n");
                }
                else
                {
                    strcat(store1_search,"*Status of Account is-");
                    strcat(store1_search,"Inactive");
                    strcat(store1_search,"\n");
                }
                strcat(store1_search,"*Account Type is is-");
                strcat(store1_search,"Joint");
                strcat(store1_search,"\n");
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_search2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                close(fd_search2);
                break;
            }
            else if(strcmp(buff4_search,buff1_search)==0)
            {
                flag_joint=1;
                strcat(store1_search,"*Username is-");
                strcat(store1_search,search_joint.username2);
                strcat(store1_search,"\n");
                strcat(store1_search,"*Password is-");
                strcat(store1_search,search_joint.password2);
                strcat(store1_search,"\n");
                char charray1_acc[20];
                sprintf(charray1_acc, "%d", search_joint.account_number);
                strcat(store1_search,"*Account Number is-");
                strcat(store1_search,charray1_acc);
                strcat(store1_search,"\n");
                if(search_joint.status==0)
                {
                    strcat(store1_search,"*Status of Account is-");
                    strcat(store1_search,"Active");
                    strcat(store1_search,"\n");
                }
                else
                {
                    strcat(store1_search,"*Status of Account is-");
                    strcat(store1_search,"Inactive");
                    strcat(store1_search,"\n");
                }
                strcat(store1_search,"*Account Type is is-");
                strcat(store1_search,"Joint");
                strcat(store1_search,"\n");
                
                lk.l_type=F_UNLCK;
                printf("Going for unlock\n");
                int fc=fcntl(fd_search2,F_SETLK,&lk);
                if(fc==0)
                {
                    printf("Lock released\n");
                }
                close(fd_search2);
                break;
            }
            else
            {
                pp++;
            }
        }
        if(flag_joint==1)
        {
            while((read(fd_search3,&search1_joint,sizeof(search1_joint)))>0)
            {
                lk.l_type   = F_RDLCK;  
                lk.l_whence = SEEK_SET; 
                lk.l_start  = (pp-1)*sizeof(search1_joint);        
                lk.l_len    = sizeof(search1_joint);        
                lk.l_pid    = getpid(); 
                printf("Before Critical section\n");
                int fc=fcntl(fd_search3,F_SETLKW,&lk);
                if(fc==0)
                {
                    printf("Lock Aquired \n");
                }
                strcpy(buff3_search,search1_joint.username1);
                strcpy(buff4_search,search1_joint.username2);
                //printf("Reached if statement\n");
                //printf("the username checking right now is %s\n",buff3_pass);
                //printf("the username checking right now is %s\n",buff4_pass);

                if(strcmp(buff3_search,buff1_search)==0)
                {   
                    char charray1[20];
                    sprintf(charray1, "%2.3f", search1_joint.rem_balance);
                    strcat(store1_search,"*Balance is- Rs");
                    strcat(store1_search,charray1);
                    strcat(store1_search,"\n");
                    lk.l_type=F_UNLCK;
                    printf("Going for unlock\n");
                    int fc=fcntl(fd_search3,F_SETLK,&lk);
                    if(fc==0)
                    {
                        printf("Lock released\n");
                    }
                    break;
                }
                else if(strcmp(buff4_search,buff1_search)==0)
                {
                    char charray1[20];
                    sprintf(charray1, "%2.3f", search1_joint.rem_balance);
                    strcat(store1_search,"*Balance is- Rs");
                    strcat(store1_search,charray1);
                    strcat(store1_search,"\n");
                    lk.l_type=F_UNLCK;
                    printf("Going for unlock\n");
                    int fc=fcntl(fd_search3,F_SETLK,&lk);
                    if(fc==0)
                    {
                        printf("Lock released\n");
                    }
                    close(fd_search3);
                    break;
                }
                else
                {
                    pp++;
                }
            }
        write(sock,&store1_search,sizeof(store1_search));
        service_admin(sock);
        }
        else if(flag_joint==0)
        {
            lk.l_type=F_UNLCK;
            printf("Going for unlock\n");
            int fc=fcntl(fd_search2,F_SETLK,&lk);
            if(fc==0)
            {
                printf("Lock released\n");
            }
            close(fd_search2);
            printf("No user in any database\n");
            char store2_search[10000];
            strcat(store2_search,"No Such Username exists");
            strcat(store1_search,"\n");
            write(sock,&store2_search,sizeof(store2_search));
            service_admin(sock);
        }
    }
    else
    {
        printf("Invalid Username\n");
    }
}