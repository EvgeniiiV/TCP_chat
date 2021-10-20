#pragma once
#include<iostream>
using namespace std;
#include<string>


template <typename T> bool compare_LP(T LP, T L, T P);//compares login and password
template <typename T1, typename T2> T2 request(T1 rst, T2 a, T2 b, T2 first);//for registrarion
template <typename T> T next_request();//case 5, 6
inline size_t Factorial(size_t num);//factorial
inline size_t Presence(size_t num);//to calculate max number of possible groups 
//for one user based on the number of registered users

void my_send( string mes);
string my_receive ();