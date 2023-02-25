#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "npshell.h"
using namespace std;
int main(int argc,char* argv[]){
    int msock,newSocket,portNum,clientLen,nBytes;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int flag=1;
    int len=sizeof(int);
    int i;
    fd_set rfds;
    fd_set afds;
    msock = socket(PF_INET,SOCK_STREAM,0);
    portNum = atoi(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); 
    if(setsockopt(msock,SOL_SOCKET,SO_REUSEADDR,&flag,len)==-1){
        perror("setsockopt");
        exit(1);
    }
    bind(msock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(listen(msock,40)==0)
        printf("Server Listening\n");
    else
        printf("Error\n");
    addr_size = sizeof(serverStorage);
    int fd,nfds;
    nfds = sizeof(afds);
    FD_ZERO(&afds); //clear all fd_set
    FD_SET(msock,&afds); //join msock in afds
    FD_SET(0,&afds);//join 0 in afds
    while(1){
        memcpy(&rfds,&afds,sizeof(rfds)); //copy afds to rfds
        if(select(nfds,&rfds,(fd_set*)0,(fd_set*)0,(struct timeval*)0)<0){
            fprintf(stderr,"select is wrong\n");
        }
        if(FD_ISSET(0,&rfds)){// stdin
            char p[100];
            read(0,p,sizeof(p));
            if(strncmp(p,(char*)"exit",4)==0){
                close(msock);
                break;
            }
        }
        if(FD_ISSET(msock,&rfds)){ //some request
            newSocket = accept(msock, (struct sockaddr *) &serverStorage, &addr_size);//wel接到connection 給他newsocket
            if(fork()==0){
                npshell(newSocket);//go to npshell
                close(newSocket);
                break;
            }else{
                close(newSocket);//連結完關掉
                wait(NULL);
            }
        }
    }

}