/*************************************************************************
	> File Name: command.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 05:40:01 AM EDT
 ************************************************************************/

#include<iostream>
#include<stdio.h>
using namespace std;
#include<string>
#include<vector>
#include<sstream>
#include<unistd.h>
#include<string.h>
#include <signal.h>
//system
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "command.h"
#include "Pipe.h"
#include "shell_util.h"
#define MAX_WORDS_IN_LINE 500000
void Command::show_command(){
    for(auto i:command){
        for(auto j:i) cout<<j<<" ";
        cout<<endl;
    }
    for(auto i:operation){
        cout<<i<<" ";
    }
    cout<<endl;
    return;
}
int Command::print_env(string var){
    char *p = getenv((char*)var.data());
    if(p==NULL) return 0;
    else cout<<p<<endl;
    return 1;
}
int Command::set_env(string var, string val){
    int p = setenv((char*)var.data(),(char*)val.data(),1);
    if(p<0) fprintf(stderr,"Set env error !\n");
    return 1;
}
int Command::set_set(stringstream& ss){
    string tmp;
    int cur = 0;
    while(ss>>tmp){
        if(tmp[0]=='|' || tmp[0]=='!'){
            operation.push_back(tmp);
            if(tmp.length()==1){
                cur++;
                command.push_back({});
            }
        }
        else {
            command[cur].push_back(tmp);
        }
    }
    return 1;
}
int Command::specialEvent(string cmd){
    if(cmd=="printenv"){
        print_env(cmd);
        return 0;
    }else if(cmd=="setenv"){
        set_env(cmd,cmd);
        return 0;
    }else if(cmd=="exit"){
        return -1;
    }
    return 1;
}
/*
ls | cat | cat > test.txt
operation : | | >
command : {{ls},{cat},{cat},{test.txt}} 
*/
int Command::conduct(Pipe& pip){
    char** arg;
    int pip_now_number;
    int pip_number;
    int is_err;
    for(auto cmd:command){
        int event = specialEvent(cmd[0]);
        if(event==0) continue;
        else if(event<0){
            return 0;
        }
        
        pip_now_number = pip.create();     

        pip_number = isNumber(operation,operation_cursor,command_line);
        if(pip_number) is_err = isErr(operation,operation_cursor,command_line);
        int check=pip.checkNumber(command_line);
        int fp = -1;
        if(pip_number){
            pip.number_create(pip_number);
        } 
        if(cmd.size()>2 && cmd[cmd.size()-2]==">"){
            fp = open((char*)cmd[cmd.size()-1].c_str(),O_RDWR|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
        }
        signal(SIGCHLD,SIG_IGN);
        if(fork()==0){
            if(operation_cursor){ //connect the last command to next command
                pip.close_pipe(pip_now_number-1,1,pipeType::Ordinary);
                pipeState p_in={pipeType::Ordinary,pip_now_number-1,0};
                pip.dup2_pipe(p_in,pip.pipe_STDIN());
                pip.close_pipe(pip_now_number-1, 0,pipeType::Ordinary);
            }else{ //number input
                if(check){
                    pip.close_pipe(command_line,1,pipeType::Number);
                    pip.dup2_pipe({pipeType::Number,command_line},pip.pipe_STDIN());
                    pip.close_pipe(command_line,0,pipeType::Number);
                }
            }
            if(operation_cursor<operation.size()){
               if(pip_number){
                    // cout<<"set pipe to number pipe connect"<<endl;
                    pip.close_pipe(pip_number,0,pipeType::Number);
                    if(is_err)  pip.dup2_pipe({pipeType::Number,pip_number,1},pip.pipe_STDERR());
                    pip.dup2_pipe({pipeType::Number,pip_number,1},pip.pipe_STDOUT());
                    pip.close_pipe(pip_number,1,pipeType::Number);
               }else{
                    pip.close_pipe(pip_now_number,0,pipeType::Ordinary);
                    pipeState p_1 ={pipeType::Ordinary,pip_now_number,1};
                    pip.dup2_pipe(p_1,pip.pipe_STDOUT());
                    pip.close_pipe(pip_now_number,1,pipeType::Ordinary);
               }
            }else{
                if(fp>0) {
                    pipeState p_1 ={pipeType::File,fp,1};
                    pip.dup2_pipe(p_1,pip.pipe_STDOUT());
                    close(fp);
                }
                pip.close_pipe(pip_now_number,0,pipeType::Ordinary);
                pip.close_pipe(pip_now_number,1,pipeType::Ordinary);
            }
            Exec(cmd);
            exit(0);
        }else{
           int status;
           if(operation_cursor){
                // cout<<"parent close "<<pip_now_number-1<<endl;
                pip.close_pipe(pip_now_number-1,0,pipeType::Ordinary);
                pip.close_pipe(pip_now_number-1,1,pipeType::Ordinary);
                if(cmd.size()>2 && cmd[cmd.size()-2]==">") cout<<"in"<<endl;
           }
           if(check){
                pip.close_pipe(command_line,0,pipeType::Number);
                pip.close_pipe(command_line,1,pipeType::Number);
                pip.close_number_table(command_line);
           }
           if(fp>0) close(fp);
           wait(&status);
        }
        operation_cursor++;
        command_cursor++;
    }
    pip.close_last_pipe();
    command_line++;
    return 1;
}
int Command::Exec(vector<string>& cmd){
    char** arg;
    arg = new char*[MAX_WORDS_IN_LINE];
    for(int i=0;i<cmd.size();i++){
        arg[i]=strdup(cmd[i].c_str());
        if(cmd[i]==">") arg[i]=NULL;
        // cerr<<arg[i]<<endl;
    }
    arg[(int)cmd.size()]=NULL;
    if(execvp(arg[0],arg)<0){
        fprintf(stderr,"Unkown command: [%s].\n",arg[0]);
        return 0;
    }
    delete arg;
    return 1;
}
int Command::showindx(){
    cout<<"operation cursor "<<operation_cursor<<endl;
    operation_cursor++;
    return 1;
}
Command::Command(){
    command.clear();
    command.push_back({});
    operation.clear();
    command_cursor=0;
    operation_cursor=0;
    //command_line=0;
}

