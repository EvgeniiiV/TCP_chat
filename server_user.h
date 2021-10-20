#pragma once

template <typename T>class User
{
private:
    T _name, _login, _pass;
public:
    User();
    User(T name, T login, T pass);
    T get_name() const;
    T get_log_pas() const;
};

