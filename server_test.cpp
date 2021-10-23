#include<iostream>
using namespace std;
#include<string>
#include "server_chat.cpp"
#include<vector>
#include "server_user.cpp"
#include<algorithm> 
#include <sstream>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h> 

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 7777 // Будем использовать этот номер порта

#define MaxUserCount 10//limit of Users
#define MaxGroupCount 100//limit of contacts (groups of users) 
#define MaxMessageCount 200//limit of messages in one group
using std::cout;

struct sockaddr_in serveraddress, client;
socklen_t length;
int socket_file_descriptor, connection, bind_status, connection_status;
char message[MESSAGE_LENGTH];    

//server -> client
/*void my_send(string mes)
{     
    bzero(message, MESSAGE_LENGTH);
    strncpy (message, mes.c_str(), MESSAGE_LENGTH);
    //size_t to_send = strlen(message);
    ssize_t bytes = send (connection, message, MESSAGE_LENGTH, 0);
    sleep(0.1);    
    // Если передали >= 0  байт, значит пересылка прошла успешно
        if(bytes >= 0)  {
           cout << "Data successfully sent to the client.!" << strlen(message) << " bytes"<< endl;
        }
    
}  */
void my_send(string mes)
{   cout << "sent to my_send:\n " << mes << "\nsize() = " << mes.size()<< endl;//mes.size() is incresed by MESSAGE_LENGTH   after each iteration: 1037->2011->30... why? 
    bzero(ch_message, MESSAGE_LENGTH);
    const char* temp  = mes.data(); //or   const char* temp  = mes.c_str(); - gives exactly the same result! 
 
    cout << temp <<"\nstrlen (temp) = " <<strlen (temp)  << endl; // => gives only the first line (21 or 23 bytes)!!!!!
    strncpy (ch_message, temp, MESSAGE_LENGTH);
    ssize_t bytes = write (connection, ch_message, sizeof(ch_message));
    // Если передали >= 0  байт, значит пересылка прошла успешно
    if(bytes >= 0)
      {
        cout << "Data successfully sent to the client.!" << endl;
      }
    
}   



//client -> server
string my_receive ()
{    
    bzero(message, MESSAGE_LENGTH);
    read(connection, message, sizeof(message));
        if (strncmp("end", message, 3) == 0) {
            cout << "Client Exited." << endl;
            cout << "Server is Exiting..!" << endl;
            close(socket_file_descriptor);                
        }
    cout << message << endl;
    string s_read = string (message, sizeof(message));//for received msg     
    return s_read;
    
}   
//client -> client
void cc_send(string mes)
{     
    bzero(message, MESSAGE_LENGTH);
    strncpy (message, mes.c_str(), MESSAGE_LENGTH);
    //size_t to_send = strlen(message);
    ssize_t bytes = send (connection, message, MESSAGE_LENGTH, 0);    
        if(bytes >= 0)  {
           cout << "Data successfully sent to the client.!" << strlen(message) << " bytes"<< endl;
        }
    
}   
//client -> client
string cc_receive ()
{    
    bzero(message, MESSAGE_LENGTH);
    read(connection, message, sizeof(message));
        if (strncmp("end", message, 3) == 0) {
            cout << "Client Exited." << endl;
            cout << "Server is Exiting..!" << endl;
            close(socket_file_descriptor);                
        }
    cout << message << endl;
    string s_read = string (message, sizeof(message));//for received msg     
    return s_read;
    
}   


