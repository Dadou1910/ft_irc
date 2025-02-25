#ifndef USER_HPP
#define USER_HPP

#include <iostream>      // For standard input/output operations
#include <string>        // For std::string
#include <vector>        // For std::vector
#include <map>           // For std::map (if needed for user/channel storage)
#include <algorithm>     // For std::find, std::remove (useful for managing user lists)
#include <sys/types.h>   // For system data types
#include <sys/socket.h>  // For socket programming (socket(), bind(), listen(), accept(), etc.)
#include <netinet/in.h>  // For sockaddr_in (IPv4 socket address structure)
#include <arpa/inet.h>   // For inet_ntoa(), htons(), htonl()
#include <unistd.h>      // For close(), read(), write()
#include <fcntl.h>       // For setting non-blocking sockets
#include <sys/select.h>  // For select(), FD_SET, FD_ZERO, FD_ISSET
#include <errno.h>       // For error handling (errno)
#include <cstring>       // For memset(), memcpy(), strerror()
#include <sstream>       // Required for std::stringstream
#include <stdexcept>
#include "Additional.hpp"

class User
{
private:
	std::string nickname;
	std::string username;
	std::string password;
	int socketFd;
	bool isAuthenticated;

public:
	// Constructors & Destructor
	User();
	~User();
	User(const User& src);
	User& operator=(const User& rhs);

	// Getters
	std::string getNickname() const;
	std::string getUsername() const;
	bool getAuthenticationStatus() const;
	int getSocketFd() const;

	// Setters
	void setNickname(const std::string& nick);
	void setUsername(const std::string& user);
	void setAuthenticationStatus(bool status);
	void setSocketFd(int fd);

	// Methods
	bool authenticate(const std::string& pass) const;
	void sendMessage(const std::string& msg) const;
	void handleCmd(const std::string& command);
	void sendError(const std::string& errorMsg) const;
};

bool handlePass(User& user, const std::string& pass);
bool handleNick(User& user, const std::string& nick);
bool handleUser(User& user, const std::string& username);
bool handleJoin(User& user, const std::string& channelName);
void handlePrivmsg(User& sender, const std::string& target, const std::string& msg);
bool handleKick(User& op, User& target, const std::string& channel);
bool handleInvite(User& op, User& target, const std::string& channel);
bool handleTopic(User& op, const std::string& channel, const std::string& newTopic);
bool handleMode(User& op, const std::string& channel, const std::string& modeChange);
#endif