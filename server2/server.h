#ifndef SERVER_H
#define SERVER_H

#define MAX_WORDS_IN_LINE 500000
#define MAX_USER 40

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
using namespace std;

#include "Pipe.h"
#include"command.h"
class Pipe;
class Command;
class Server{
private:
    Pipe* P;
    int user_line[MAX_USER] ; //每個user在shell裡面跑第幾行;
    int user_p[MAX_USER][MAX_USER][2];
    int user_pipe_check[MAX_USER][MAX_USER];
    int user[MAX_USER];
    pid_t pid_num[MAX_USER][PIPE_SIZE][100];
    int p_number[MAX_USER][PIPE_SIZE][2];
    int pipe_target[MAX_USER][PIPE_SIZE];//裡面放在第幾行之後要執行
    
    int wait_table[MAX_USER][100][PIPE_SIZE];//有一起wait放在一起

    char* user_port[MAX_USER];
    char* user_name[MAX_USER];
    char* user_ip[MAX_USER];
    char* env[MAX_USER][100][2];
    int env_number[MAX_USER];
    int user_numberpipe_index[MAX_USER];
    int user_n;
    char* start_bar = (char*)"****************************************\n";
    char* wel_bar= (char*)"** Welcome to the information server. **\n";
public:
    void initset(int);
    int broadcast(char*,int,char*);
    int find_user_id(int);
    int chat_cmd(int);
    int clean_user_info(int,int);
    int npshell(int,int);
    int set_user(char*,char*,char*,int);
    int setUserFdtable(int);
    int setUserEnv(int);
    int welcome(int);
    int specialEvent(Command,int);
    int exitshell(int);
    int set_env(string,string,int);
    int print_env(string,int);
    int reName(char*,int);
    Server(Pipe*);
};

void handler_end(int);
string count_command(char*);

#endif