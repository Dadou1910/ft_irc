#include "../Server.hpp"

bool IRCServer::handlePrivmsg(int fd, Client& client, const std::vector<std::string>& tokens, const std::string& cmd) {
    // Check for minimum parameters (PRIVMSG <target> <message>)
    if (tokens.size() < 3) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "PRIVMSG"));
        return false;
    }

    std::string target = tokens[1];
    std::string msg = cmd.substr(cmd.find(tokens[2]));

    // Handle channel message
    if (target[0] == '#') {
        if (channels.find(target) != channels.end() && channels[target].members.find(fd) != channels[target].members.end()) {
            for (std::map<int, bool>::iterator it = channels[target].members.begin(); it != channels[target].members.end(); ++it) {
                if (it->first != fd) {
                    sendMessage(it->first, ":" + client.nick + "!" + client.user + "@" + client.host + " PRIVMSG " + target + " " + msg + "\r\n");
                }
            }
        }
    }
    // Handle private message to a user
    else {
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.nick == target) {
                sendMessage(it->first, ":" + client.nick + "!" + client.user + "@" + client.host + " PRIVMSG " + target + " " + msg + "\r\n");
                break;
            }
        }
    }

    return true;
}