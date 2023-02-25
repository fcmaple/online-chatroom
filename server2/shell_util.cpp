#include <cctype>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#define PIPE_SIZE 10000
using namespace std;
int string_to_int(string& str);
int isNumber(vector<string> operation,int operation_cur,int cursor);
int revUser(vector<string> command){
    int u=0;
    for(auto cmd:command){
        if(cmd[0]=='<'){
            for(int i=1;i<cmd.size();i++){
                u = 10*u+(cmd[i]-'0');
            }
            return u;
        }
        
    }
    return u;
}
int sdUser(vector<string> command){
    int u=0;
    for(auto cmd:command){
        if(cmd[0]=='>'){
            for(int i=1;i<cmd.size();i++){
                u = 10*u+(cmd[i]-'0');
            }
            return u;
        }
    }
    return u;
}

int isNumber(vector<string> operation,int operation_cur,int cursor){
    // cout<<"operation_cur "<<operation_cur<<endl;
    if(!operation.size() || operation_cur>=operation.size()) return 0;
    string str = operation[operation_cur];
    // cout<<str<<" "<<cursor<<endl;
    if((str[0]=='|' || str[0]=='!') && str.length()>1) 
        return (string_to_int(str)+cursor)%PIPE_SIZE;
    else return 0;
}
int isErr(vector<string> operation,int operation_cur,int cursor){
    string str = operation[operation_cur];
    // cout<<str<<" "<<cursor<<endl;
    if(str[0]=='!') 
        return 1;
    else return 0;
}

int string_to_int(string& str){
    int s=0;
    int i=1;
    int len=str.length();
    while(i<len){
        // cout<<str[i]<<endl;
        s = 10*s+(str[i++]-'0');
    }
    return s;
}

