#include "../Server.hpp"
#include <cctype>
#include <sstream>

bool isSpecialChar(char c) {
    const std::string specialChars = "-[]\\`_^{}|";
    return (specialChars.find(c) != std::string::npos);
}

bool isValidNickname(const std::string& nickname) {
    if (nickname.length() > 9) {
        return false;
    }
    if (nickname[0] == '-' || (nickname[0] >= '0' && nickname[0] <= '9')) {
        return false;
    }
    for (int i = 0; i < (int)nickname.length(); i++) {
        if (!isalnum(nickname[i]) && !isSpecialChar(nickname[i])) {
            return false;
        }
    }
    return true;
}

bool IRCServer::nicknameAlreadyUsed(const std::string& name, const Client& client) {
    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++) {
        if (it->second.nick == name && it->second.fd != client.fd) {
            return true;
        }
    }
    return false;
}

bool IRCServer::handleNick(int fd, Client& client, const std::vector<std::string>& tokens) {
    std::map<int, Client>::iterator it;
    std::string currentNickname = clients[fd].nick;
    std::string newNickname = "";

    if (tokens.size() < 2) {
        std::string nickToUse = client.nick.empty() ? std::string("*") : client.nick;
        sendMessage(fd, ERR_NONICKNAMEGIVEN_MSG(servername, nickToUse));
        return false;
    }

    if (!client.registered) {
        std::stringstream ss;
        std::string str;
        ss << fd;
        ss >> str;
        newNickname = tokens[1] + str;
        clients[fd].nick = newNickname;
    } else {
        newNickname = tokens[1];
        if (newNickname.empty()) {
            std::string nickToUse = client.nick.empty() ? std::string("*") : client.nick;
            sendMessage(fd, ERR_NONICKNAMEGIVEN_MSG(servername, nickToUse));
            return false;
        }
        for (it = clients.begin(); it != clients.end(); it++) {
            if (it->second.nick == newNickname && it->second.fd != client.fd) {
                currentNickname = it->second.nick;
            }
        }
        if (newNickname == currentNickname && client.registered) {
            sendMessage(fd, ERR_NICKNAMEINUSE_MSG(servername, newNickname));
            return false;
        }
        if (!isValidNickname(newNickname)) {
            sendMessage(fd, ERR_ERRONEUSNICKNAME_MSG(servername, currentNickname));
            return false;
        }
        if (client.registered) {
            // Ensure user and host are set for the NICK message
            std::string user = clients[fd].user.empty() ? "user" : clients[fd].user;
            std::string host = clients[fd].host.empty() ? "localhost" : clients[fd].host;
            std::string msg = NICK_MSG(clients[fd].nick, user, host, newNickname);
            std::cout << "[DEBUG] Sending NICK change message for fd=" << fd << ": " << msg << std::endl;
        
            // Check if the client is in any channel
            bool isInChannel = false;
            for (std::map<std::string, Channel>::iterator chan_it = channels.begin(); chan_it != channels.end(); ++chan_it) {
                if (chan_it->second.members.find(fd) != chan_it->second.members.end()) {
                    isInChannel = true;
                    break;
                }
            }
        
            // If the client is in at least one channel, broadcast to all shared channels (including the sender)
            if (isInChannel) {
                broadcastToSharedChannels(fd, msg);
            } else {
                // If the client is not in any channel, send the message only to the client
                sendMessage(fd, msg);
            }
        }
        clients[fd].nick = newNickname; // Simply update the nick
    }

    return true;
}