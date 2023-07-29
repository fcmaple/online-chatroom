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
int Command::set_set(stringstream& ss,int id,string str){
    string tmp;
    int cur = 0;
    user_id = id;
    all_command = str;
    command.clear();
    operation.clear();
    command.push_back({});
    command_cursor=0;
    operation_cursor=0;
    // cout<<user_name[id]<<endl;
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
/*
fifo_send : -1 no_user -2 pip_already
fifo_rev : -1 no_user -2 pip_no exist
*/
int Command::conduct(Pipe* pip,int fifo_send,int fifo_rev){
    char** arg;
    int pip_now_number=0;
    int pip_number=0;
    int is_err;
    int res=0;
    int p[PIPE_SIZE][2] ={0};
    int user_np=-1;
    int user_safe=1;
    int null_fd = open((char*)"/dev/null",O_RDWR);
    for(auto cmd:command){
        int rev_id = revUser(cmd); //find receiver id
        int send_id = sdUser(cmd);// find send id
        // 2 send 3  rev 4 send+rev        
        if(pipe(p[pip_now_number])<0) cout<<"create pipe error\n";
        pip_number = isNumber(operation,operation_cursor,command_line);//target line
        if(pip_number) is_err = isErr(operation,operation_cursor,command_line);
        int check=pip->checkTarget(command_line);
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
                if(rev_id && fifo_rev>0){ //2
                    dup2(fifo_rev,STDIN_FILENO);
                    close(fifo_rev);
                }else if(rev_id && fifo_rev==-1){
                    printf("*** Error: user #%d does not exist yet. ***\n",rev_id);
                    dup2(null_fd,STDIN_FILENO);
                }else if(rev_id && fifo_rev==-2){
                    printf("*** Error: the pipe #%d->#%d does not exist yet. ***\n",rev_id,user_id);
                    dup2(null_fd,STDIN_FILENO);
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
                if(send_id && fifo_send>0){
                    dup2(fifo_send,STDOUT_FILENO);
                    close(fifo_send);
                }else if(send_id && fifo_send==-1){
                    printf("*** Error: user #%d does not exist yet. ***\n",send_id);
                    dup2(null_fd,STDOUT_FILENO);
                }else if(send_id && fifo_send==-2){
                    printf("*** Error: the pipe #%d->#%d already exists. ***\n",user_id,send_id);
                    dup2(null_fd,STDOUT_FILENO);
                }
                close(p[pip_now_number][0]);
                close(p[pip_now_number][1]);
            }
            close(null_fd);
            Exec(cmd);
            exit(0);
        }else{
            int status;
            if(operation_cursor){
                close(p[pip_now_number-1][0]);
                close(p[pip_now_number-1][1]);
            }
            if(check){
                pip->close_pipe(check,0,user_id,pipeType::Number);
                pip->close_pipe(check,1,user_id,pipeType::Number);
                pip->close_number_table(check,user_id);
            }
            if(rev_id && fifo_rev>0){
                close(fifo_rev);
            }
            if(send_id && fifo_send>0){
                close(fifo_send);
                res = send_id;
            }
           if(fp>0) close(fp);
           wait(&status);
        }
        pip_now_number++;
        operation_cursor++;
        command_cursor++;
    }
    close(p[pip_now_number-1][0]);
    close(p[pip_now_number-1][1]);
    command_line++;
    return res;
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
    command_line=0;
}

