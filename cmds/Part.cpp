#include "../Server.hpp"

bool IRCServer::handlePart(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (PART <channel>)
    if (tokens.size() < 2) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "PART"));
        return false;
    }

    // Normalize channel name
    std::string channel = tokens[1];
    if (channel[0] != '#') {
        channel = "#" + channel;
    }

    // Verify the channel exists and the client is a member
    std::map<std::string, Channel>::iterator chan_it = channels.find(channel);
    if (chan_it == channels.end() || chan_it->second.members.find(fd) == chan_it->second.members.end()) {
        sendMessage(fd, ERR_NOTONCHANNEL_MSG(servername, client.nick, channel));
        return false;
    }

    // Parse the reason (tokens[2] onward, handle spaces)
    std::string reason = "Leaving";
    if (tokens.size() > 2) {
        reason = tokens[2];
        // Remove leading colon if present (e.g., ":wel" -> "wel")
        if (!reason.empty() && reason[0] == ':') {
            reason = reason.substr(1);
        }
        // Concatenate remaining tokens for multi-word reasons
        for (size_t i = 3; i < tokens.size(); ++i) {
            reason += " " + tokens[i];
        }
    }

    // Ensure user and host are set for the PART message
    std::string user = client.user.empty() ? "user" : client.user;
    std::string host = client.host.empty() ? "localhost" : client.host;

    // Construct the PART message
    std::string msg = ":" + client.nick + "!" + user + "@" + host + " PART " + channel + " :" + reason + "\r\n";
    std::cout << "[DEBUG] Sending PART message for fd=" << fd << ": " << msg << std::endl;

    // Broadcast the PART message to all channel members (including the user leaving)
    broadcastToSharedChannels(fd, msg);

    // Remove the client from the channel
    chan_it->second.members.erase(fd);
    std::cout << "[DEBUG] Removed fd=" << fd << " (" << client.nick << ") from channel " << channel << ". Members left: " << chan_it->second.members.size() << std::endl;

    // If the channel is empty, remove it
    if (chan_it->second.members.empty()) {
        channels.erase(channel);
        std::cout << "[DEBUG] Channel " << channel << " is empty and has been removed" << std::endl;
        std::cout << "[NOTIFY] Channel " << channel << " has been deleted due to no remaining members after " << client.nick << " parted." << std::endl;
    } else {
        // Send updated user list to remaining channel members
        std::string names;
        for (std::map<int, bool>::iterator it = chan_it->second.members.begin(); it != chan_it->second.members.end(); ++it) {
            names += (it->second ? "@" : "") + clients[it->first].nick + " ";
        }
        for (std::map<int, bool>::iterator it = chan_it->second.members.begin(); it != chan_it->second.members.end(); ++it) {
            int member_fd = it->first;
            Client& member = clients[member_fd];
            sendMessage(member_fd, RPL_NAMREPLY_MSG(servername, member.nick, channel, names));
            sendMessage(member_fd, RPL_ENDOFNAMES_MSG(servername, member.nick, channel));
        }
        std::cout << "[DEBUG] Updated user list sent to remaining members of " << channel << ": " << names << std::endl;
    }

    return true;
}