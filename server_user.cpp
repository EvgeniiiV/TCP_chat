#include "server_user.h"

template<typename T>User<T>::User() = default;
template<typename T>User<T>::User
(T name, T login, T pass) : _name(name), _login(login), _pass(pass){}
template<typename T> T User<T>::get_name() const { return _name; }
template<typename T> T User<T>::get_log_pas() const { return _login + _pass; }
