#include "../Server.hpp"

bool IRCServer::handleTopic(int fd, Client& client, const std::vector<std::string>& tokens, const std::string& cmd) {
    // Check for minimum parameters (TOPIC <channel> [topic])
    if (tokens.size() < 2) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "TOPIC"));
        return false;
    }

    std::string chan = tokens[1];

    // Verify the channel exists and the client is a member
    if (channels.find(chan) == channels.end() || channels[chan].members.find(fd) == channels[chan].members.end()) {
        sendMessage(fd, ERR_NOTONCHANNEL_MSG(servername, client.nick, chan));
        return false;
    }

    // Check if topic is restricted and the client is not an operator
    if (channels[chan].topicRestricted && !channels[chan].members[fd]) {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED_MSG(servername, client.nick, chan));
        return false;
    }

    // If no new topic is provided, return the current topic
    if (tokens.size() == 2) {
        sendMessage(fd, RPL_TOPIC_MSG(servername, client.nick, chan, channels[chan].topic));
    }
    // Set a new topic
    else {
        // Extract the topic from the command
        std::string newTopic = cmd.substr(cmd.find(tokens[2]));
        
        // Remove leading colon if present
        if (!newTopic.empty() && newTopic[0] == ':') {
            newTopic = newTopic.substr(1);
        }

        // Trim trailing \r\n if present
        size_t pos = newTopic.find("\r\n");
        if (pos != std::string::npos) {
            newTopic = newTopic.substr(0, pos);
        }

        // Store the cleaned topic
        channels[chan].topic = newTopic;

        // Broadcast the TOPIC change with proper formatting
        broadcastToSharedChannels(fd, ":" + client.nick + "!" + client.user + "@" + client.host + " TOPIC " + chan + " :" + channels[chan].topic + "\r\n");
    }

    return true;
}