/*************************************************************************
 * 
	> File Name: Pipe.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 09:46:27 AM EDT
 ************************************************************************/

#include <cstdio>
#include<iostream>
#include <string.h>
using namespace std;
#include "Pipe.h"
#include <unistd.h>
int Pipe::create(){
    if(pipe(p[p_create])<0) cout<<"create pipe error\n";
    return p_create++;
}
int Pipe::checkNumber(int command_line){
    if(p_number_table[command_line]) return 1;
    else return 0;
}
int Pipe::number_create(int num){
    if(p_number_table[num]) {
        return 1;
    }
    if(pipe(p_number[num])<0){
        cout<<"create pipe error\n";
    }else{
        p_number_table[num]=1;
    }
    return 1;
}
int Pipe::close_number_table(int check){
    p_number_table[check]=0;
    return 1;
}
int Pipe::close_pipe(int num,int w,pipeType pt){
    switch (pt){
        case pipeType::Ordinary:
            close(p[num][w]);
            break;
        case pipeType::Number:
            close(p_number[num][w]);
            break;
        case pipeType::STDIN:
            fprintf(stderr,"close std in error\n");
        case pipeType::STDERR:
            fprintf(stderr,"close std err error\n");
        case pipeType::STDOUT:
            fprintf(stderr,"close std out error\n");
        defalt:
            break;
    }
    return 1;
}
int Pipe::dup2_pipe(pipeState p_1,pipeState p_2){
    if(p_1.type==pipeType::Ordinary){
        if(p_2.type==pipeType::STDOUT) dup2(p[p_1.num][p_1.port],STDOUT_FILENO);
        if(p_2.type==pipeType::STDIN) dup2(p[p_1.num][p_1.port],STDIN_FILENO);
    }
    if(p_1.type==pipeType::Number){
        // cout<<"number pipe "<<p_1.num<<" "<<p_1.port<<endl;;
        // cout<<"p_number filedescriptor"<<p_number[p_1.num][0]<<endl;
        if(p_2.type==pipeType::STDOUT) dup2(p_number[p_1.num][p_1.port],STDOUT_FILENO);
        if(p_2.type==pipeType::STDIN) dup2(p_number[p_1.num][p_1.port],STDIN_FILENO);
        if(p_2.type==pipeType::STDERR) dup2(p_number[p_1.num][p_1.port],STDERR_FILENO);

    }
    if(p_1.type==pipeType::File){
        if(p_2.type==pipeType::STDOUT) dup2(p_1.num,STDOUT_FILENO);
    }
    return 1;
}
Pipe::Pipe(){
    p_create = 0;
    p_number_create=0;
}
pipeState Pipe::pipe_STDOUT(){
    return pipeState{pipeType::STDOUT,0,0};
}
pipeState Pipe::pipe_STDIN(){
    return pipeState{pipeType::STDIN,0,0};
}
pipeState Pipe::pipe_STDERR(){
   return pipeState{pipeType::STDERR,0,0}; 
}
int Pipe::close_last_pipe(){
   close(p[p_create-1][0]);
   close(p[p_create-1][1]);
   return 1;
}
