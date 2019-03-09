#include <string>

//NOTE: AF_UNIX domain stream sockets only.
enum class app_type {client, server};

class socket
{
    int _socket;
    app_type _type = app_type::client;
    std::string _name;
    bool _failed = false;
    void createClient();
    void createServer();

    public:
        socket();
        socket(const int socket) : _socket(socket) {}
        socket(const app_type type, const std::string& name);
        ~socket();
        socket accept();
        bool recv(void *buffer, const std::size_t len);
        bool send(void *buffer, const std::size_t len);
        bool status();
};
