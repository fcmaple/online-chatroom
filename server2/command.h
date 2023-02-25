/*************************************************************************
	> File Name: command.h
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Oct 2022 05:29:11 AM EDT
 ************************************************************************/

#ifndef _COMMAND_H
#define _COMMAND_H
#include <string>
#include<vector>
#include<sstream>
#include "Pipe.h"
using namespace std;
class Pipe;
class Command{
private:
    vector<vector<string>> command;
    vector<string> operation;
    int command_cursor;
    int operation_cursor;
    int command_line; //count total command number
    int Exec(vector<string>& cmd);
    int print_env(string var);
    int set_env(string var,string val);
    int specialEvent(vector<string> str);
    int exitshell();
    int broadcastUser(int id,int s,int r);
    int checkUser(int s,int r);
    int mySocket;
    int user_id;
    string all_command;
    char** user_name;
    int* user;
public:
    int set_set(stringstream& ss,int fd,int id,string str,char** user_name,int* user);
    void set_name(string n);
    void print_name();
    void show_command();
    int conduct(Pipe* p,int line) ;
    int showindx();
    vector<vector<string>> get_command();
    Command();
};

#endif
