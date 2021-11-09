#include<iostream>
using namespace std;
#include<string>
#include<string.h>
#include <sstream>
#include <streambuf>
#include "server_chat.h"
#include <vector>
#include <sys/socket.h>
#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных



template <typename T> bool compare_LP(T LP, T L, T P)//compares login and password
{
    return LP == L + P;
}

//server -> client
void my_send(string _mes, int connection)
{   
    ssize_t bytes;  
    bzero(ch_message, MESSAGE_LENGTH);
    strncpy (ch_message, _mes.c_str(), MESSAGE_LENGTH);
    if (_mes.size() >= MESSAGE_LENGTH)
    {
        ch_message[MESSAGE_LENGTH-1] = '\0';
    }               
    bytes = send(connection, ch_message, sizeof(ch_message), 0);
    if (bytes < 0) cout <<  "send failed" << endl;
       
}  
 
//client -> server
string my_receive (int connection)
{    
    bzero(ch_message, MESSAGE_LENGTH);
    ssize_t bytes = recv(connection, ch_message, MESSAGE_LENGTH, 0);    
    if (bytes < 0) 
    {
        perror("recv");        
    }   
    string s_read = string (ch_message, strlen(ch_message));//for received msg    
    cout << s_read << endl;  
    return s_read;    
}  
