#include "../Server.hpp"

#include "../Server.hpp"

bool IRCServer::handleKick(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (KICK <channel> <user> [reason])
    if (tokens.size() < 3) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "KICK"));
        return false;
    }

    std::string chan = tokens[1];
    std::string target_nick = tokens[2];
    std::string reason = (tokens.size() > 3) ? tokens[3] : "No reason given";
    if (!reason.empty() && reason[0] == ':') {
        reason = reason.substr(1);
    }

    // Verify the channel exists and the client is a member
    if (channels.find(chan) == channels.end() || channels[chan].members.find(fd) == channels[chan].members.end()) {
        sendMessage(fd, ERR_NOTONCHANNEL_MSG(servername, client.nick, chan));
        return false;
    }

    // Verify the client is an operator
    if (!channels[chan].members[fd]) {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED_MSG(servername, client.nick, chan));
        return false;
    }

    // Find the target user in the channel
    int target_fd = -1;
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.nick == target_nick && channels[chan].members.find(it->first) != channels[chan].members.end()) {
            target_fd = it->first;
            break;
        }
    }

    if (target_fd == -1) {
        sendMessage(fd, ":" + std::string(servername) + " " + ERR_NOSUCHNICK + " " + client.nick + " " + target_nick + " :No such nick/channel\r\n");
        return false;
    }

    // Construct the KICK message
    std::string msg = ":" + client.nick + "!" + client.user + "@" + client.host + " KICK " + chan + " " + target_nick + " :" + reason + "\r\n";

    // Broadcast the KICK message to all channel members (including the target) using the target's fd
    broadcastToSharedChannels(target_fd, msg);

    // Remove the target from the channel
    channels[chan].members.erase(target_fd);

    // If the channel is now empty, remove it
    if (channels[chan].members.empty()) {
        channels.erase(chan);
    }

    return true;
}