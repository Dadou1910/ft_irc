#include "../Server.hpp"

bool IRCServer::handlePing(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check if the client is registered
    if (!client.registered) {
        sendMessage(fd, ERR_NOTREGISTERED_MSG(servername));
        return false;
    }

    // Check if the PING command has a token
    if (tokens.size() < 2) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "PING"));
        return false;
    }

    // Get the token from the command
    std::string token = tokens[1];

    // Send the PONG response: :servername PONG servername token
    std::string pongMsg = ":" + servername + " PONG " + servername + " " + token + "\r\n";
    sendMessage(fd, pongMsg);
    return true;
}