#include "../Server.hpp"


bool IRCServer::handleInvite(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (INVITE <nickname> <channel>)
    if (tokens.size() < 3) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "INVITE"));
        return false;
    }

    std::string target_nick = tokens[1];
    std::string channel = tokens[2];
    if (channel[0] != '#') {
        channel = "#" + channel;
    }

    // Verify the channel exists and the client is a member
    std::map<std::string, Channel>::iterator chan_it = channels.find(channel);
    if (chan_it == channels.end() || chan_it->second.members.find(fd) == chan_it->second.members.end()) {
        sendMessage(fd, ERR_NOTONCHANNEL_MSG(servername, client.nick, channel));
        return false;
    }

    // Optional: Require the inviter to be an operator (comment out if not desired)
    if (!chan_it->second.members[fd]) {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED_MSG(servername, client.nick, channel));
        return false;
    }

    // Find the target user
    int target_fd = -1;
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.nick == target_nick) {
            target_fd = it->first;
            break;
        }
    }

    if (target_fd == -1) {
        sendMessage(fd, ":" + std::string(servername) + " " + ERR_NOSUCHNICK + " " + client.nick + " " + target_nick + " :No such nick/channel\r\n");
        return false;
    }

    // Check if the target user is already in the channel
    if (chan_it->second.members.find(target_fd) != chan_it->second.members.end()) {
        sendMessage(fd, ERR_USERONCHANNEL_MSG(servername, client.nick, target_nick, channel));
        return false;
    }

    // Add the target user to the channel's invite list
    chan_it->second.invited.insert(target_nick);
    std::cout << "[DEBUG] Added " << target_nick << " to invite list for channel " << channel << std::endl;

    // Send INVITE message to the target user
    std::string invite_msg = ":" + client.nick + "!" + client.user + "@" + client.host + " INVITE " + target_nick + " :" + channel + "\r\n";
    sendMessage(target_fd, invite_msg);
    std::cout << "[DEBUG] Sent INVITE message to fd=" << target_fd << ": " << invite_msg << std::endl;

    // Send RPL_INVITING to the inviter
    sendMessage(fd, RPL_INVITING_MSG(servername, client.nick, target_nick, channel));
    std::cout << "[DEBUG] Sent RPL_INVITING to fd=" << fd << ": " << target_nick << " invited to " << channel << std::endl;

    return true;
}