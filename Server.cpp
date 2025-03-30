#include "Server.hpp"

IRCServer::IRCServer(int port, const std::string& pass) : password(pass), servername("localhost"), server_fd(-1), serverCreationTime(time(NULL)) {
    setupServer(port);
}


IRCServer::~IRCServer() {
    if (server_fd >= 0) {
        close(server_fd);
    }
    for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
        if (it->fd >= 0) {
            close(it->fd);
        }
    }
    fds.clear();
    clients.clear();
    channels.clear();
}

const std::map<int, Client>& IRCServer::getClients() const { 
    return clients; 
}

std::string IRCServer::toUpper(const std::string& str) {
    std::string upper = str;
    for (size_t i = 0; i < upper.length(); ++i) {
        upper[i] = std::toupper(static_cast<unsigned char>(upper[i]));
    }
    return upper;
}

void IRCServer::sendMessage(int fd, const std::string& msg) {
    std::cout << "[DEBUG] Queuing message for fd=" << fd << ": " << msg << std::endl;

    // Add the message to the client's send queue
    send_queues[fd] += msg;

    // Find the pollfd for this fd and set POLLOUT
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd) {
            fds[i].events |= POLLOUT; // Add POLLOUT to the events to monitor
            break;
        }
    }
}

void IRCServer::broadcastToSharedChannels(int fd, const std::string& message) {
    // Client& sender = clients[fd];
    for (std::map<std::string, Channel>::iterator chan_it = channels.begin(); chan_it != channels.end(); ++chan_it) {
        if (chan_it->second.members.find(fd) != chan_it->second.members.end()) {
            for (std::map<int, bool>::iterator member_it = chan_it->second.members.begin(); member_it != chan_it->second.members.end(); ++member_it) {
                int member_fd = member_it->first;
                // Send to all members, including the sender
                sendMessage(member_fd, message);
            }
        }
    }
}

void IRCServer::broadcastChannel(const std::string& channel, const std::string& msg, int exclude_fd) {
    Channel& ch = channels[channel];
    for (std::map<int, bool>::iterator it = ch.members.begin(); it != ch.members.end(); ++it) {
        if (it->first != exclude_fd) {
            sendMessage(it->first, msg);
        }
    }
}

void IRCServer::setupServer(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        std::exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed\n";
        std::exit(1);
    }
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed\n";
        std::exit(1);
    }

    pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    fds.push_back(pfd);
}

void IRCServer::handleCommand(int fd, const std::string& cmd) {
    Client& client = clients[fd];

    std::cout << "[DEBUG] Received raw command from fd=" << fd << ": '" << cmd << "'" << std::endl;

    std::vector<std::string> tokens;
    std::string tmp;
    for (size_t i = 0; i < cmd.length(); ++i) {
        if (cmd[i] == ' ' || cmd[i] == '\r' || cmd[i] == '\n') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += cmd[i];
        }
    }
    if (!tmp.empty()) tokens.push_back(tmp);

    if (tokens.empty()) {
        std::cout << "[DEBUG] Parsed command: (empty)" << std::endl;
        return;
    }

    std::cout << "[DEBUG] Parsed command from fd=" << fd << ": " << tokens[0];
    for (size_t i = 1; i < tokens.size(); ++i) {
        std::cout << " " << tokens[i];
    }
    std::cout << std::endl;

    std::string command = tokens[0];
    if (command == "PASS") {
        handlePass(fd, client, tokens);
    } else if (command == "USER") {
        handleUser(fd, client, tokens);
    } else if (command == "JOIN") {
        handleJoin(fd, client, tokens);
    } else if (command == "PRIVMSG") {
        handlePrivmsg(fd, client, tokens, cmd);
    } else if (command == "KICK") {
        handleKick(fd, client, tokens);
    } else if (command == "TOPIC") {
        handleTopic(fd, client, tokens, cmd);
    } else if (command == "MODE") {
        handleMode(fd, client, tokens);
    } else if (command == "PART") {
        handlePart(fd, client, tokens);
    } else if (command == "NICK") {
        handleNick(fd, client, tokens);
    } else if (command == "INVITE") {
        handleInvite(fd, client, tokens);
    } else if (command == "PING") {
        handlePing(fd, client, tokens);
    }
}

int IRCServer::findFdIndex(int fd) {
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd) return i;
    }
    return -1;
}

std::string IRCServer::formatRplCreatedMsg(const std::string& servername, const std::string& nick) const {
    std::string timeStr = ctime(&serverCreationTime); // Format: "Wed Dec 18 12:34:56 2024\n"
    timeStr = timeStr.substr(0, timeStr.length() - 1); // Remove the trailing newline
    return ":" + servername + " 003 " + nick + " :This server was created " + timeStr + "\r\n";
}

