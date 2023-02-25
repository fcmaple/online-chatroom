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
void initset(){
    setenv("PATH","bin:.",1);
}
// global
Pipe pip;

void npshell(int serverSocket){
    initset();
    stringstream ss;
    string str_in="";
    Command* c;
    dup2(serverSocket,0);
    dup2(serverSocket,1);
    dup2(serverSocket,2);
    while(1){
        ss.clear();
        c = new Command;
        cout<<"% ";
        if(!getline(cin,str_in,'\n')) break;
        if(str_in=="\r") continue;
        str_in = str_in.substr(0,str_in.size()-1);
        ss.str(str_in); // put str_in in string buffer
        c->set_set(ss);
        if(c->conduct(pip)==0) {
            delete c;
            break;
        }
        str_in="";
        delete c;
    }
    return ;
}
