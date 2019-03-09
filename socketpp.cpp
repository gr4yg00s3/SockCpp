#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include "socketpp.h"

void socket::createClient()
{
    sockaddr_un addr;
    memset(&addr, 0, sizeof(sockaddr_un));

    addr.sun_family = AF_UNIX;
    _name.copy(addr.sun_path, _name.size() + 1);

    int ret = connect(_socket, (const sockaddr*) &addr, sizeof(sockaddr_un));
    if(ret == -1)
    {
        std::cout << "Failed to connect." << std::endl;
        _failed = true;
        return;
    }
}

void socket::createServer()
{
    sockaddr_un addr;
    memset(&addr, 0, sizeof(sockaddr_un));

    addr.sun_family = AF_UNIX;
    _name.copy(addr.sun_path, _name.size() + 1);
    unlink(_name.c_str());

    int ret = bind(_socket, (const sockaddr*) &addr, sizeof(sockaddr_un));
    if(ret == -1)
    {
        std::cout << "Failed to bind socket." << std::endl;
        _failed = true;
        return;
    }

    ret = listen(_socket, 5);
    if(ret == -1)
    {
        std::cout << "Failed to listen." << std::endl;
        _failed = true;
        return;
    }
}

socket::socket(const app_type type, const std::string& name) : _type(type), _name(name)
{
    _socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(_socket == -1)
    {
        std::cout << "Failed to create socket." << std::endl;
        _failed = true;
        return;
    }

    if(_type == app_type::client)
        createClient();
    else if (_type == app_type::server)
        createServer();
    else
        std::cout << "Invalid type." << std::endl;
}

class socket socket::accept()
{
    const int connection = ::accept(_socket, nullptr, nullptr);
    if(connection == -1)
        std::runtime_error(strerror(errno));

    return socket(connection);
}

bool socket::recv(void *buffer, const std::size_t len)
{
    const int ret = ::recv(_socket, buffer, len, 0);
    if(ret == -1)
    {
        return false;
    }
    else
        return true;
}

bool socket::send(void *buffer, const std::size_t len)
{
    const int ret = ::send(_socket, buffer, len, 0);
    if(ret == -1)
        return false;
    else
        return true;
}

bool socket::status()
{
    return _failed;
}

socket::~socket()
{
    close(_socket);

    if(_type == app_type::server)
        unlink(_name.c_str());
}
