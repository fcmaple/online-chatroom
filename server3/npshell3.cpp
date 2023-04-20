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

char command_buf[MAX_WORDS_IN_LINE] = {};


int SHM::npshell(int mySocket,int user_id){
    stringstream ss;
    string str_in="";
    Command c;
    int res=0;
    initset();

    dup2(mySocket,STDIN_FILENO);
    dup2(mySocket,STDOUT_FILENO);
    dup2(mySocket,STDERR_FILENO);
    wel_mes(user_id);
    while(1){
        ss.clear();
        cout<<"% ";
        if(!getline(cin,str_in,'\n')) break;
        if(str_in=="\r") continue;
        str_in = str_in.substr(0,str_in.size());
        ss.str(str_in); // put str_in in string buffer
        // cout<<"command "<<str_in<<endl;
        c.set_set(ss,user_id,str_in);
        // c->show_command();
        int special = specialEvent(c,user_id);
        if(special>0) {
            pair<int,int> userP = userPipe(str_in,user_id);
            res = c.conduct(this->P,userP.first,userP.second);
            sig(res,user_id,str_in);
        }
        else if(special==-1) return -1;
        str_in="";
        // cout<<str_in<<endl;
    }
    return 1;
}
