#include "../Server.hpp"

bool IRCServer::handleUser(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (USER <username> <hostname> <servername> <realname>)
    if (tokens.size() < 5) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "USER"));
        return false;
    }

    // Check if the client is already registered
    if (client.registered) {
        sendMessage(fd, ERR_ALREADYREGISTERED_MSG(servername, client.nick));
        return false;
    }

    // Set client user and host
    client.user = tokens[1];
    client.host = tokens[3];
    checkRegistration(fd);

    return true;
}