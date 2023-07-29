#ifndef SERVER_H
#define SERVER_H

#define MAX_WORDS_IN_LINE 1024
#define MAX_USER 40
#define sharemem 5556
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
using namespace std;

#include "Pipe.h"
#include"command.h"
class Pipe;
class Command;
class SEM;
enum class semType{
    SH,
    CLIENT,
    COUNT,
    SH_CLIENT,
};
typedef struct 
{
    pid_t pidd[MAX_USER]; //每個人的pid;
    char name[MAX_USER][MAX_USER];
    char ip[MAX_USER][MAX_USER];
    char port[MAX_USER][MAX_USER];
    int user_p[MAX_USER][MAX_USER][2];
    int user[MAX_USER];
    int user_in;
    int user_out;
    int newn; //the client in the event
    int event; // signal event
    int tell_user;
    int yell_user;
    char tellmes[MAX_WORDS_IN_LINE];
    char yellmes[MAX_WORDS_IN_LINE];
    int user_pipe_sender;
    int user_pipe_rec;
    char user_mes[MAX_USER][MAX_USER][MAX_WORDS_IN_LINE];
    int mes_come_table[MAX_USER][MAX_USER];
    int fifo_table[MAX_USER][MAX_USER];
    int me;
    int user_count;
    int user_count_read;
}SHMEM;

class SHM{
private:
    SHMEM* sh;
    int shm_id;
    Pipe* P;
    SEM* sem;
public:
    SHM(Pipe*,SEM*);
    ~SHM();
    void initset();
    int init_table();
    int setUserFdtable(int);
    int specialEvent(Command,int);
    int set_user(string,string,string,int);
    int print_env(string);
    int set_env(string,string);
    int npshell(int,int);
    int set_pid(pid_t,int);
    int logout(int);
    int free_user(int);
    void handle();
    void wel_mes(int);
    pair<int,int> userPipe(string,int);
    int close_fifo(int);
    int sig(int,int,string);
    // int setSemVal(int);
    // int setSem();
};


class SEM{
private:
    sem_t sem_sh;
    sem_t sem_client_id;
    sem_t sem_count;
public:
    SEM();
    ~SEM();

    int wait_sem(semType);
    int release_sem(semType);

};
extern SHM* sh;
extern SEM* sem;
extern Pipe* global_p;
void handler_end(int);
string count_command(char*);
void handler(int);

#endif