int main()
{
     // Создадим сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_file_descriptor == -1){
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

    char start []{"Do you want to start chatting? (y/n)"};
    ssize_t bytes = write (connection, start, sizeof(message));
        // Если передали >= 0  байт, значит пересылка прошла успешно
        if(bytes >= 0)  {
           cout << "Start is ok!" << endl;
        }
    bzero(message, MESSAGE_LENGTH);
    read(connection, message, sizeof(message));
            if (strncmp("n", message, 1) == 0) {
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

    //cout << "REGISTRATION" << endl;
    //channel (socket_file_descriptor, "REGISTRATION");
    while (ch)
    {
        switch (choice)
        {
        case 1:
            name.clear();
            n = true;
            while (n)
            {                
                    my_send ("REGISTRATION: Enter your NAME");
                    while (name.empty())
                {    
                    name = my_receive(/*socket_file_descriptor*/);                
                   
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
                    my_send ("!This name is already used. Choose another name");
                    name.clear();
                    n = true;
                }
                else n = false;
            }
            n = true;
            while (n)
            {
                login.clear(); password.clear();                
                my_send ("Enter LOGIN");
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
                    my_send("Change the login and try again");
                    login.clear(); password.clear();
                    n = true;
                }
                else  n = false;
            }
            choice = request <string, size_t>("to continue REGISTRATION press key 1, for AUTHORIZATION press key 2", 1, 2, user.size());
            user.push_back(User<string>(name, login, password));
            
            {
                stringstream ss;
                ss << "!REGISTERED USERS:" << '\n';
                for (size_t i = 0; i < user.size(); i++)
                    ss << i + 1 << ". " << user[i].get_name() << '\n';
                    string to_send = ss.str();
                    my_send(to_send); 
                    cout << to_send << "/" << sizeof(to_send) << "bytes" << endl;//just to compare server's and client's options              
            }
            break;

        case 2:
            //cout << "AUTHORIZATION" << endl;
            n = true;
            while (n)
            {
                name.clear(); login.clear(); password.clear();               
                my_send("AUTHORIZATION: Enter LOGIN");
                while (login.empty())
                {
                    //getline(cin, login);
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
                    //cout << "Invalid password or login. Try again" << endl;
                    my_send("!Invalid password or login. Try again");
                    login.clear(); password.clear();
                    n = true;
                }
                else n = false;
            }
            name = user[user_ind].get_name();            
            {string grit = "!\nHELLO " + name + "!\n\n";
            my_send (grit);}

            choice = request <string, size_t>("ADD a CONTACT: press key 3, CHOOSE a CONTACT: press key 4", 3, 4, user.size());

            //check if this USER has contacts (if choice == 4 && number of groups = 0)
            if (choice == 4 && group.size() == 0)
            {                
                my_send("!You don't have contacts yet");
                choice = 3;
            }
            n = true;
            //check if this USER has contacts (if choice == 4 && numOf_groups = 0)
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
                    my_send("!You don't have contacts yet");
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
                my_send("!" + user[user_ind].get_name() + ", you are already in touch with all possible contacts");
                choice = 4;
            }
            n = true;
            if (choice != 4)
            {
                while (n)//crearing of contacts
                {                   
                    contact_name.clear();                    
                    my_send("CREATE a CONTACT\nEnter the NAME of CONTACT");
                    while (contact_name.empty())
                    {                       
                       contact_name = my_receive();
                    }
                    //Chat with yourself is impossible
                    if (user[user_ind].get_name() == contact_name)
                    {                       
                        my_send("!Chat with yourself is impossible");
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
                        my_send("!UNREGISTERED user. Try again");
                        contact_name.clear();
                    }
                    else n = false;

                    //if This USER has already been ADDED to this group
                    if (temp.size() != 0)
                        for (size_t i = 0; i < temp.size(); i++)
                            //if This USER has already been ADDED to this group
                            if (find(temp.begin(), temp.end(), contact_name) != temp.end())
                            {                                
                                my_send ("!This USER has already been ADDED to this group.");
                                n = true;
                                break;
                            }
                }
                name = user[user_ind].get_name();
                if (temp.size() == 0)
                    temp.push_back(name);//authorized user's name -> temp
                    temp.push_back(contact_name);//contact name -> temp
                    sort(temp.begin(), temp.end());
                    //cout << contact_name << " was ADDED" << endl;
                    my_send("!\n" + contact_name + " was ADDED"); //!!!!!!! comes to client without "was added"
                    choice = request <string, size_t>
                    ("ADD a CONTACT to THIS group: press key 3, CHOOSE a CONTACT: press key 4", 3, 4, user.size());

                //if ALL the USERS have already been ADDED to this group and number of groups == 0

                if ((choice == 3 || choice == 4) && temp.size() >= user.size() && group.size() == 0)
                {                    
                    group.push_back(temp);
                    string newgroup;
                    //cout << "!Your new group:" << endl;
                    for (size_t s = 0; s < temp.size(); s++)
                    newgroup = newgroup + to_string (s + 1) + ". " + temp[s] + '\n';
                    // cout << s + 1 << ". " << temp[s] << endl;
                    //cout << "!All registred USERS are in this group" << endl;
                    my_send ("!Your new group:\n" + newgroup + "All registred USERS are in this group\n");
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
                            my_send ("This Group is already exists, it's NUMBER is " + to_string(i) + "\nAll registred USERS are in this group\n");
                            choice = 4;
                            temp.clear();
                            n = false;
                            break;
                        }
                    if (n)
                    {  
                        group.push_back(temp);
                        string newgroup;
                    //cout << "!Your new group:" << endl;
                        for (size_t s = 0; s < temp.size(); s++)
                        newgroup = newgroup + to_string (s + 1) + ". " + temp[s] + '\n';                    
                        my_send ("!Your new group:\n" + newgroup + "All registred USERS are in this group\n");
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
                            my_send ("This Group is already exists, it's NUMBER is " + to_string(i) + '\n');
                            n = false;
                            temp.clear();
                            break;
                        }

                    if (n)
                    {     //Temp -> Group
                        group.push_back(temp);
                        string newgroup;                      
                        for (size_t s = 0; s < temp.size(); s++)                        
                         newgroup = newgroup + to_string (s + 1) + ". " + temp[s] + '\n';
                         my_send("!Your new group:\n" + newgroup);
                        temp.clear();
                        break;
                    }
                }
                //if the group in temp was created but number of groups == 0 
                else if ((choice == 4) && (temp.size() < user.size()) && (group.size() == 0))// !!!!!
                {    
                    group.push_back(temp);
                        string newgroup;                       
                        for (size_t s = 0; s < temp.size(); s++)                         
                         newgroup = newgroup + to_string (s + 1) + ". " + temp[s] + '\n';
                         my_send("!Your new group:\n" + newgroup);
                        temp.clear();
                        break;
                }                
                else if (choice == 3 && temp.size() < user.size())
                {
                    string newgroup;                  
                    for (size_t s = 0; s < temp.size(); s++)                      
                        newgroup = newgroup + to_string (s + 1) + ". " + temp[s] + '\n';
                        my_send("!Your new group:\n" + newgroup);
                    n = true;
                }
            }
            break;

        case 4://choose a contact      

            choice = request <string, size_t>
                ("CHAT with your contacts: press key 4, MESSAGE for everyone: press key 6", 4, 6, user.size());
            if (choice == 6) break;      

            {
            stringstream ss;
            ss << "!Your contacts (groups):" << endl;
            for (size_t i = 0; i < group.size(); i++)
                for (size_t j = 0; j < group[i].size(); j++)
                    if (group[i][j] == user[user_ind].get_name())
                    {
                        ss << '\n' << i << ". ";
                        for (size_t k = 0; k < group[i].size(); k++)
                            ss << group[i][k] << " ";
                        ss << '\n';
                    }ss << '\n';

            string to_out = ss.str();
            my_send(to_out);      

            }

            do
            {                
                my_send("Choose NUMBER of CONTACT (GROUP)");            
                num_of_contact = (size_t)stoi(my_receive());                    
                //prevents obviously wrong input
                if (num_of_contact >= group.size() || num_of_contact < 0)
                {                  
                    
                    my_send ("!Error: INVALID number of CONTACT\n");
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
                    my_send ("!It is NOT YOUR GROUP. Try again\n");
                    choice = 4;
                }
            }

            break;

        case 5://to send message

            if (num_of_contact < mess.size())
            {   stringstream ss;
                 for (size_t i = 0; i < mess[num_of_contact].size(); i++)
                {                    
                    ss << num_of_contact << ". " << mess[num_of_contact][i] << '\n';
                    string to_out = ss.str();
                    my_send ("!" + to_out);
                    cout << to_out;

                }
            }   
            while (choice == 5)
            {                
                my_send ("\nTEXT your message");                
                message = cc_receive();
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
            my_send ("\nTEXT your message");           
             message = cc_receive();
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

 
    return 0;
}


