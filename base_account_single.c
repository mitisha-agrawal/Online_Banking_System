#include<stdio.h>
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


#include "account_number_single_increment.c"
void main()
{
    struct account_number_single st;
    st.account_number_single=1;
    int fd=open("account_increment_single.txt",O_RDWR,0777);
    write(fd,&st,sizeof(st));
    close(fd);
}