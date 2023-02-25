/*************************************************************************
	> File Name: Pipe.h
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 09:43:28 AM EDT
 ************************************************************************/

#ifndef _PIPE_H
#define _PIPE_H
#define PIPE_SIZE 10000
#include "server.h"
enum class pipeType{
    Ordinary,
    Number,
    File,
    User,
    STDOUT,
    STDIN,
    STDERR,
};
struct pipeState{
    pipeType type;
    int num;
    int port;
};
class Pipe{
private:
    // int p[PIPE_SIZE][2];
    int p_number[PIPE_SIZE][2];
    int p_user[MAX_USER][MAX_USER][2];
    int user_pipe_check[MAX_USER][MAX_USER];
    int pipe_target[PIPE_SIZE];
    int user_table[MAX_USER][MAX_USER];
    //裡面放在第幾行之後要執行 ex:pipe_target[1][1]=5 mean user1 first number pipe is 5 


    int wait_table[100][PIPE_SIZE];
    int p_number_table[PIPE_SIZE];
    // int p_create;
    int p_number_create;


public:
    Pipe();
    int create();
    int user_create(int sd,int id,int fd);
    int number_create(int num,int id);
    int close_pipe(int num,int w,int id,pipeType pt);
    int dup2_pipe(pipeState p1,int id , pipeState p2 );
    int close_last_pipe();
    int close_number_table(int check,int id);
    int checkNumber(int command_line);
    void set_output(int fd);
    int checkTarget(int now_line);
    void setUserTable(int rev,int id);
    int user_rev(int rev,int user_id,int fd,string str,char** user_name);
    pipeState pipe_STDOUT();
    pipeState pipe_STDIN();
    pipeState pipe_STDERR();
};
#endif
