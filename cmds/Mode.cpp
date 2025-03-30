#include "../Server.hpp"

bool IRCServer::handleMode(int fd, Client& client, const std::vector<std::string>& tokens) {
    // Check for minimum parameters (MODE <channel> <mode>)
    if (tokens.size() < 3) {
        sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "MODE"));
        return false;
    }

    // Normalize channel name
    std::string chan = tokens[1];
    if (chan[0] != '#') {
        chan = "#" + chan;
    }

    std::string mode = tokens[2];

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

    // Extract parameters if provided
    std::string params = (tokens.size() > 3) ? tokens[3] : "";
    std::string modeChange = mode;

    // Handle different modes
    if (mode == "+i") {
        channels[chan].inviteOnly = true;
        std::cout << "[DEBUG] Set inviteOnly to true for channel " << chan << std::endl;
    } else if (mode == "-i") {
        channels[chan].inviteOnly = false;
        channels[chan].invited.clear(); // Clear the invited list since the channel is no longer invite-only
        std::cout << "[DEBUG] Set inviteOnly to false for channel " << chan << " and cleared invited list" << std::endl;
    } else if (mode == "+t") {
        channels[chan].topicRestricted = true;
        std::cout << "[DEBUG] Set topicRestricted to true for channel " << chan << std::endl;
    } else if (mode == "-t") {
        channels[chan].topicRestricted = false;
        std::cout << "[DEBUG] Set topicRestricted to false for channel " << chan << std::endl;
    } else if (mode == "+k") {
        if (params.empty()) {
            sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "MODE"));
            return false;
        }
        channels[chan].key = params;
        modeChange += " " + params;
        std::cout << "[DEBUG] Set key to \"" << params << "\" for channel " << chan << std::endl;
    } else if (mode == "-k") {
        channels[chan].key = "";
        std::cout << "[DEBUG] Removed key for channel " << chan << std::endl;
    } else if (mode == "+o") {
        if (params.empty()) {
            sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "MODE"));
            return false;
        }
        bool found = false;
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.nick == params && channels[chan].members.find(it->first) != channels[chan].members.end()) {
                channels[chan].members[it->first] = true;
                modeChange += " " + params;
                found = true;
                std::cout << "[DEBUG] Granted operator status to " << params << " in channel " << chan << std::endl;
                break;
            }
        }
        if (!found) {
            sendMessage(fd, ":" + std::string(servername) + " " + ERR_NOSUCHNICK + " " + client.nick + " " + params + " :No such nick/channel\r\n");
            return false;
        }
    } else if (mode == "-o") {
        if (params.empty()) {
            sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "MODE"));
            return false;
        }
        bool found = false;
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.nick == params && channels[chan].members.find(it->first) != channels[chan].members.end()) {
                channels[chan].members[it->first] = false;
                modeChange += " " + params;
                found = true;
                std::cout << "[DEBUG] Removed operator status from " << params << " in channel " << chan << std::endl;
                break;
            }
        }
        if (!found) {
            sendMessage(fd, ":" + std::string(servername) + " " + ERR_NOSUCHNICK + " " + client.nick + " " + params + " :No such nick/channel\r\n");
            return false;
        }
    } else if (mode == "+l") {
        if (params.empty()) {
            sendMessage(fd, ERR_NEEDMOREPARAMS_MSG(servername, client.nick, "MODE"));
            return false;
        }
        // Parse the limit using std::atoi and cast to size_t
        int parsedLimit = std::atoi(params.c_str());
        if (parsedLimit <= 0) {
            sendMessage(fd, ":" + std::string(servername) + " 461 " + client.nick + " MODE :Invalid limit\r\n");
            return false;
        }
        channels[chan].userLimit = static_cast<size_t>(parsedLimit);
        modeChange += " " + params;
        std::cout << "[DEBUG] Set user limit to " << parsedLimit << " for channel " << chan << std::endl;
    } else if (mode == "-l") {
        channels[chan].userLimit = 0;
        std::cout << "[DEBUG] Removed user limit for channel " << chan << std::endl;
    } else {
        sendMessage(fd, ":" + std::string(servername) + " " + ERR_UNKNOWNCOMMAND + " " + client.nick + " MODE :Unknown mode " + mode + "\r\n");
        return false;
    }

    // Broadcast the MODE change to all channel members
    std::string mode_msg = ":" + client.nick + "!" + client.user + "@" + client.host + " MODE " + chan + " " + modeChange + "\r\n";
    broadcastToSharedChannels(fd, mode_msg); // Specify the channel to broadcast to

    return true;
}