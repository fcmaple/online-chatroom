

#include "server.h"
using namespace std;

int Server::find_user_id(int fd)
{
    for(int k=0;k<MAX_USER;k++)
        if(fd == user[k])
            return k;
    return -1;
}
int Server::setUserFdtable(int fd){
    for(int i=1;i<MAX_USER;i++){
        if(user[i]<0){
            user[i] = fd;
            return i;
        }
    }
    return -1; //user fd table
}
int Server::clean_user_info(int id,int fd){
    user[id] = -1;          
    close(fd);
    free(user_port[id]);
    free(user_name[id]);
    free(user_ip[id]);
    for(int s=1;s<=env_number[id];s++){
        free(env[id][s][0]);
        free(env[id][s][1]);
    }
    env_number[id] = 0;
    return 1;
}
int Server::chat_cmd(int fd)
{
    int res =1;
    int user_id;
    user_id = find_user_id(fd);
    if(user_id<0) cerr<<"id error\n";
    res = npshell(fd,user_id);
    if(res< 0){
        //cout<<"now to close"<<res<<endl;;
    }else{
        user_line[user_id]++;
        // cout<<"user id"<<user_id<<" line : "<<user_line[user_id]<<endl;
    }
    return res;
}
int Server::set_user(char* port,char* name,char* ip,int id){
    // cout<<"user id "<<id<<endl;
    user_port[id] = port;
    user_name[id] = name;
    user_ip[id] = ip;
    // cout<<"set_user "<<user_name[id]<<" "<<user_ip[id]<<" "<<user_port[id]<<endl;
    return 1;
}
int Server::setUserEnv(int id){
    char* eenv_p = (char*)malloc(sizeof((char*)"PATH"));
    char* eenv_v = (char*)malloc(sizeof((char*)"bin:."));
    strcpy(eenv_p,(char*)"PATH"); 
    strcpy(eenv_v,(char*)"bin:.");
    env_number[id] = 1;
    env[id][env_number[id]][0] = eenv_p;
    env[id][env_number[id]][1] = eenv_v;
    // cout<<eenv_p<<" "<<eenv_v<<endl;
    return 1;
}
int Server::welcome(int id){
    send(user[id],start_bar,strlen(start_bar),0);
    send(user[id],wel_bar,strlen(wel_bar),0);
    send(user[id],start_bar,strlen(start_bar),0);
    broadcast((char*)"login",id,(char*)"\0");
    send(user[id],(char*)"% ",strlen("% "),0);
    return 1;
}

int Server::broadcast(char* select,int myiid,char* mes_content){
    char* user_bar =  (char*)"*** User '(no name)' entered from ";
    if(strncmp(select,(char*)"login",5)==0){
        for(int o=0;o<MAX_USER;o++){
            if(user[o]!=-1){
                send(user[o],user_bar,strlen(user_bar),0);
                // cout<<user_ip[myiid]<<endl;
                send(user[o],user_ip[myiid],strlen(user_ip[myiid]),0);
                send(user[o],(char*)":",strlen(":"),0);
                send(user[o],user_port[myiid],strlen(user_port[myiid]),0);
                send(user[o],(char*)". ***\n",strlen((char*)". ***\n"),0);
            }
        }

        return -1;
    }else if(strncmp(select,(char*)"logout",6)==0){
        // cout<<"logout\n";
        for(int o=0;o<MAX_USER;o++){
            if(user[o]>0 && o!=myiid){
                send(user[o],(char*)"*** User '",strlen((char*)"*** User '"),0);
                send(user[o],user_name[myiid],strlen(user_name[myiid]),0);
                send(user[o],(char*)"' left. ***\n",strlen((char*)"' left. ***\n"),0);
            }
        }
    }else if(strncmp(select,(char*)"yell",4)==0){
        for(int o=0;o<MAX_USER;o++){
            if(user[o]>0){
                send(user[o],(char*)"*** ",strlen((char*)"*** "),0);
                send(user[o],user_name[myiid],strlen(user_name[myiid]),0);
                send(user[o],(char*)" yelled ***: ",strlen((char*)" yelled ***: "),0);
                send(user[o],mes_content,strlen(mes_content),0);
                send(user[o],(char*)"\n",strlen((char*)"\n"),0);
                if(o!=myiid)send(user[o],(char*)"% ",strlen("% "),0);
            }
        }
        return 1;
    }
    
    return -1;
}

