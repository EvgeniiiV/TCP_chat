#include<iostream>
using namespace std;
#include<string>
#include "server_chat.cpp"
#include<vector>
#include "server_user.cpp"
#include<algorithm> 
#include<sstream>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h> 

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 7777 // Будем использовать этот номер порта

#define MaxUserCount 10//limit of Users
#define MaxGroupCount 100//limit of contacts (groups of users) 
#define MaxMessageCount 200//limit of messages in one group

struct sockaddr_in serveraddress, client;
socklen_t length;
int socket_file_descriptor, connection, bind_status, connection_status;
char ch_message[MESSAGE_LENGTH];

//server -> client
void my_send(string _mes)
{     
    bzero(ch_message, MESSAGE_LENGTH);
    strncpy (ch_message, _mes.c_str(), MESSAGE_LENGTH);
    ssize_t bytes = send (connection, ch_message, sizeof(ch_message),0);   
  
}  
 
//client -> server
string my_receive ()
{    
    bzero(ch_message, MESSAGE_LENGTH);
    recv(connection, ch_message, sizeof(ch_message), 0);
        // if (strncmp("end", ch_message, 3) == 0) {
        //     cout << "Client Exited." << endl;
        //     cout << "Server is Exiting..!" << endl;
        //     close(socket_file_descriptor);                
        // }
    cout << ch_message << endl;
    string s_read = string (ch_message, sizeof(ch_message));//for received msg     
    return s_read;    
}  

