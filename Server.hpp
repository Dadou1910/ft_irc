#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <cctype>
#include "Client.hpp"
#include <sstream>
#include <set>
#include "irc.hpp"

const int BUFFER_SIZE = 512;

struct Channel {
    std::string topic;
    std::map<int, bool> members; // fd -> isOperator
    bool inviteOnly;
    bool topicRestricted;
    std::string key;
    long unsigned int userLimit;
    std::set<std::string> invited; // Add this field to track invited users

    Channel();
};

// Helper functions (standalone)
bool isSpecialChar(char c);
bool isValidNickname(const std::string& nickname);

class IRCServer {
    private:
        std::string password;
        std::string servername;
        std::vector<struct pollfd> fds;
        std::map<int, Client> clients;
        std::map<std::string, Channel> channels;
        int server_fd;
        time_t serverCreationTime;
        std::map<int, std::string> send_queues;
    
        int findFdIndex(int fd);
        void setupServer(int port);
        std::string formatRplCreatedMsg(const std::string& servername, const std::string& nick) const;
    
    public:
        IRCServer(int port, const std::string& pass);
        ~IRCServer();
    
        void run();
        void checkRegistration(int fd);
        void handleCommand(int fd, const std::string& cmd);
        void sendMessage(int fd, const std::string& msg);
        void broadcastToSharedChannels(int fd, const std::string& msg);
        void broadcastChannel(const std::string& channel, const std::string& msg, int exclude_fd);
        bool handlePass(int fd, Client& client, const std::vector<std::string>& tokens); // Added declaration
        bool handleUser(int fd, Client& client, const std::vector<std::string>& tokens); // Added declaration
        bool handleNick(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handlePart(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handleJoin(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handleMode(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handlePing(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handlePrivmsg(int fd, Client& client, const std::vector<std::string>& tokens, const std::string& cmd);
        bool handleKick(int fd, Client& client, const std::vector<std::string>& tokens);
        bool handleTopic(int fd, Client& client, const std::vector<std::string>& tokens, const std::string& cmd);
        bool handleInvite(int fd, Client& client, const std::vector<std::string>& tokens);
        bool nicknameAlreadyUsed(const std::string& name, const Client& client);
        std::string toUpper(const std::string& str);
        const std::map<int, Client>& getClients() const;
    };

#endif