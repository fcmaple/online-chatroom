#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <string>
using namespace std;
int string_to_number(string& str);
int isNumber(vector<string> operation,int operation_cur,int cursor);
int isErr(vector<string> operation,int operation_cur,int cursor);

#endif
