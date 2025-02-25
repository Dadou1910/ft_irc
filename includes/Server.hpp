#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"

class Server {
private:
	int port;
	std::string password;
	int serverSocket;
	std::vector<int> clientSockets;
	fd_set masterSet;
    fd_set readSet;
    int maxFd; 
	std::vector<User*> users;

public:
	Server(int port, const std::string& password);
	~Server();
	Server(const Server& other);
	Server& operator=(const Server& other);

	// Getters
	int getPort() const;
	std::string getPassword() const;

	// Setters
	void setPort(int newPort);
	void setPassword(const std::string& newPassword);

	// Methods
	void run();
	void acceptNewClient(fd_set& masterSet, int& maxFd);
	void handleClientCommunication(fd_set& masterSet, fd_set& readSet);
	int acceptNewConnection() const;
	void sendError(const User& user, const std::string& errorMsg) const;
	void setupServerSocket();
};

#endif