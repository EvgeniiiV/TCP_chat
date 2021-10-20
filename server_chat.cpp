#include<iostream>
using namespace std;
#include<string>
#include<string.h>
#include <streambuf>
#include "server_chat.h"
#include <vector>




template <typename T> bool compare_LP(T LP, T L, T P)//compares login and password
{
    return LP == L + P;
}

template <typename T1, typename T2> T2 request(T1 rst, T2 a, T2 b, T2 first)
{
    T2 choice = 0;
    if (!first)
    {
       
        my_send ("REGISTRATION of next USER: press key 1\n");
        do//to prevent invalid input
        {
            
            string rcv =  my_receive();
            choice = (size_t)stoi (rcv);
            
            

            if (choice != a)
                
                my_send("Enter proper sign\n");
        } 
        while (choice != a);
    }
    if (first)
    {
        do//to prevent invalid input during 
        {
            
            my_send(/*socket_file_descriptor,*/rst);
            
            string rcv =  my_receive();
            choice = (size_t)stoi (rcv);           
            if (choice != a && choice != b)               
            my_send("Enter proper sign\n");    
        } 
        while (choice != a && choice != b);
    }
    return choice;
}

template <typename T> T next_request()//case 5, 6
{
    size_t choice = 0;
    do
    {
        
        my_send ("\nLogout and Registration: press key 1\nLogout and Authorization: press key 2\nCreate new contact: press key 3\nChose another contact: press key 4\nNext message to this contact: press key  5\nMessage for everyone: press key 6\nQuit: press key 7\n");
        string rcv = my_receive();
        choice = (size_t)stoi (rcv);
        // stringstream ss(my_receive(socket_file_descriptor));
        // ss >> choice;
        // if (!cin)
        //     cout << "ENTER a DIGIT" << endl;
        if (choice < 1 || choice > 7)
            //cout << "ENTER 1 - 7" << endl;
            my_send("ENTER 1 - 7\n");
    } while (choice < 1 || choice > 7);
    return choice;
}

size_t Factorial(size_t num)
{
    size_t p = 0;
    size_t f = 1;
    for (size_t i = 1; i <= num; i++)
        f *= i;
    return (num != 0) ? f : 1;
}

size_t Presence(size_t num)
{
    if (num <= 2) return 1;
    num--;
    size_t p = 0;
    size_t d = num;
    do
    {
        p += Factorial(num) / (Factorial(num - d) * Factorial(d));
        d--;
    } while (d >= 2);
    p += num;

    return p;
}
