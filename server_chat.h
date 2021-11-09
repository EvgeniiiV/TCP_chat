#pragma once
#include<iostream>
using namespace std;
#include<string>
#include<vector>
#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
char ch_message[MESSAGE_LENGTH];

template <typename T> bool compare_LP(T LP, T L, T P);
void my_send( string mes, int connection);
string my_receive (int connection);
