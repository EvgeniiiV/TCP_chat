#pragma once


template <typename T>class User
{
private:
    int _connection;
    T _name, _login, _pass;    
        
public:    
    User();
    User(T name, T login, T pass, int connection);
    T get_name() const;
    T get_log_pas() const;
    int get_FD_by_name() const {return _connection;}
    T get_name_by_FD() const {return _name;}
};

