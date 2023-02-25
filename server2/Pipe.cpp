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
// int Pipe::create(){
//     if(pipe(p[p_create])<0) cout<<"create pipe error\n";
//     // cout<<"create pipe No:"<<p_create<<" filescriptor"<<p[p_create][0]<<endl;
//     return p_create++;
// }

int Pipe::checkTarget(int user_id,int now_line){
    if(!now_line) return 0;
    for(int i=1;i<PIPE_SIZE;i++){
        if(now_line==pipe_target[user_id][i]) {
            // cout<<"match "<<i<<endl;
            return i;  //第ith target
        }
    }
    return 0;
}
int Pipe::number_create(int target_line,int user_id){
    // if(p_number_table[num]) {
    //     return 1;
    // }
    for(int i=1;i<PIPE_SIZE;i++){
        if(!pipe_target[user_id][i]){
            pipe_target[user_id][i] = target_line;
            if(pipe(p_number[user_id][i])<0){
                cout<<"create pipe error\n";
            }
            return i; //user的第i個number pipe 是指到 target line
        }
    }
    // if(pipe(p_number[user_id][num])<0){
    //     cout<<"create pipe error\n";
    // }else{
    //     // cout<<"create number pipe success "<<num<<" "<<p_number[num][0]<<endl;
    //     // p_number_table[num]=1;
    // }
    return 0;
}
int Pipe::user_create(int sd,int user_id,int fd){
    if(!sd) return 0;
    string u_str = to_string(user_id);
    string s_str = to_string(sd);
    if(user_table[user_id][sd]){
        send(fd,(char*)"*** Error: the pipe #",strlen((char*)"*** Error: the pipe #"),0);
        send(fd,(char*)u_str.c_str(),strlen((char*)u_str.c_str()),0);
        send(fd,(char*)"->#",strlen((char*)"->#"),0);
        send(fd,(char*)s_str.c_str(),strlen((char*)s_str.c_str()),0);
        send(fd,(char*)" already exists. ***\n",strlen((char*)" already exists. ***\n"),0);
        return -1;
    }
    if(pipe(p_user[user_id][sd])<0){
        cout<<"user create error !\n";
        return -1;
    }
    user_table[user_id][sd]=1;
    return 1;

}
int Pipe::user_rev(int rev,int user_id,int fd,string str,char** user_name){
    if(!rev) return 0;
    string u_str = to_string(user_id);
    string r_str = to_string(rev);
    if(!user_table[rev][user_id]){
        send(fd,(char*)"*** Error: the pipe #",strlen((char*)"*** Error: the pipe #"),0);
        send(fd,(char*)r_str.c_str(),strlen((char*)r_str.c_str()),0);
        send(fd,(char*)"->#",strlen((char*)"->#"),0);
        send(fd,(char*)u_str.c_str(),strlen((char*)u_str.c_str()),0);
        send(fd,(char*)" does not exist yet. ***\n",strlen((char*)" does not exist yet. ***\n"),0);
        return -1;
    }

    return 1;
}
int Pipe::close_number_table(int check,int user_id){
    pipe_target[user_id][check]=0;
    return 1;
}
void Pipe::setUserTable(int rev,int user_id){
    user_table[rev][user_id]=0;
}
int Pipe::close_pipe(int num,int w,int user_id,pipeType pt){
    switch (pt){
        // case pipeType::Ordinary:
        //     close(p[num][w]);
        //     break;
        case pipeType::Number:
            close(p_number[user_id][num][w]);
            break;
        case pipeType::UserRev:
            close(p_user[num][user_id][w]);
            break;
        case pipeType::UserSend:
            close(p_user[user_id][num][w]);
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
int Pipe::dup2_pipe(pipeState p_1,int user_id,pipeState p_2){
    // if(p_1.type==pipeType::Ordinary){
    //     if(p_2.type==pipeType::STDOUT) dup2(p[p_1.num][p_1.port],STDOUT_FILENO);
    //     if(p_2.type==pipeType::STDIN) dup2(p[p_1.num][p_1.port],STDIN_FILENO);
    // }
    if(p_1.type==pipeType::Number){
        // cout<<"number pipe "<<p_1.num<<" "<<p_1.port<<endl;;
        // cout<<"p_number filedescriptor"<<p_number[p_1.num][0]<<endl;
        if(p_2.type==pipeType::STDOUT) dup2(p_number[user_id][p_1.num][p_1.port],STDOUT_FILENO);
        if(p_2.type==pipeType::STDIN) dup2(p_number[user_id][p_1.num][p_1.port],STDIN_FILENO);
        if(p_2.type==pipeType::STDERR) dup2(p_number[user_id][p_1.num][p_1.port],STDERR_FILENO);

    }
    if(p_1.type==pipeType::File){
        if(p_2.type==pipeType::STDOUT) dup2(p_1.num,STDOUT_FILENO);
    }
    if(p_1.type==pipeType::UserSend){
        if(p_2.type==pipeType::STDOUT) dup2(p_user[user_id][p_1.num][p_1.port],STDOUT_FILENO);
        if(p_2.type==pipeType::STDIN) dup2(p_user[user_id][p_1.num][p_1.port],STDIN_FILENO);
        if(p_2.type==pipeType::STDERR) dup2(p_user[user_id][p_1.num][p_1.port],STDERR_FILENO);
    }
    if(p_1.type==pipeType::UserRev){
        if(p_2.type==pipeType::STDOUT) dup2(p_user[p_1.num][user_id][p_1.port],STDOUT_FILENO);
        if(p_2.type==pipeType::STDIN) dup2(p_user[p_1.num][user_id][p_1.port],STDIN_FILENO);
        if(p_2.type==pipeType::STDERR) dup2(p_user[p_1.num][user_id][p_1.port],STDERR_FILENO);
    }
    return 1;
}
void Pipe::set_output(int fd){
    dup2(fd,STDOUT_FILENO);
    dup2(fd,STDERR_FILENO);
    return;
}
Pipe::Pipe(){
    // p_create = 0;
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
// int Pipe::close_last_pipe(){
//    close(p[p_create-1][0]);
//    close(p[p_create-1][1]);
//    //memset(p_number_table,0,PIPE_SIZE*sizeof(int));
//    return 1;
// }
