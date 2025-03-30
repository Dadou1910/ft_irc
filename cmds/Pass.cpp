#include "../Server.hpp"

bool IRCServer::handlePass(int fd, Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "PASS"));
        return false;
    }
    if (client.registered) {
        sendMessage(fd, ERR_ALREADYREGISTERED_MSG(servername, client.nick));
        return false;
    }
    std::cout << "[DEBUG] Checking password: received=" << tokens[1] << ", expected=" << password << std::endl;
    if (tokens[1] != password) {
        sendMessage(fd, ":" + servername + " 464 " + (client.nick.empty() ? "*" : client.nick) + " :Password incorrect\r\n");
        // Do not close the connection; let the client continue
        return false;
    }
    sendMessage(fd, ":" + servername + " NOTICE * :Password accepted\r\n");
    client.hasPassword = true; // Add a flag to track if the correct password was provided
    return true;
}