int main()
{
     // Создадим сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_file_descriptor == -1)
    {
        cout << "Socket creation failed.!" << endl;
        exit(1);
    }
    // 
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Привяжем сокет
    bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress,
    sizeof(serveraddress));
    if(bind_status == -1)  {
        cout << "Socket binding failed.!" << endl;
        exit(1);
    }
    // Поставим сервер на прием данных 
    connection_status = listen(socket_file_descriptor, 5);
    if(connection_status == -1){
    cout << "Socket is unable to listen for new connections.!" << endl;
    exit(1);
    }  else  {
            cout << "Server is listening for new connection: " << endl;
        }
    length = sizeof(client);
    connection = accept(socket_file_descriptor,(struct sockaddr*)&client, &length);
    if(connection == -1)  {
        cout << "Server is unable to accept the data from client.!" << endl;
        exit(1);
    }

    char start []{"Do you want to start chatting? (press any key - to continue, # - to quit)"};
    ssize_t bytes = send (connection, start, sizeof(ch_message),0);        
        if(bytes >= 0)
        {
           cout << "Start is ok!" << endl;
        }
    bzero(ch_message, MESSAGE_LENGTH);

    recv(connection, ch_message, sizeof(ch_message), 0);
    cout << ch_message << endl;  
        
    if (strncmp("n", ch_message, 1) == 0)
    {
    cout << "Client Exited." << endl;
    cout << "Server is Exiting..!" << endl;
    close(socket_file_descriptor);
    return 0;
    }    
        

    
    vector<User<string>>user;
    user.reserve(MaxUserCount);
    vector<vector<string>>group;//to storage user's names in the group (contact)
    group.reserve(MaxGroupCount);
    vector<vector<string>>mess;//messages
    mess.reserve(MaxMessageCount);
    vector<string>temp;//keeps cuttent group's composition 
    temp.reserve(MaxGroupCount);
    vector <string>temp_mes;//for temporary storage of messages (case 5, 6)
    temp_mes.reserve(20);
    size_t user_ind = 0;//to keep current index of AUTHORIZED USER       
    string name, login, password;//current user
    string contact_name;//current contact
    string message;
    size_t choice = 1;//switch
    bool ch(true);//while    
    bool n;//cycles   
    size_t num_of_contact;//to chose the contact in case 4
    size_t presence = 0;//number of groups where this user lists
    stringstream ss;//to collect strings for my_send()

    
    while (ch)
    {
        switch (choice)
        {
        case 1:
            name.clear();
            n = true;
            while (n)
            {                
                my_send (ss.str() + "REGISTRATION: Enter your NAME");
                ss.str("");
                while (name.empty())
                {    
                 name = my_receive();              
                  
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
                my_send (ss.str() + "Enter LOGIN");
                ss.str("");
                while (login.empty())
                {                    
                 login = my_receive(); 
                }                
                my_send ("Enter PASSWORD");
                while (password.empty())
                {                    
                 password = my_receive();                    
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
            choice = request <string, size_t>("to continue REGISTRATION press key 1, for AUTHORIZATION press key 2", 1, 2, user.size());
            user.push_back(User<string>(name, login, password));
            
            {                
                ss.str("");
                ss << "REGISTERED USERS:\n";
                for (size_t i = 0; i < user.size(); i++)
                ss << to_string(i + 1) << ". " << user[i].get_name() << '\n';                         
            }                          
            break;             
            
           

        case 2:
           
            n = true;
            while (n)
            {
                name.clear(); login.clear(); password.clear();                
                my_send(ss.str() + "AUTHORIZATION: Enter LOGIN");
                ss.str("");
                while (login.empty())
                {                    
                 login = my_receive();
                }
               
                my_send("Enter PASSWORD");
                while (password.empty())
                {                    
                 password = my_receive();
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
                }
                else n = false;
            }
            name = user[user_ind].get_name();            
            ss << "\nHELLO " + name + "!\n";            

            choice = request <string, size_t>(ss.str () + "ADD a CONTACT: press key 3, CHOOSE a CONTACT: press key 4", 3, 4, user.size());
            ss.str("");
            if (choice == 4 && group.size() == 0)
            {               
              ss << "You don't have contacts yet\n";
              choice = 3;
            }
            n = true;            
            if (choice == 4 && group.size() > 0)
            {
                for (size_t i = 0; i < group.size(); i++)
                {
                    for (size_t j = 0; j < group[i].size(); j++)
                    {
                        if (group[i][j] == user[user_ind].get_name())
                        {
                            n = false;
                            break;
                        }
                    }if (n == false) break;
                }

                if (n)
                {                    
                 ss << "You don't have contacts yet\n";
                 choice = 3;
                 break;
                }
            }

            break;

        case 3:
            //to prevent entry to case 3 if the user already lists in all groups                  
            presence = 0;//number of groups where this user lists
            for (size_t i = 0; i < group.size(); i++)
                for (size_t j = 0; j < group[i].size(); j++)
                    if (group[i][j] == name)
                    {
                        presence++;
                    }
            //cheks is there opporutity to create more contacts
            if (presence >= Presence(user.size()))
            {                
             ss << user[user_ind].get_name() + ", you are already in touch with all possible contacts";
             choice = 4;
            }
            n = true;
            if (choice != 4)
            {
                while (n)//crearing of contacts
                {                    
                 contact_name.clear();                  
                 my_send(ss.str() + "CREATE a CONTACT\nEnter the NAME of CONTACT\n");
                 ss.str("");
                 while (contact_name.empty())
                    {
                     contact_name = my_receive();
                    }
                    //Chat with yourself is impossible
                    if (user[user_ind].get_name() == contact_name)
                    {                        
                      ss << "!Chat with yourself is impossible\n";
                      continue;
                    }
                    //  the name is among of the registered
                    for (size_t i = 0; i < user.size(); i++)
                    {
                        if (user[i].get_name() == contact_name)
                        {
                            n = false;
                            break;
                        }
                    }
                    //UNREGISTERED user
                    if (n)
                    {                       
                     ss << "UNREGISTERED user. Try again\n";
                     contact_name.clear();
                    }
                    else n = false;

                    //if This USER has already been ADDED to this group
                    if (temp.size() != 0)
                        for (size_t i = 0; i < temp.size(); i++)
                            //if This USER has already been ADDED to this group
                            if (find(temp.begin(), temp.end(), contact_name) != temp.end())
                            {                              
                             ss << "This USER has already been ADDED to this group\n";
                             n = true;
                             break;
                            }
                }
                name = user[user_ind].get_name();
                if (temp.size() == 0)
                    temp.push_back(name);//authorized user's name -> temp
                    temp.push_back(contact_name);//contact name -> temp
                    sort(temp.begin(), temp.end());                   
                    ss << contact_name + " was ADDED\n"; 
                    choice = request <string, size_t>
                    (ss.str() + "ADD a CONTACT to THIS group: press key 3, CHOOSE a CONTACT: press key 4", 3, 4, user.size());
                    ss.str("");
                //if ALL the USERS have already been ADDED to this group and number of groups == 0
                if ((choice == 3 || choice == 4) && temp.size() >= user.size() && group.size() == 0)
                {                    
                    group.push_back(temp);                                      
                    for (size_t s = 0; s < temp.size(); s++)
                    ss << to_string (s + 1) + ". " + temp[s] + '\n';                    
                    ss << "Your new group:\n" + ss.str() + "All registred USERS are in this group\n";
                    choice = 4;
                    temp.clear();
                    break;              
                }
                //if ALL the USERS have already been ADDED to this group 
                //and there are number of groups more than 0
                if ((choice == 3 || choice == 4) && temp.size() >= user.size() && group.size() > 0)
                {
                    n = true;
                    for (size_t i = 0; i < group.size(); i++)//check if This Group is already exists                           
                        if (temp == group[i])
                        {                            
                          ss << "This Group is already exists, it's NUMBER is " + to_string(i) + "\nAll registred USERS are in this group\n";
                          choice = 4;
                          temp.clear();
                          n = false;
                          break;
                        }
                    if (n)
                    {  
                        group.push_back(temp);                                   
                        for (size_t s = 0; s < temp.size(); s++)
                        ss << to_string (s + 1) + ". " + temp[s] + '\n';                    
                        ss << "Your new group:\n" + ss.str() + "All registred USERS are in this group\n";
                        choice = 4;
                        temp.clear();
                        break;   
                    }
                }
                //If NOT all users were involved in the group and user goes to choose next contact                          
                else if ((choice == 4) && (temp.size() < user.size()) && (group.size() > 0))
                {
                    //check if This Group is already exists
                    n = true;
                    for (size_t i = 0; i < group.size(); i++)
                        //if there is the same group                            
                        if (temp == group[i])
                        {                            
                         ss << "This Group is already exists, it's NUMBER is " + to_string(i) + '\n';
                         n = false;
                         temp.clear();
                         break;
                        }

                    if (n)
                    {     //Temp -> Group
                        group.push_back(temp);                                             
                        for (size_t s = 0; s < temp.size(); s++)                         
                         ss<<  to_string (s + 1) + ". " + temp[s] + '\n';
                         ss << "Your new group:\n" + ss.str();
                        temp.clear();
                        break;
                    }
                }
                //if the group in temp was created but number of groups == 0 
                else if ((choice == 4) && (temp.size() < user.size()) && (group.size() == 0))
                {
                    group.push_back(temp);
                                              
                        for (size_t s = 0; s < temp.size(); s++)                         
                         ss << to_string (s + 1) + ". " + temp[s] + '\n';
                         ss << "Your new group:\n" + ss.str();
                        temp.clear();
                        break;
                }
                //If we return in 3 to add a contact in this group
                //and NOT all users were involved in this group 
                else if (choice == 3 && temp.size() < user.size())
                {
                                     
                    for (size_t s = 0; s < temp.size(); s++)                      
                        ss << to_string (s + 1) + ". " + temp[s] + '\n';
                        ss << "Your new group:\n" + ss.str();

                    n = true;
                }
            }
            break;

        case 4://choose a contact      

            choice = request <string, size_t>
                (ss.str() + "CHAT with your contacts: press key 4, MESSAGE for everyone: press key 6", 4, 6, user.size());
                ss.str("");
            if (choice == 6) break;          

            {
            
            ss << "Your contacts (groups):" << endl;
            for (size_t i = 0; i < group.size(); i++)
                for (size_t j = 0; j < group[i].size(); j++)
                    if (group[i][j] == user[user_ind].get_name())
                    {
                        ss << '\n' << i << ". ";
                        for (size_t k = 0; k < group[i].size(); k++)
                            ss << group[i][k] << " ";
                        ss << '\n';
                    }ss << '\n';

                

            }

            do
            {                
                my_send(ss.str() + "Choose NUMBER of CONTACT (GROUP)\n");
                ss.str("");       

                num_of_contact = (size_t)stoi(my_receive());                    
                if (num_of_contact >= group.size() || num_of_contact < 0)
                {          
                 ss << "Error: INVALID number of CONTACT\n";
                }                
            } while (num_of_contact >= group.size() || num_of_contact < 0);
            //check if this USER belongs to this contact
            if (num_of_contact < group.size())
            {
                n = true;
                for (size_t j = 0; j < group[num_of_contact].size(); j++)
                    if (group[num_of_contact][j] == user[user_ind].get_name())
                    {
                        choice = 5;
                        n = false;
                        break;
                    }

                if (n)
                {                    
                 ss << "It is NOT YOUR GROUP. Try again\n";
                 choice = 4;
                }
            }

            break;

        case 5://to send message

            if (num_of_contact < mess.size())
            {   
                 for (size_t i = 0; i < mess[num_of_contact].size(); i++)
                {                  
                    ss << num_of_contact << ". " << mess[num_of_contact][i] << '\n';                    
                }
            }   
            while (choice == 5)
            {                
                my_send (ss.str() + "\nTEXT your message");  
                ss.str("");             
                message = my_receive();
                temp_mes.push_back(user[user_ind].get_name() + ": " + message);
                choice = next_request<size_t>();//to choose further destination
            }
            //if there is new group without messages
            if (mess.size() <= num_of_contact)
                mess.push_back(temp_mes);
            else
            {
                for (size_t i = 0; i < temp_mes.size(); i++)
                    mess[num_of_contact].push_back(temp_mes[i]);
            }
            temp_mes.clear();
            if (choice == 7) ch = false;
            break;

        case 6://Message for everyone
            
            my_send (ss.str() + "\nTEXT your message\n");            
            message = my_receive();
            //to push messages to all existing groups with messages
            for (size_t i = 0; i < mess.size(); i++)
            {
                mess[i].push_back(user[user_ind].get_name() + ": " + message);
            }
            //to push message to new group without messages
            if (group.size() > mess.size())
            {
                temp_mes.push_back(user[user_ind].get_name() + ": " + message);
                mess.push_back(temp_mes);
            }
            temp_mes.clear();
            choice = next_request<size_t>();//to choose further destination
            if (choice == 7) ch = false;
            break;
        }
    }

    if (ch == false)
    {
        my_send ("#");
        cout << "socket is to close" << endl;
        close(socket_file_descriptor);
    }

    return 0;
}     




