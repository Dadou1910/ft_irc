#pragma once

// GENERAL
// Defines the name of the IRC server
#define	SERVER_NAME "localhost"
// Sets the password required to gain operator privileges
#define OPERATOR_PASSWORD "admin"
// Limits the number of simultaneous events handled by the server
#define MAX_EVENTS 10
// Specifies the server version for identification
#define VERSION "1"

/*--------------------------------------------COLORS--------------------------------------------*/

# define RESET "\e[0m"

//Regular text
# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define YEL "\e[0;33m"
# define BLU "\e[0;34m"
# define MAG "\e[0;35m"
# define CYN "\e[0;36m"
# define WHT "\e[0;37m"

//Regular bold text
# define BRED "\e[1;31m"
# define BGRN "\e[1;32m"
# define BYEL "\e[1;33m"
# define BBLU "\e[1;34m"
# define BMAG "\e[1;35m"
# define BCYN "\e[1;36m"
# define BWHT "\e[1;37m"

/*----------------------------------------------------------------------------------------------*/

typedef enum e_modes
{ 
	i = 1 << 0, // 000001
	t = 1 << 1, // 000010
	k = 1 << 2, // 000100
	o = 1 << 3, // 001000
	l = 1 << 4, // 010000
}	MODES; // i + o -> 000110
//to set MODES userMode = i | o; add one : userModes |= r; remove userModes &= ~i; to check : if (userModes & o);

// Formats a standard server reply with a given numeric reply code and nickname.
#define REPLY_FORMAT(num_rply_numb, nickname) (std::string(":") + SERVER_NAME + " " + num_rply_numb + " " + nickname + " ")

// Constructs a client identifier string including nickname, username, and server name for a given command.
#define CLIENT_ID(nickname, username, command) (":" + nickname + "!" + username + "@" + SERVER_NAME + " " + command + " ")


// 000 (SPECIAL RESPONSES)
// Sends a welcome message to the user upon successful connection to the server.
#define RPL_WELCOME(nickname) (std::string(":") + SERVER_NAME + " 001 " + nickname + " :Welcome to the " + MAG + SERVER_NAME + RESET + " Server " + nickname + "\r\n")


// 200 (COMMAND RESPONSES)
// Sends the current user mode settings to the client.
#define RPL_UMODEIS(nickname, mode) (std::string(":") + SERVER_NAME + " 221 " + nickname + " " + mode + "\r\n")


// 300 (INFORMATIONAL RESPONSES)
// Sent in response to /WHOIS, providing user details.
#define RPL_WHOISUSER(nickname, username, hostname, realname) (std::string(":") + SERVER_NAME + " 311 " + nickname + " " + username + " " + hostname + " * :" + realname + "\r\n")

// Indicates which server a user is connected to in /WHOIS.
#define RPL_WHOISSERVER(nickname, servername, serverinfo) (std::string(":") + SERVER_NAME + " 312 " + nickname + " " + servername + " :" + serverinfo + "\r\n")

// Marks the end of a WHOIS reply.
#define RPL_ENDOFWHOIS(nickname) (std::string(":") + SERVER_NAME + " 318 " + nickname + " :End of /WHOIS list" + "\r\n")

// Sent when a channel has no topic set.
#define RPL_NOTOPIC(nickname, channel) (std::string(":") + SERVER_NAME + " 331 " + nickname + " " + channel + " :No topic is set" + "\r\n")

// Sends the current topic of a channel.
#define RPL_TOPIC(nickname, channel, topic) (std::string(":") + SERVER_NAME + " 332 " + nickname + " " + channel + " " + topic + "\r\n")

// Indicates who set the topic and when.
#define RPL_TOPICWHOTIME(nickname, channel, concerned_client_nickname, time) (std::string(":") + SERVER_NAME + " 333 " + nickname + " " + channel + " " + concerned_client_nickname + " " + time + "\r\n")

// Confirms that a user has been invited to a channel.
#define RPL_INVITING(nickname, target, channel) (std::string(":") + SERVER_NAME + " 341 " + nickname + " " + target + " " + channel + "\r\n")

// Lists users in a channel as part of /NAMES.
#define RPL_NAMREPLY(nickname, channel, list_client) (std::string(":") + SERVER_NAME + " 353 " + nickname + " = " + channel + " :" + list_client  + "\r\n")

// Marks the end of a /NAMES list.
#define RPL_ENDOFNAMES(nickname, channel) (std::string(":") + SERVER_NAME + " 366 " + nickname + " " + channel + " :End of /NAMES list" + "\r\n")

// Sends server information in response to /INFO.
#define RPL_INFO(nickname) (std::string(":") + SERVER_NAME + " 371 " + nickname + " :" + SERVER_NAME + "\r\n" + \
                            ":" + SERVER_NAME + " 371 " + nickname + " :" + "written by FKY" + "\r\n" + \
                            ":" + SERVER_NAME + " 371 " + nickname + " :" + "version 1.0" + "\r\n")

