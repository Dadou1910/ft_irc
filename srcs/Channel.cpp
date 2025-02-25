#include "../includes/Channel.hpp"
#include "../includes/User.hpp"

// Constructor
Channel::Channel(const std::string& name) : name(name), topic(""), password(""), userLimit(-1) {}

// Destructor
Channel::~Channel() {}

// Copy Constructor
Channel::Channel(const Channel& other)
    : name(other.name), topic(other.topic), users(other.users), operators(other.operators), 
      password(other.password), userLimit(other.userLimit) {}

// Assignment Operator
Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        name = other.name;
        topic = other.topic;
        users = other.users;
        operators = other.operators;
        password = other.password;
        userLimit = other.userLimit;
    }
    return *this;
}

// Getters
std::string Channel::getName() const {
    return name;
}

std::string Channel::getTopic() const {
    return topic;
}

std::vector<User*> Channel::getUsers() const {
    return users;
}

std::vector<User*> Channel::getOperators() const {
    return operators;
}

std::string Channel::getPassword() const {
    return password;
}

int Channel::getUserLimit() const {
    return userLimit;
}

// Setters
void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

void Channel::setPassword(const std::string& newPassword) {
    password = newPassword;
}

void Channel::setUserLimit(int limit) {
    userLimit = limit;
}

// Adds a user to the channel
void Channel::addUser(User* user) {
    if (userLimit > 0 && static_cast<int>(users.size()) >= userLimit) {
        user->sendMessage(ERR_CHANNELISFULL(user->getNickname(), name));
        return;
    }
    users.push_back(user);
}

// Removes a user from the channel
void Channel::removeUser(User* user) {
    std::vector<User*>::iterator it = std::find(users.begin(), users.end(), user);
    if (it != users.end()) {
        users.erase(it);
    }
}

// Adds an operator to the channel
void Channel::addOperator(User* user) {
    if (!isOperator(user)) {
        operators.push_back(user);
    }
}

// Removes an operator from the channel
void Channel::removeOperator(User* user) {
    std::vector<User*>::iterator it = std::find(operators.begin(), operators.end(), user);
    if (it != operators.end()) {
        operators.erase(it);
    }
}

// Checks if a user is an operator
bool Channel::isOperator(User* user) const {
    return std::find(operators.begin(), operators.end(), user) != operators.end();
}

// Checks if a user is in the channel
bool Channel::isUserInChannel(User* user) const {
    return std::find(users.begin(), users.end(), user) != users.end();
}

// Broadcasts a message to all users in the channel
void Channel::broadcastMessage(const User& sender, const std::string& msg) const {
    for (size_t i = 0; i < users.size(); ++i) {
        users[i]->sendMessage(PRIVMSG(sender.getNickname(), sender.getUsername(), name, msg));
    }
}

// Sends an error message to a specific user
void Channel::sendError(const User& user, const std::string& errorMsg) const {
    user.sendMessage(ERR_UNKNOWNMODE(name, errorMsg));
}