void IRCServer::checkRegistration(int fd) {
    Client& client = clients[fd];
    // Debug output: Convert bool to int (0 or 1) for C++98 compatibility
    std::cout << "[DEBUG] Checking registration for fd=" << fd
              << ": nick=" << client.nick
              << ", user=" << client.user
              << ", hasPassword=" << (client.hasPassword ? 1 : 0)
              << ", registered=" << (client.registered ? 1 : 0) << std::endl;

    if (!client.nick.empty() && !client.user.empty() && client.hasPassword && !client.registered) {
        client.registered = true;
        sendMessage(fd, RPL_WELCOME_MSG(servername, client.nick));
        sendMessage(fd, RPL_YOURHOST_MSG(servername, client.nick));
        sendMessage(fd, formatRplCreatedMsg(servername, client.nick));
        sendMessage(fd, RPL_MYINFO_MSG(servername, client.nick));
        sendMessage(fd, RPL_MOTDSTART_MSG(servername, client.nick));
        sendMessage(fd, RPL_MOTD_MSG(servername, client.nick, "Welcome to my custom IRC server!"));
        sendMessage(fd, RPL_ENDOFMOTD_MSG(servername, client.nick));
    } else {
        std::cout << "[DEBUG] Registration not complete for fd=" << fd << std::endl;
    }
}

void IRCServer::run() {
    while (true) {
        if (poll(fds.data(), fds.size(), -1) < 0) {
            std::cerr << "Poll error\n";
            std::exit(1);
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents == 0) {
                continue; // No events on this fd
            }

            int fd = fds[i].fd;

            // Handle POLLIN (incoming data or new connection)
            if (fds[i].revents & POLLIN) {
                if (fd == server_fd) {
                    // New connection
                    sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);
                    if (client_fd < 0) continue;

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    pollfd pfd;
                    pfd.fd = client_fd;
                    pfd.events = POLLIN; // Initially monitor for POLLIN
                    pfd.revents = 0;
                    fds.push_back(pfd);
                    clients[client_fd] = Client("", "", inet_ntoa(client_addr.sin_addr), "", false, client_fd);
                    std::cout << "[DEBUG] New client connected: fd=" << client_fd << std::endl;
                } else {
                    // Incoming data from a client
                    char buffer[BUFFER_SIZE];
                    int bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);
                    if (bytes <= 0) {
                        // Client disconnected
                        std::cout << "[DEBUG] Client disconnected: fd=" << fd << std::endl;
                        close(fd);
                        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
                            it->second.members.erase(fd);
                        }
                        clients.erase(fd);
                        send_queues.erase(fd); // Clear the send queue for this client
                        fds.erase(fds.begin() + i--);
                    } else {
                        buffer[bytes] = '\0';
                        clients[fd].clientBuffer += buffer;

                        size_t pos;
                        while ((pos = clients[fd].clientBuffer.find("\r\n")) != std::string::npos) {
                            std::string cmd = clients[fd].clientBuffer.substr(0, pos);
                            clients[fd].clientBuffer.erase(0, pos + 2);
                            handleCommand(fd, cmd);
                        }
                    }
                }
            }

            // Handle POLLOUT (socket is ready to send data)
            if (fds[i].revents & POLLOUT) {
                std::map<int, std::string>::iterator queue_it = send_queues.find(fd);
                if (queue_it != send_queues.end() && !queue_it->second.empty()) {
                    // Send as much data as possible from the queue
                    const std::string& msg = queue_it->second;
                    int bytesSent = send(fd, msg.c_str(), msg.length(), 0);
                    if (bytesSent < 0) {
                        std::cerr << "[ERROR] Failed to send to fd=" << fd << ": " << strerror(errno) << std::endl;
                        // Optionally handle the error (e.g., close the connection)
                        close(fd);
                        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
                            it->second.members.erase(fd);
                        }
                        clients.erase(fd);
                        send_queues.erase(fd);
                        fds.erase(fds.begin() + i--);
                    } else {
                        // Remove the sent data from the queue
                        queue_it->second.erase(0, bytesSent);
                        std::cout << "[DEBUG] Sent " << bytesSent << " bytes to fd=" << fd << std::endl;

                        // If the queue is empty, clear POLLOUT and remove the queue entry
                        if (queue_it->second.empty()) {
                            send_queues.erase(fd);
                            fds[i].events &= ~POLLOUT; // Clear POLLOUT
                            std::cout << "[DEBUG] Send queue empty for fd=" << fd << ", cleared POLLOUT" << std::endl;
                        }
                    }
                }
            }
        }
    }
}

Channel::Channel() : inviteOnly(false), topicRestricted(false), key(""), userLimit(0) {
    invited.clear(); // Ensure the invited set is empty
}