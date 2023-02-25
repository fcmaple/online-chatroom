/*************************************************************************
	> File Name: Pipe.h
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 09:43:28 AM EDT
 ************************************************************************/

#ifndef _PIPE_H
#define _PIPE_H
#define PIPE_SIZE 10000
enum class pipeType{
    Ordinary,
    Number,
    File,
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
    int p[PIPE_SIZE][2];
    int p_number[PIPE_SIZE][2];
    int wait_table[100][PIPE_SIZE];
    int p_number_table[PIPE_SIZE];
    int p_create;
    int p_number_create;
public:
    Pipe();
    int create();
    int number_create(int num);
    int close_pipe(int num,int w,pipeType pt);
    int dup2_pipe(pipeState p1 , pipeState p2 );
    int close_last_pipe();
    int close_number_table(int check);
    int checkNumber(int command_line);
    pipeState pipe_STDOUT();
    pipeState pipe_STDIN();
    pipeState pipe_STDERR();
};
#endif
