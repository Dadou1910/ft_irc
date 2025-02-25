#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"

class Channel {
private:
	std::string name;
	std::string topic;
	std::vector<User*> users;
	std::vector<User*> operators;
	std::string password;
	int userLimit;

public:
	// Constructor & Destructor
	Channel(const std::string& name);
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);

	// Getters
	std::string getName() const;
	std::string getTopic() const;
	std::vector<User*> getUsers() const;
	std::vector<User*> getOperators() const;
	std::string getPassword() const;
	int getUserLimit() const;

	// Setters
	void setTopic(const std::string& topic);
	void setPassword(const std::string& password);
	void setUserLimit(int limit);

	// Methods
	void addUser(User* user);
	void removeUser(User* user);
	void addOperator(User* user);
	void removeOperator(User* user);
	bool isOperator(User* user) const;
	bool isUserInChannel(User* user) const;
	void broadcastMessage(const User& sender, const std::string& msg) const;
	void sendError(const User& user, const std::string& errorMsg) const;
};
	#endif