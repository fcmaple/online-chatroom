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
#include "server.h"
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
int Command::set_set(stringstream& ss,int fd,int id,string str,char** name,int* us){
    string tmp;
    int cur = 0;
    mySocket = fd;
    user_id = id;
    all_command = str;
    user_name = name;
    user = us;
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

/*
ls | cat | cat > test.txt
operation : | | >
command : {{ls},{cat},{cat},{test.txt}} 
*/
vector<vector<string>> Command::get_command(){
    return command;
}
int Command::broadcastUser(int id,int s,int r){
    string u_str = to_string(id);
    string s_str = to_string(s);
    string r_str = to_string(r);
    if(r){
        for(int o=0;o<MAX_USER;o++){
            if(user[o]!=-1){
                send(user[o],(char*)"*** ",strlen("*** "),0);
                send(user[o],user_name[r],strlen(user_name[r]),0);
                send(user[o],(char*)" (",strlen(" ("),0);
                send(user[o],(char*)r_str.c_str(),strlen((char*)r_str.c_str()),0);
                send(user[o],(char*)")",strlen(")"),0);
                send(user[o],(char*)" just received from ",strlen((char*)" just received from "),0);
                send(user[o],user_name[id],strlen(user_name[id]),0);
                send(user[o],(char*)" (",strlen("("),0);
                send(user[o],(char*)s_str.c_str(),strlen((char*)s_str.c_str()),0);
                send(user[o],(char*)")",strlen(")"),0);
                send(user[o],(char*)" by '",strlen((char*)" by '"),0);
                send(user[o],(char*)all_command.c_str(),strlen((char*)all_command.c_str()),0);
                send(user[o],(char*)"' ***\n",strlen((char*)"' ***\n"),0);
            }
        }
    }
    if(s){
        for(int o=0;o<MAX_USER;o++){
            if(user[o]!=-1){
                send(user[o],(char*)"*** ",strlen("*** "),0);
                send(user[o],user_name[id],strlen(user_name[id]),0);
                send(user[o],(char*)" (",strlen(" ("),0);
                send(user[o],(char*)s_str.c_str(),strlen((char*)s_str.c_str()),0);
                send(user[o],(char*)")",strlen(")"),0);
                send(user[o],(char*)" just piped '",strlen((char*)" just piped '"),0);
                send(user[o],(char*)all_command.c_str(),strlen((char*)all_command.c_str()),0);
                send(user[o],(char*)"' to ",strlen((char*)"' to "),0);
                send(user[o],user_name[s],strlen(user_name[s]),0);
                send(user[o],(char*)" (",strlen(" ("),0);
                send(user[o],(char*)r_str.c_str(),strlen((char*)r_str.c_str()),0);
                send(user[o],(char*)")",strlen(")"),0);
                send(user[o],(char*)" ***\n",strlen((char*)"' ***\n"),0);
            }
        }
    }
    return 1;
}
int Command::checkUser(int s,int r){
    string r_str = to_string(r);
    string s_str=to_string(s);
    int sign = 1;
    if((r>MAX_USER || user[r]<0 )&& r){
        send(mySocket,(char*)"*** Error: user #",strlen((char*)"*** Error: user #"),0);
        send(mySocket,(char*)r_str.c_str(),strlen((char*)r_str.c_str()),0);
        send(mySocket,(char*)" does not exist yet. ***\n",strlen((char*)" does not exist yet. ***\n"),0);
        sign =0;
    }
    if((s>MAX_USER || user[s]<0) && s){
        send(mySocket,(char*)"*** Error: user #",strlen((char*)"*** Error: user #"),0);
        send(mySocket,(char*)s_str.c_str(),strlen((char*)s_str.c_str()),0);
        send(mySocket,(char*)" does not exist yet. ***\n",strlen((char*)" does not exist yet. ***\n"),0);
        sign=0;
    }
    return sign;
}
int Command::conduct(Pipe* pip,int now_line){
    char** arg;
    int pip_now_number=0;
    int pip_number=0;
    int is_err;
    int conduct_num = pip->checkTarget(user_id,now_line);
    int p[PIPE_SIZE][2] ={0};
    int user_np=-1;
    int user_safe=1;
    int null_fp = open((char*)"/dev/null",O_RDWR);
    // cout<<"command line : "<<command_line<<endl;
    for(auto cmd:command){

        int revPipe = revUser(cmd);
        int sendPipe = sdUser(cmd);
        if(checkUser(sendPipe,revPipe)){
            user_safe =1;
            int sendRes = pip->user_create(sendPipe,user_id,mySocket);
            int revRes = pip->user_rev(revPipe,user_id,mySocket,all_command,user_name);
            if(sendRes<0) return 1;
            if(revRes<0) return 1;
            broadcastUser(user_id,sendPipe,revPipe);
        }else{
            user_safe = 0;
        }


        // 2 send 3  rev 4 send+rev

        // pip_now_number = pip.create();  
        
        if(pipe(p[pip_now_number])<0) cout<<"create pipe error\n";
        //pip_number : number pipe在幾行之後要執行
        pip_number = isNumber(operation,operation_cursor,now_line);//target line
        // user_pip = isUser(operation,operation_cursor,now_line);
        if(pip_number) is_err = isErr(operation,operation_cursor,command_line);
        if(pip_number) cout<<pip_number<<endl;
        int check=pip->checkTarget(user_id,now_line);
        int fp = -1;
        if(pip_number){
            user_np= pip->number_create(pip_number,user_id);
            //user_np 該user中第幾個number pipe
        } 
        if(cmd.size()>2 && cmd[cmd.size()-2]==">"){
            fp = open((char*)cmd[cmd.size()-1].c_str(),O_RDWR|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
        }
        signal(SIGCHLD,SIG_IGN);
        if(fork()==0){
            if(operation_cursor){ //connect the last command to next command
                close(p[pip_now_number-1][1]);
                dup2(p[pip_now_number-1][0],STDIN_FILENO);
                close(p[pip_now_number-1][0]);
            }else{ //number input
                if(check){
                    pip->close_pipe(check,1,user_id,pipeType::Number);
                    pip->dup2_pipe({pipeType::Number,check,0},user_id,pip->pipe_STDIN());
                    pip->close_pipe(check,0,user_id,pipeType::Number);
                }
                if(revPipe && user_safe){ //2
                    pip->close_pipe(revPipe,1,user_id,pipeType::UserRev);
                    pip->dup2_pipe({pipeType::UserRev,revPipe,0},user_id,pip->pipe_STDIN());
                    pip->close_pipe(revPipe,0,user_id,pipeType::UserRev);
                }else if(revPipe && !user_safe){
                    dup2(null_fp,STDIN_FILENO);
                }
            }
            if(operation_cursor<operation.size()){
               if(user_np>0){
                    pip->close_pipe(user_np,0,user_id,pipeType::Number);
                    if(is_err)  pip->dup2_pipe({pipeType::Number,user_np,1},user_id,pip->pipe_STDERR());
                    pip->dup2_pipe({pipeType::Number,user_np,1},user_id,pip->pipe_STDOUT());
                    pip->close_pipe(user_np,1,user_id,pipeType::Number);
               }else{
                    close(p[pip_now_number][0]);
                    dup2(p[pip_now_number][1],STDOUT_FILENO);
                    close(p[pip_now_number][1]);
                    
               }
            }else{
                if(fp>0) {
                    pipeState p_1 ={pipeType::File,fp,1};
                    dup2(fp,STDOUT_FILENO);
                    close(fp);
                }
                if(sendPipe>0 && user_safe){
                    pip->close_pipe(sendPipe,0,user_id,pipeType::UserSend);
                    pip->dup2_pipe({pipeType::UserSend,sendPipe,1},user_id,pip->pipe_STDOUT());
                    pip->close_pipe(sendPipe,1,user_id,pipeType::UserSend);
                }else if(sendPipe>0 && !user_safe){
                    dup2(null_fp,STDOUT_FILENO);
                    dup2(mySocket,STDERR_FILENO);
                }else{
                    pip->set_output(mySocket);
                }
                close(p[pip_now_number][0]);
                close(p[pip_now_number][1]);

            }
            close(null_fp);
            Exec(cmd);
            exit(0);
        }else{
            int status;
            if(operation_cursor){
                close(p[pip_now_number-1][0]);
                close(p[pip_now_number-1][1]);
                if(cmd.size()>2 && cmd[cmd.size()-2]==">") cout<<"in"<<endl;
            }
            if(check){
                pip->close_pipe(check,0,user_id,pipeType::Number);
                pip->close_pipe(check,1,user_id,pipeType::Number);
                pip->close_number_table(check,user_id);
            }
            if(revPipe && user_safe){
                pip->close_pipe(revPipe,0,user_id,pipeType::UserRev);
                pip->close_pipe(revPipe,1,user_id,pipeType::UserRev);
                pip->setUserTable(revPipe,user_id);
            }
           if(fp>0) close(fp);
           wait(&status);
        }
        pip_now_number++;
        operation_cursor++;
        command_cursor++;
    }
    close(null_fp);
    close(p[pip_now_number-1][0]);
    close(p[pip_now_number-1][1]);
    return 1;
}
int Command::Exec(vector<string>& cmd){
    char** arg;
    arg = new char*[MAX_WORDS_IN_LINE];
    for(int i=0;i<cmd.size();i++){
        arg[i]=strdup(cmd[i].c_str());
        if(cmd[i]==">" || cmd[i][0]=='>' || cmd[i][0]=='<') arg[i]=NULL;
    }
    arg[(int)cmd.size()]=NULL;
    if(execvp(arg[0],arg)<0){
        fprintf(stderr,"Unkown command: [%s].\n",arg[0]);
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
}

