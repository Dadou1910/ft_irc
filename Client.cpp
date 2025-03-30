#include "Client.hpp"

Client::Client()
    : nick(""), user(""), host(""), realname(""), registered(false), fd(-1), clientBuffer(""), hasPassword(false) {}

Client::Client(const std::string& n, const std::string& u, const std::string& h, const std::string& r, bool reg, int f)
    : nick(n), user(u), host(h), realname(r), registered(reg), fd(f), clientBuffer(""), hasPassword(false) {}