// Marks the end of an /INFO response.
#define RPL_ENDOFINFO(nickname) (std::string(":") + SERVER_NAME + " 374 " + nickname + " " + ":End of /INFO list. " + "\r\n")

// Confirms that a user is now an IRC operator.
#define RPL_YOUROPER(nickname) (std::string(":") + SERVER_NAME + " 381 * " + YEL + nickname + RESET + " :is now an IRC operator" + "\r\n")


// 400 (ERROR HANDLING)
// Error 401: The specified nickname does not exist.
// This is returned when trying to message or interact with a user that isn't online.
#define ERR_NOSUCHNICK(nickname, other_nickname) (std::string(":") + SERVER_NAME + " 401 " + nickname + " " + other_nickname + RED + " :No such nick\r\n" + RESET)

// Error 402: The specified server does not exist.
#define ERR_NOSUCHSERVER(nickname) (REPLY_FORMAT(" 402", nickname) + SERVER_NAME + " :" + "\r\n")

// Error 403: The specified channel does not exist.
#define ERR_NOSUCHCHANNEL(nickname, channel) (std::string(":") + SERVER_NAME + " 403 " + nickname + " " + channel + RED + " :No such channel" + "\r\n" + RESET)

// Error 403 (Custom): No users left in the channel (custom implementation).
#define ERR_NOUSERLEFT(nickname, channel) (std::string(":") + SERVER_NAME + " 403 " + nickname + " " + channel + RED + " :No user left" + "\r\n" + RESET)

// Error 409: The command requires an origin, but none was provided.
#define ERR_NOORIGIN(nickname) (std::string(":") + SERVER_NAME + " 409 " + nickname + RED + " :No origin\r\n")

// Error 431: No nickname was provided during registration.
#define ERR_NONICKNAMEGIVEN(nickname) (std::string(":") + SERVER_NAME + " 431 *" + nickname + RED + " :No nickname given\r\n")

// Error 432: The provided nickname is invalid (contains forbidden characters).
#define ERR_ERRONEUSNICKNAME(nickname) (std::string(":") + SERVER_NAME + " 432 *" + nickname + RED +" Erroneous nickname\r\n")

// Error 433: The requested nickname is already in use.
#define ERR_NICKNAMEINUSE(nickname) ( std::string(":") + SERVER_NAME + " 433 * " + MAG + nickname + RESET + "\r\n")

// Error 441: The specified user is not in the given channel.
#define ERR_USERNOTINCHANNEL(nickname, channel, concerned_client_nickname) (std::string(":") + SERVER_NAME + " 441 " + nickname + " " + concerned_client_nickname + " " + channel + RED + " :They aren't on that channel" + "\r\n" + RESET)

// Error 442: The client attempted an action in a channel they are not a member of.
#define ERR_NOTONCHANNEL(nickname, channel) (std::string(":") + SERVER_NAME + " 442 " + nickname + " " + channel + RED + " :You're not on that channel" + "\r\n")

// Error 443: The specified user is already in the given channel.
#define ERR_USERONCHANNEL(nickname, channel, concerned_client_nickname) (std::string(":") + SERVER_NAME + " 443 " + nickname + " " + concerned_client_nickname + " #" + channel + RED + " :is already on channel" + "\r\n" + RESET)

// Error 451: The client has not registered before issuing the given command.
#define ERR_NOTREGISTERED(nickname, command) (std::string(":") + SERVER_NAME + " 451 " + nickname + " " + command + RED +" :You have not registered" + "\r\n" + RESET)

// Error 461: The command requires more parameters than were provided.
#define ERR_NEEDMOREPARAMS(nickname, command) (std::string(":") + SERVER_NAME + " 461 " + nickname + " " + command + RED + " :Not enough parameters\r\n" + RESET)

// Error 462: The client is already registered and cannot re-register.
#define ERR_ALREADYREGISTRED(nickname) (std::string(":") + SERVER_NAME + " 462 " + nickname + RED + " :Already registered\r\n" + RESET)

// Error 464: The provided password is incorrect.
#define ERR_PASSWDMISMATCH(nickname) (std::string(":") + SERVER_NAME + " 464 " + nickname + RED + " :Password incorrect" + "\r\n" + RESET)

// Error 471: The channel has reached its user limit and cannot accept new users.
#define ERR_CHANNELISFULL(nickname, channel) (REPLY_FORMAT("471", nickname) + channel + RED + " :Cannot join channel (+l)" + "\r\n" + RESET)

// Error 472: The given mode character is unknown or invalid.
#define ERR_UNKNOWNMODE(command, message) (REPLY_FORMAT("472", command) + message + RED +" :is unknown mode char to me" + "\r\n" + RESET)

// Error 473: The channel is invite-only, and the client has not been invited.
#define ERR_INVITEONLYCHAN(nickname, channel) (std::string(":") + SERVER_NAME + " 473 " + nickname + " " + channel + RED +" :Cannot join channel (+i)" + "\r\n" + RESET)

