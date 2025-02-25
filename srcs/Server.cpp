// Server.cpp
#include "../includes/Server.hpp"
#include "../includes/User.hpp"

// Constructor
Server::Server(int port, const std::string& password) 
    : port(port), password(password), serverSocket(-1), maxFd(0) {
    setupServerSocket();
}

// Copy Constructor
Server::Server(const Server& other) 
    : port(other.port), password(other.password), serverSocket(-1), maxFd(other.maxFd) {
    setupServerSocket();
}

// Assignment Operator
Server& Server::operator=(const Server& other) {
    if (this != &other) { // Prevent self-assignment
        port = other.port;
        password = other.password;
        maxFd = other.maxFd;
        
        // Close existing socket before creating a new one
        if (serverSocket != -1) {
            close(serverSocket);
        }
        setupServerSocket();
    }
    return *this;
}

// Destructor
Server::~Server() {
    if (serverSocket != -1) {
        close(serverSocket);
    }

    // Free dynamically allocated users
    for (size_t i = 0; i < users.size(); ++i) {
        delete users[i];
    }
    users.clear();
}



// Handles incoming messages from existing clients
void Server::handleClientCommunication(fd_set& masterSet, fd_set& readSet) {
    for (size_t i = 0; i < users.size(); ++i) {
        int fd = users[i]->getSocketFd();
        if (FD_ISSET(fd, &readSet)) {
            char buffer[1024];
            int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

            if (bytesRead <= 0) {
                // Client disconnected
                close(fd);
                FD_CLR(fd, &masterSet);

                delete users[i];  // Free allocated memory
                users.erase(users.begin() + i);
                --i;  // Adjust index after erase
            } else {
                buffer[bytesRead] = '\0';
                users[i]->handleCmd(buffer);
            }
        }
    }
}


// Accepts a new client connection
int Server::acceptNewConnection() const {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

    if (clientSocket < 0) {
        // Notify all connected users of the issue
        for (size_t i = 0; i < users.size(); ++i) {
            users[i]->sendMessage(ERR_NOSUCHSERVER(users[i]->getNickname()));  
        }
        return -1;
    }

    return clientSocket;
}

// Accepts a new client connection and adds them to the server
void Server::acceptNewClient(fd_set& masterSet, int& maxFd) {
    int clientSocket = acceptNewConnection();
    if (clientSocket > 0) {
        FD_SET(clientSocket, &masterSet);
        maxFd = std::max(maxFd, clientSocket);

        // Create a new user and store it in the list
        User* newUser = new User();
        newUser->setSocketFd(clientSocket);
        users.push_back(newUser);

        // Send welcome message
        newUser->sendMessage(RPL_WELCOME(newUser->getNickname()));
    }
}

void Server::setupServerSocket()
{
    struct sockaddr_in serverAddr;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    // Set socket options (SO_REUSEADDR) to allow address reuse
    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        throw std::runtime_error("Failed to set option (SO_REUSEADDR) on socket");
    }

    // Set socket to non-blocking mode
    if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set option (O_NONBLOCK) on socket");
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  // Convert port to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }

    // Start listening for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1) {
        throw std::runtime_error("listen() failed");
    }

    // Initialize select() configurations
    FD_ZERO(&masterSet); // Clear master set
    FD_SET(serverSocket, &masterSet); // Add server socket to set
    maxFd = serverSocket; // Set max file descriptor

    std::cout << "Server listening on port " << port << std::endl;
}

// Runs the server, initializes socket, and starts the event loop
void Server::run() {

	fd_set readSet;

    while (true) {
        readSet = masterSet; // Copy master set

        if (select(maxFd + 1, &readSet, NULL, NULL, NULL) < 0) {
            // Notify all users of a server error
            for (size_t i = 0; i < users.size(); ++i) {
                users[i]->sendMessage(ERR_NOORIGIN(users[i]->getNickname()));  
            }
            return;
        }

        if (FD_ISSET(serverSocket, &readSet)) {
            acceptNewClient(masterSet, maxFd);
        }

        handleClientCommunication(masterSet, readSet);
    }
}



// Sends an error response using the IRC macros
void Server::sendError(const User& user, const std::string& errorMsg) const {
    user.sendMessage(ERR_NEEDMOREPARAMS(user.getNickname(), errorMsg));
}

// Getters
int Server::getPort() const {
    return port;
}

std::string Server::getPassword() const {
    return password;
}

// Setters
void Server::setPort(int newPort) {
    port = newPort;
}

void Server::setPassword(const std::string& newPassword) {
    password = newPassword;
}


