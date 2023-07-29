/*************************************************************************
	> File Name: npshell.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 05:06:34 AM EDT
 ************************************************************************/
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <signal.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;
#include "Pipe.h"
#include"command.h"
#include "server.h"
// global



int Server::npshell(int mySocket,int user_id){
    char command_buf[MAX_WORDS_IN_LINE] = {};

    stringstream ss;
    string str_in="";
    Command c;
    initset(user_id);
    // dup2(serverSocket,0);
    // dup2(serverSocket,1);
    // dup2(serverSocket,2);
    // ss.clear();
    // if(!getline(cin,str_in,'\n')) break;
    // pip.set_output(mySocket);
    ssize_t res =  read(mySocket,command_buf,sizeof(command_buf));
    str_in = count_command(command_buf);
    if(str_in=="\n") {
        user_line[user_id]--;
        send(mySocket,(char*)"% ",strlen("% "),0);
        return 0;
    }
    ss.str(str_in); // put str_in in string buffer
    c.set_set(ss,mySocket,user_id,str_in,user_name,user);
    // c->show_command();
    int special = specialEvent(c,user_id);
    if(special>0) c.conduct(this->P,user_line[user_id]);
    else if(special==-1) return -1;

    send(mySocket,(char*)"% ",strlen("% "),0);
    return 1;
}