// Error 475: The channel requires a password (key), and the provided one is incorrect or missing.
#define ERR_BADCHANNELKEY(nickname, channel) (REPLY_FORMAT("475", nickname) + channel + RED + " :Cannot join channel (+k)" + "\r\n" + RESET)

// Error 481: The client attempted an action requiring operator privileges but is not an IRC operator.
#define ERR_NOPRIVILEGES(nickname) (std::string(":") + SERVER_NAME + " 481 " + nickname + RED + " :Permission Denied - You're not an IRC operator" + "\r\n" + RESET)

// Error 482: The client attempted an action requiring channel operator privileges but is not a channel operator.
#define ERR_CHANOPRIVSNEEDED(channel, nickname) (std::string(":") + SERVER_NAME + " 482 " + nickname + " " + channel + RED + " :You're not channel operator" + "\r\n" + RESET)

// Error 501: The client attempted to set an unknown or unsupported user mode.
#define ERR_UMODEUNKNOWNFLAG(nickname) (std::string(":") + SERVER_NAME + " 501 " + nickname + RED + " :Unknown MODE flag" + "\r\n" + RESET)

// Error 502: The client attempted to change another user's mode, which is not allowed.
#define ERR_USERSDONTMATCH(nickname) (std::string(":") + SERVER_NAME + " 502 " + nickname + RED +" :Can't change mode for other users\r\n" + RESET)


// COMMANDS MACRO
// Sends a PONG response when the server receives a PING request from a client.
// This is used to maintain an active connection and prevent timeouts.
#define PONG(nickname) (std::string(":") + SERVER_NAME + " PONG " + SERVER_NAME + " :" + SERVER_NAME + "\r\n")

// Notifies that a user has changed their nickname.
// Format: ":old_nick!username@host NICK :new_nick\r\n"
#define NICK(nickname, username, new_nickname) (CLIENT_ID(nickname, username, "NICK") + ":" + MAG + new_nickname + RESET + "\r\n")

// Sends a NOTICE message to a user or channel.
// NOTICEs are similar to PRIVMSG but do not trigger auto-replies.
// Format: ":nickname!username@host NOTICE dest :msg\r\n"
#define NOTICE(nickname, username, dest, msg) (CLIENT_ID(nickname, username, "NOTICE") + dest + " :" + msg + "\r\n")

// Updates the topic of a channel.
// Format: ":nickname!username@host TOPIC #channel :New Topic\r\n"
#define TOPIC(nickname, username, channel, topic) (CLIENT_ID(nickname, username, "TOPIC") + channel + " " + BLU + topic + RESET + "\r\n")

// Sends a private message to a user or channel.
// Format: ":nickname!username@host PRIVMSG dest :msg\r\n"
#define PRIVMSG(nickname, username, dest, msg) (CLIENT_ID(nickname, username, "PRIVMSG") + dest + " :" + BLU + msg + RESET + "\r\n")

// Notifies the server that a user has joined a channel.
// Format: ":nickname!username@host JOIN :#channel\r\n"
#define JOIN(nickname, username, channel) (CLIENT_ID(nickname, username, "JOIN") + ":" + GRN + channel + RESET + "\r\n")

// Notifies that a user has quit the IRC server.
// Format: ":nickname!username@host QUIT :Quit message\r\n"
#define QUIT(nickname, username, message) (CLIENT_ID(nickname, username, "QUIT") + ":" + message + "\r\n")

// Kicks a user from a channel with a reason.
// Only operators (admins) can perform this action.
// Format: ":nickname!username@host KICK #channel target :Reason\r\n"
#define KICK(nickname, username, channel, target, message) (CLIENT_ID(nickname, username, "KICK") + channel + " " + RED + target + RESET + " :" + message + "\r\n")

// Notifies that a user has left a channel voluntarily.
// Format: ":nickname!username@host PART #channel :Goodbye!\r\n"
#define PART(nickname, username, channel, message) (CLIENT_ID(nickname, username, "PART") + channel + " :" + message + "\r\n")

// Invites a user to a channel (used for invite-only channels).
// Format: ":nickname!username@host INVITE target :#channel\r\n"
#define INVITE(nickname, username, target, channel) (CLIENT_ID(nickname, username, "INVITE") + target + " :" + channel + "\r\n")

// Changes the mode of a user (e.g., +i for invisible).
// Format: ":nickname!username@host MODE user +mode\r\n"
#define MODE_USER(nickname, username, target, mode) (CLIENT_ID(nickname, username, "MODE") + target + " " + mode + "\r\n")

// Changes the mode of a channel (e.g., +m for moderated).
// Format: ":nickname!username@host MODE #channel +mode\r\n"
#define MODE_CHANNEL(nickname, username, channel, mode) (CLIENT_ID(nickname, username, "MODE") + channel + " " + mode + "\r\n")
