#define MAX_WORDS_IN_LINE 500000
#define MAX_USER 40
#define PIPE_SIZE 5000

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
extern int user_line[MAX_USER] ; //每個user在shell裡面跑第幾行;
extern int user_p[MAX_USER][MAX_USER][2];
extern int user_pipe_check[MAX_USER][MAX_USER];
extern int user[MAX_USER];
extern pid_t pid_num[MAX_USER][PIPE_SIZE][100];
extern int p_number[MAX_USER][PIPE_SIZE][2];
extern int pipe_target[MAX_USER][PIPE_SIZE];//裡面放在第幾行之後要執行
extern int wait_table[MAX_USER][100][PIPE_SIZE];//有一起wait放在一起
extern int all_numpip;
extern char* user_port[MAX_USER];
extern char* user_name[MAX_USER];
extern char* user_ip[MAX_USER];
extern char* env[MAX_USER][100][2];
extern int env_number[MAX_USER];
extern int user_numberpipe_index[MAX_USER];
extern int user_n;

int broadcast(char*,int,char*);
void handler_end(int);
int find_user_id(int);
int chat_cmd(int);
int clean_user_info(int,int);
int npshell(int,int);
string count_command(char*);
int specialEvent(string);