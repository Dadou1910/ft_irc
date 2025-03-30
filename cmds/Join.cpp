#include "../Server.hpp"

bool IRCServer::handleJoin(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (JOIN <channel>)
    if (tokens.size() < 2) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "JOIN"));
        return false;
    }

    std::string chan = tokens[1];
    if (chan[0] != '#') {
        chan = "#" + chan;
    }

    // Check if the channel exists
    bool new_channel = (channels.find(chan) == channels.end());
    if (new_channel) {
        channels[chan] = Channel();
        channels[chan].members[fd] = true; // First user becomes operator
    }

    Channel& channel = channels[chan];

    // Check if the channel has a key and verify it
    if (!channel.key.empty()) {
        std::string provided_key = (tokens.size() > 2) ? tokens[2] : "";
        if (provided_key != channel.key) {
            sendMessage(fd, ":" + std::string(servername) + " 475 " + client.nick + " " + chan + " :Cannot join channel (+k)\r\n");
            if (new_channel) {
                channels.erase(chan); // Remove the channel if it was just created
            }
            return false;
        }
    }

    // Check if the channel is invite-only and if the user is invited
    if (channel.inviteOnly) {
        if (channel.invited.find(client.nick) == channel.invited.end()) {
            sendMessage(fd, ":" + std::string(servername) + " 473 " + client.nick + " " + chan + " :Cannot join channel (+i)\r\n");
            if (new_channel) {
                channels.erase(chan); // Remove the channel if it was just created
            }
            return false;
        }
        // Remove the user from the invite list after they join
        channel.invited.erase(client.nick);
        std::cout << "[DEBUG] Removed " << client.nick << " from invite list for channel " << chan << std::endl;
    }

    // Add the user to the channel if not already a member
    if (channel.members.find(fd) == channel.members.end()) {
        channel.members[fd] = (new_channel ? true : false); // First user is operator, others are not
    }

    // Send JOIN message to all channel members
    std::string join_msg = ":" + client.nick + "!" + client.user + "@" + client.host + " JOIN :" + chan + "\r\n";
    broadcastToSharedChannels(fd, join_msg); // Specify the channel for broadcast

    // Send topic if it exists
    if (!channel.topic.empty()) {
        sendMessage(fd, ":" + std::string(servername) + " 332 " + client.nick + " " + chan + " :" + channel.topic + "\r\n");
    }

    // Send user list to the joining user
    std::string names;
    for (std::map<int, bool>::iterator it = channel.members.begin(); it != channel.members.end(); ++it) {
        names += (it->second ? "@" : "") + clients[it->first].nick + " ";
    }
    sendMessage(fd, RPL_NAMREPLY_MSG(servername, client.nick, chan, names));
    sendMessage(fd, RPL_ENDOFNAMES_MSG(servername, client.nick, chan));

    return true;
}