#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
using namespace std; 
#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 20000 // Будем использовать этот номер порта
 
int socket_file_descriptor, connection;
struct sockaddr_in serveraddress, client;
char message[MESSAGE_LENGTH];

int main()
{
    // Создадим сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_file_descriptor == -1){
        cout << "Creation of Socket failed!" << endl;
        exit(1);
    } 
    // Установим адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Установим соединение с сервером
    connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if(connection == -1){
        cout << "Connection with the server failed!" << endl;
        exit(1);
    }
    
    while(1)
    {
      // Ждем ответа от сервера     
      bzero(message, MESSAGE_LENGTH); 
      recv(socket_file_descriptor, message, MESSAGE_LENGTH,0);

      cout << message << endl; 
      if ((strncmp(message, "#", 1)) == 0)
        {            
            cout << "Client Exit." << endl;
            close(socket_file_descriptor);
            return 0;
        }     
    
      bzero(message, MESSAGE_LENGTH);
      string snd_message;
      while (snd_message.empty())//to prevent empty input        
      {
        snd_message.clear();
        getline (cin, snd_message);
      }
      strncpy (message, snd_message.c_str(), MESSAGE_LENGTH);

      if ((strncmp(message, "#", 1)) == 0)
        {
            write(socket_file_descriptor, message, MESSAGE_LENGTH);
            cout << "Client Exit." << endl;
            break;
        }
      ssize_t bytes = send(socket_file_descriptor, message, sizeof (snd_message), 0);
      if (bytes < 0)
      perror ("send");
    
    }
    // закрываем сокет, завершаем соединение
    close(socket_file_descriptor);
    return 0;
}
