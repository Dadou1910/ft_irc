#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>



class Client {
public:
    std::string nick;
    std::string user;
    std::string host;
    std::string realname;
    bool registered;
    int fd;
    std::string clientBuffer;
    bool hasPassword; // New flag to track if the correct password was provided

    // Constructor declaration
    Client();
    Client(const std::string& n, const std::string& u, const std::string& h, const std::string& r, bool reg, int f);
};

#endif