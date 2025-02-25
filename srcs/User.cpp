#include "../includes/User.hpp"

User::User() : socketFd(-1), isAuthenticated(false) {}

User::~User() {
    if (socketFd >= 0)
        close(socketFd);
}

User::User(const User& src) {
    this->nickname = src.nickname;
    this->username = src.username;
    this->password = src.password;
    this->socketFd = src.socketFd;
    this->isAuthenticated = src.isAuthenticated;
}

User& User::operator=(const User& rhs) {
    if (this != &rhs) {
        this->username = rhs.username;
        this->password = rhs.password;
        this->socketFd = rhs.socketFd;
        this->isAuthenticated = rhs.isAuthenticated;
    }
    return *this;
}


void User::setNickname(const std::string& nick) {
    nickname = nick;
}

void User::setUsername(const std::string& user) {
    username = user;
}

void User::setAuthenticationStatus(bool status) {
    isAuthenticated = status;
}

void User::setSocketFd(int fd) {
    socketFd = fd;
}

void User::sendMessage(const std::string& msg) const {
    if (socketFd >= 0)
        send(socketFd, msg.c_str(), msg.length(), 0);
}

std::string User::getNickname() const {
    return nickname;
}

std::string User::getUsername() const {
    return username;
}

bool User::getAuthenticationStatus() const {
    return isAuthenticated;
}

int User::getSocketFd() const {
    return socketFd;
}

void User::handleCmd(const std::string& command) {
    std::stringstream ss(command);
    std::string cmd;

    if (!(ss >> cmd)) {
        sendMessage(ERR_NEEDMOREPARAMS(getNickname(), "UNKNOWN"));
        return;
    }

    if (cmd == "NICK") {
        std::string newNick;
        if (!(ss >> newNick) || newNick.size() == 0) {  // C++98: No empty()
            sendMessage(ERR_NONICKNAMEGIVEN(getNickname()));
            return;
        }
        setNickname(newNick);
        sendMessage(NICK(getNickname(), getUsername(), newNick));
    }
    else if (cmd == "JOIN") {
        std::string channelName;
        if (!(ss >> channelName) || channelName.size() == 0) {
            sendMessage(ERR_NEEDMOREPARAMS(getNickname(), "JOIN"));
            return;
        }
        sendMessage(JOIN(getNickname(), getUsername(), channelName));
    }
    else if (cmd == "PRIVMSG") {
        std::string target, msg;
        if (!(ss >> target)) {
            sendMessage(ERR_NOSUCHNICK(getNickname(), target));
            return;
        }
        char c;
        while (ss.get(c) && (c == ' ')) {} // Skip leading spaces
        if (ss.eof()) {
            sendMessage(ERR_NEEDMOREPARAMS(getNickname(), "PRIVMSG"));
            return;
        }
        msg = c;  // Start message with first non-space character
        std::string rest;
        while (std::getline(ss, rest)) {
            msg += "\n" + rest;  // Append full message
        }
        sendMessage(PRIVMSG(getNickname(), getUsername(), target, msg));
    }
    else {
        sendMessage(ERR_UNKNOWNMODE(cmd, "UNKNOWN"));
    }
}