int Server::exitshell(int user_id){
    char* mes = user_name[user_id];
    for(int q=0;q<MAX_USER;q++){
        if(user_p[user_id][q][0]!=0 ||user_p[user_id][q][1]!=0){
            close(user_p[user_id][q][0]);
            close(user_p[user_id][q][1]);
            user_p[user_id][q][0]=0;
            user_p[user_id][q][1]=0;
            user_pipe_check[user_id][q]=0;
            
        }
        if(user_p[q][user_id][0]!=0 ||user_p[q][user_id][1]!=0){
            close(user_p[q][user_id][0]);
            close(user_p[q][user_id][1]);
            user_p[q][user_id][0]=0;
            user_p[q][user_id][1]=0;
            user_pipe_check[q][user_id]=0;
        }
    }
    for(int q=0;q<PIPE_SIZE;q++){
        p_number[user_id][q][0] =0;
        p_number[user_id][q][1] =0;
        pipe_target[user_id][q] =0;
    }
    for(int q=0;q<100;q++){
        for(int w=0;w<PIPE_SIZE;w++){
            pid_num[user_id][q][w]=0;
        }
    }
    broadcast((char*)"logout",user_id,(char*)"\0");
    return -1;
}
void Server::initset(int id)
{
    for(int p=1;p<=env_number[id];p++){
        setenv(env[id][env_number[id]][0],env[id][env_number[id]][1],1);
        // cout<<env[id][env_number[id]][1]<<endl;
    }
    
}
int Server::set_env(string var, string val,int user_id){
    for(int p=1;p<=env_number[user_id];p++){
        if(strcmp(env[user_id][p][0],(char*)var.c_str())==0)
        {
            strcpy(env[user_id][p][1],(char*)val.c_str());
            return 1;
        }
    }
    int p = setenv((char*)var.data(),(char*)val.data(),1);
    if(p<0) fprintf(stderr,"Set env error !\n");
    env_number[user_id]++;
    env[user_id][env_number[user_id]][0] = (char*)var.data();
    env[user_id][env_number[user_id]][1] = (char*)val.data();
    return 1;
}
int Server::print_env(string var,int Socket){
    char *p = getenv((char*)var.data());
    if(p==NULL) return 0;
    send(Socket,p,strlen(p),0);
    send(Socket,(char*)"\n",strlen((char*)"\n"),0);
    // cout<<p<<endl;
    return 1;
}
int Server::reName(char* newName,int user_id){
    for(int d=1;d<=MAX_USER;d++){
        if(user[d]!=-1)
        {
            if(strcmp(newName,user_name[d])==0 && strlen(newName)==strlen(user_name[d])){
                send(user[user_id],(char*)"*** User '",strlen((char*)"*** User '"),0);
                send(user[user_id],user_name[d],strlen(user_name[d]),0);
                send(user[user_id],(char*)"' already exists. ***\n",strlen((char*)"' already exists. ***\n"),0);
                return 0;
            }
        }
    }
    strcpy(user_name[user_id],newName);
    //broadcast((char*)"rename",myid,(char*)"\0");
    return 1;
}
int Server::specialEvent(Command com,int user_id){
    vector<vector<string>> command = com.get_command();
    for(auto cmd:command){
        if(cmd[0]=="printenv"){
            print_env(cmd[1],user[user_id]);
            return 0;
        }else if(cmd[0]=="setenv"){
            set_env(cmd[1],cmd[2],user_id);
            return 0;
        }else if(cmd[0]=="exit"){
            exitshell(user_id);
            // cout<<"exit!!"<<endl;
            return -1;
        }else if(cmd[0]=="name"){
            int x = reName((char*)cmd[1].c_str(),user_id);
            if(x<0) return -1;
            if(!x) return 0;
            for(int o=0;o<MAX_USER;o++){
                if(user[o]!=-1){
                    send(user[o],(char*)"*** User from ",strlen((char*)"*** User from "),0);
                    send(user[o],user_ip[user_id],strlen(user_ip[user_id]),0);
                    send(user[o],(char*)":",strlen(":"),0);
                    send(user[o],user_port[user_id],strlen(user_port[user_id]),0);
                    send(user[o],(char*)" is named '",strlen((char*)" is named '"),0);
                    send(user[o],user_name[user_id],strlen(user_name[user_id]),0);
                    send(user[o],(char*)"'. ***\n",strlen((char*)"'. ***\n"),0);
                    if(o!=user_id)send(user[o],(char*)"% ",strlen("% "),0);
                    
                }
            }
            return 0;

        }else if(cmd[0]=="who"){
            send(user[user_id],(char*)"<ID>    <nickname>    <IP:port>    <indicate me>\n",strlen("<ID>    <nickname>    <IP:port>    <indicate me>\n"),0);
            for(int i=1;i<MAX_USER;i++){
                if(user[i]!=-1){
                    string num(to_string(i));
                    send(user[user_id],(char*)num.c_str(),strlen((char*)num.c_str()),0);
                    send(user[user_id],(char*)"     ",strlen((char*)"     "),0);
                    send(user[user_id],user_name[i],strlen(user_name[i]),0);
                    send(user[user_id],(char*)"     ",strlen((char*)"     "),0);
                    send(user[user_id],user_ip[i],strlen(user_ip[i]),0);
                    send(user[user_id],(char*)":",strlen((char*)":"),0);
                    send(user[user_id],user_port[i],strlen(user_port[i]),0);
                    if(i == user_id) send(user[user_id],(char*)"    <-me\n",strlen((char*)"    <-me\n"),0);
                    else send(user[user_id],(char*)"\n",strlen((char*)"\n"),0);
                }    
            }
            return 0;
        }else if(cmd[0]=="yell"){
            string mes;
            for(int i=1;i<cmd.size();i++){
                mes += cmd[i];
                if(i!=cmd.size()-1) mes+=" ";
            }
            broadcast((char*)"yell",user_id,(char*)mes.c_str());
            return 0;
        }else if(cmd[0]=="tell"){
            int receiver = atoi(cmd[1].c_str());
            string mes = "";
            for(int i=2;i<cmd.size();i++){
                mes += cmd[i];
                if(i!=cmd.size()-1) mes+=" ";
            }
            if(user[receiver]<0){
                send(user[user_id],(char*)"*** Error: user #",strlen((char*)"*** Error: user #"),0);
                send(user[user_id],(char*)cmd[1].c_str(),strlen(cmd[1].c_str()),0);
                send(user[user_id],(char*)" does not exist yet. ***\n",strlen((char*)" does not exist yet. ***\n"),0);
                return 0;
            }else{
                send(user[receiver],(char*)"*** ",strlen((char*)"*** "),0);
                send(user[receiver],user_name[user_id],strlen(user_name[user_id]),0);
                send(user[receiver],(char*)" told you ***: ",strlen((char*)" told you ***: "),0);
                send(user[receiver],(char*)mes.c_str(),strlen((char*)mes.c_str()),0);
                send(user[receiver],(char*)"\n",strlen((char*)"\n"),0);
                return 0;
            }
        }
    }
    return 1;
}
Server::Server(Pipe *p){
    for(int i=1;i<=MAX_USER;i++) user[i]=-1;
    // for(int i=0;i<=)
    this->P = p;
}
string count_command(char* buf){
    int q=0;
    string s="";
    if(buf[0]=='\r') {
        return s;
    }
    if(strlen(buf)==0 || buf[1]=='\n') return "";
    while(buf[q]!='\n' || buf[q]!='\0'){
        if(buf[q]=='\r') break;
        q++;
    }
    char* bbuf = (char*)calloc(MAX_WORDS_IN_LINE,sizeof(char));
    strncpy(bbuf,buf,q);
    string str = string(bbuf);
    free(bbuf);
    return str;
}
void handler_end(int signo)
{
    exit(0);
}
