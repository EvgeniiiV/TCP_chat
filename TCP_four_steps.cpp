#include <iostream>
using namespace std;
#include <string>
#include "server_chat.cpp"
#include "server_chat.h"
#include <vector>
#include "server_user.cpp"
//#include "server_user.h"
#include <algorithm> 
#include <sstream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
//#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 20000 // Будем использовать этот номер порта
#define MY_QUEUE 5//num of clients in the connection queue

#define MaxUserCount 10//limit of Users

struct sockaddr_in serveraddress, client;
socklen_t length;
int socket_file_descriptor, connection, bind_status, connection_status, activity, sd, max_sd, max_clients = 5;
vector <int>connections;//to store socket descriptors
fd_set readfds; //set of socket descriptors


int main()
{
    vector<User<string>>user;
    user.reserve(MaxUserCount);    
    size_t user_ind = 0;//to keep current index of AUTHORIZED USER       
    string name, login, password;//current user  
    size_t choice = 1;//switchmax_clients = 30
    bool ch(true);//while    
    bool n;//cycles   
    size_t num_of_contact;//to chose the contact in case 4
    size_t presence = 0;//number of groups where this user lists
    stringstream ss;//to collect strings for my_send()   
    int opt = 1; 

     // Создадим сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_file_descriptor == -1)
    {
        cout << "Socket creation failed.!" << endl;
        exit(1);
    }
    if( setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    //fcntl(socket_file_descriptor, F_SETFL, O_NONBLOCK);//to make non-blocking socket
    
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Привяжем сокет
    bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress,
    sizeof(serveraddress));
    if(bind_status == -1)
    {
        cout << "Socket binding failed.!" << endl;
        exit(1);
    }
    // Поставим сервер на прием данных 
    connection_status = listen(socket_file_descriptor, MY_QUEUE);
    if(connection_status == -1)
    {
    cout << "Socket is unable to listen for new connections.!" << endl;
    exit(1);
    }  else cout << "Server is listening for new connection: " << endl;         
      
    while (ch)
    {
      switch (choice)
      {
        case 1:      
               
         //clear the socket set 
         FD_ZERO(&readfds);  
     
         //add listening socket to set 
         FD_SET(socket_file_descriptor, &readfds);  
         max_sd = socket_file_descriptor; 
         
         //add child sockets to set 
         for ( int i = 0 ; i < connections.size() ; i++)  
         {  
            //socket descriptor 
            sd = connections[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
         }     
         
         //wait for an activity on one of the sockets          
         activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);         
         {  
            printf("select error");  
         }             
         //If something happened on the master socket , 
         //then its an incoming connection 
          if (FD_ISSET(socket_file_descriptor, &readfds))  
          {  
            if ((connection = accept(socket_file_descriptor, 
                    (struct sockaddr*)&client, &length))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , connection , inet_ntoa(serveraddress.sin_addr) , ntohs
                  (serveraddress.sin_port));  
            connections.push_back(connection);         
            
          }  
          
          else
          {
            for (int i = 0; i < connections.size(); i++)
            my_receive(connections[i]); 
            choice  = 2;
            break;
          }              

            name.clear();
            n = true;
            while (n)//REGISTRATION
            {                
                my_send (ss.str() + "REGISTRATION: Enter your NAME", connection);
                ss.str("");
                while (name.empty())
                {    
                 name = my_receive(connection);              
                  
                }
                
                for (size_t k = 0; k < user.size(); k++)
                {
                    if (user[k].get_name() == name)
                    {
                        n = false;
                        break;
                    }
                }
                if (!n)
                {                
                 
                 ss << "This name is already used. Choose another name\n";
                 name.clear();
                 n = true;
                }
                else n = false;
                
            }
            n = true;
            while (n)
            {                
                login.clear(); password.clear();                
                my_send (ss.str() + "Enter LOGIN", connection);
                ss.str("");
                while (login.empty())
                {                    
                 login = my_receive(connection);
                }                

                my_send ("Enter PASSWORD", connection);
                while (password.empty())
                {                    
                 password = my_receive(connection);                    
                }
                
                for (size_t k = 0; k < user.size(); k++)
                {
                    if (compare_LP <string>(user[k].get_log_pas(), login, password))
                    {
                        n = false;
                        break;
                    }
                }
                if (!n)
                {                 
                 
                 ss << "Change the login and try again\n";
                 login.clear(); password.clear();
                 n = true;
                }
                else  n = false;
            }            

            user.push_back(User<string>(name, login, password, connection));
            cout << "User's connection = " << connection << endl;
                         
            ss.str("");
            ss << "REGISTERED USERS:\n";
            for (size_t i = 0; i < user.size(); i++)
                ss << to_string(i + 1) << ". " << user[i].get_name() << '\n';

            my_send(ss.str() + "Enter SPACE to continue \n", connection);            
                   
                             
            break;   
           

        case 2://AUTHORIZATION
           
            n = true;
            while (n)
            {
                
                for(int i = 0; i < connections.size(); i++)
                {
                    connection = connections[i];
                    name.clear(); login.clear(); password.clear();                
                    my_send(ss.str() + "AUTHORIZATION: Enter LOGIN", connection);
                    ss.str("");
                    while (login.empty())
                    {                    
                        login = my_receive(connection);
                    }                    
               
                    my_send("Enter PASSWORD", connection);
                    while (password.empty())
                    {                    
                        password = my_receive(connection);
                    }
                   
                    for (size_t k = 0; k < user.size(); k++)
                    {
                        if (compare_LP <string>(user[k].get_log_pas(), login, password))
                        {
                            user_ind = k;
                            n = false;
                            break;
                        }
                    }
                    if (n)
                    {                    
                        ss << "Invalid password or login. Try again\n";
                        login.clear(); password.clear();
                        n = true;
                        break;
                    }
                    else n = false;
                   
                    name = user[user_ind].get_name(); 
                       
                    ss << "\nHELLO " + name + "!\n"; 
                    if (i < connections.size() - 1)                    
                    {
                        my_send (ss.str() + "Please wait until all the users are authorized, ENTER SPACE\n", connection);
                        ss.str("");
                    }
                    else
                    my_send (ss.str() + "All the users are authorized, ENTER SPACE\n", connection);
                    ss.str("");
                    my_receive(connection);
                }  
                choice = 3;       

            }
            break;

        case 3://individual messages
        {
            
            my_send (ss.str() + "Enter SPACE to choose the name of your friend\nEnter 4 to text to everyone\nEnter # to quit chat\n", connection);
            ss.str("");
            if (my_receive(connection) == "4")
            {
                choice  = 4; break;
            }
            if (ch_message[0] == '#')
            {
                
                choice  = 5;
                break;
            }
            if (ch_message[0] != ' ')
            {
                ss << "Wrong sign, try again\n";
                break;
            }
            my_send ("text the name of your friend", connection);
            string n = my_receive(connection);
            
            size_t k;
            for (k = 0; k < user.size(); k++)
            {
                if (n == user[k].get_name())
                {                    
                    break;
                }
            }
            if (k >= user.size())
            {
                ss << "Error of the name, try again\n";
                n.clear();
                break;
            }
            my_send ("text your message to " + n, connection);
            string m = my_receive(connection);
            connection = user[k].get_FD_by_name();
            my_send (m + "\nENTER SPACE to continue", connection);
            my_receive(connection);
           
        }
            break;
        
        case 4://message for everyone    
        {
           
            my_send ("text your message for everyone\n", connection);
            string m = my_receive(connection);           
            for (int i = 0; i < connections.size(); i++)
            {
                 if (connections[i] == connection && i < connections.size() - 1)
                {
                    i++;
                    my_send(m + "\nEnter SPACE to continue", connections[i]);
                    my_receive(connections[i]);
                }
                else if (connections[i] != connection)
                {
                    my_send(m + "\nEnter SPACE to continue", connections[i]);
                    my_receive(connections[i]);
                }
                else break;
            }     
        }
           
        choice = 3;
        break;

        case 5:

        if (connections.size() == 1)
         {
             cout << "server is to close" << endl;
             my_send("# BYE!", connection);
             close(connection);
             ch = false;
             break;

         }
         else
         {                      
            my_send("# BYE!", connection);            
            for (int i = 0; i < connections.size(); i++)
            {
                if (connections[i] == connection && i < connections.size() - 1)
                {
                    close(connection);
                    connections.erase(connections.begin() + i);
                    if (i < connections.size() - 1) i++;
                    connection = connections[i];
                    choice = 3;
                    break;
                }
                else if (connections[i] == connection && i == connections.size() - 1)
                {
                    close(connection);
                    connections.erase(connections.begin() + i);
                    connection = connections[0];
                    choice = 3;
                    break;
                }
                 
            }
         }if (choice == 3) break;

      }
       
    }


    return 0;
}     




