#ifndef IRC_HPP
#define IRC_HPP

#include <string>

// Numeric Replies
#define RPL_INVITING_MSG(servername, nick, target, channel) (":" + servername + " 341 " + nick + " " + target + " " + channel + "\r\n")
#define ERR_USERONCHANNEL_MSG(servername, nick, target, channel) (":" + servername + " 443 " + nick + " " + target + " " + channel + " :is already on channel\r\n")

#define ERR_NOTREGISTERED_MSG(servername) (":" + servername + " 451 * :You have not registered\r\n")
#define ERR_INVITEONLYCHAN_MSG(servername, nick, chan) (":" + servername + " 473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY_MSG(servername, nick, chan) (":" + servername + " 475 " + nick + " " + chan + " :Cannot join channel (+k)\r\n")
#define ERR_CHANNELISFULL_MSG(servername, nick, chan) (":" + servername + " 471 " + nick + " " + chan + " :Cannot join channel (+l)\r\n")

#define RPL_WELCOME "001"
#define RPL_WELCOME_MSG(servername, nickname) (":" + servername + RPL_WELCOME + nickname + " Welcome to the " + servername + " Server " + nickname + "! \r\n")

#define RPL_YOURHOST "002"
#define RPL_YOURHOST_MSG(servername, nick) \
    ":" + servername + " " + RPL_YOURHOST + " " + nick + " :Your host is " + servername + ", running version 1.0\r\n"

#define RPL_CREATED "003"
#define RPL_CREATED_MSG(servername, nick) \
    ":" + servername + " " + RPL_CREATED + " " + nick + " :This server was created [date]\r\n"

#define RPL_MYINFO "004"
#define RPL_MYINFO_MSG(servername, nick) \
    ":" + servername + " " + RPL_MYINFO + " " + nick + " :" + servername + " 1.0 o o\r\n"

#define RPL_MOTDSTART "375"
#define RPL_MOTDSTART_MSG(servername, nick) \
    ":" + servername + " " + RPL_MOTDSTART + " " + nick + " :- " + servername + " Message of the Day -\r\n"

#define RPL_MOTD "372"
#define RPL_MOTD_MSG(servername, nick, msg) \
    ":" + servername + " " + RPL_MOTD + " " + nick + " :- " + msg + "\r\n"

#define RPL_ENDOFMOTD "376"
#define RPL_ENDOFMOTD_MSG(servername, nick) \
    ":" + servername + " " + RPL_ENDOFMOTD + " " + nick + " :End of MOTD command\r\n"

#define RPL_TOPIC "332"
#define RPL_TOPIC_MSG(servername, nick, channel, topic) \
    ":" + servername + " " + RPL_TOPIC + " " + nick + " " + channel + " :" + topic + "\r\n"

#define RPL_NAMREPLY "353"
#define RPL_NAMREPLY_MSG(servername, nick, channel, names) \
    ":" + servername + " " + RPL_NAMREPLY + " " + nick + " = " + channel + " :" + names + "\r\n"

#define RPL_ENDOFNAMES "366"
#define RPL_ENDOFNAMES_MSG(servername, nick, channel) \
    ":" + servername + " " + RPL_ENDOFNAMES + " " + nick + " " + channel + " :End of /NAMES list\r\n"

// Error Replies
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHNICK_MSG(servername, nick, target) \
    ":" + servername + " " + ERR_NOSUCHNICK + " " + nick + " " + target + " :No such nick/channel\r\n"

#define ERR_NOTONCHANNEL "442"
#define ERR_NOTONCHANNEL_MSG(servername, nick, channel) \
    ":" + servername + " " + ERR_NOTONCHANNEL + " " + nick + " " + channel + " :You're not on that channel\r\n"

#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_CHANOPRIVSNEEDED_MSG(servername, nick, channel) \
    ":" + servername + " " + ERR_CHANOPRIVSNEEDED + " " + nick + " " + channel + " :You're not channel operator\r\n"

#define ERR_NEEDMOREPARAMS "461"
#define ERR_NEEDMOREPARAMS_MSG(servername, nick, command) \
    ":" + servername + " " + ERR_NEEDMOREPARAMS + " " + nick + " " + command + " :Not enough parameters\r\n"

#define ERR_ALREADYREGISTERED "462"
#define ERR_ALREADYREGISTERED_MSG(servername, nick) \
    ":" + servername + " " + ERR_ALREADYREGISTERED + " " + nick + " :You may not reregister\r\n"

#define ERR_NONICKNAMEGIVEN "431"
#define ERR_NONICKNAMEGIVEN_MSG(servername, nick) \
    ":" + servername + " " + ERR_NONICKNAMEGIVEN + " " + nick + " :No nickname given\r\n"

#define ERR_NICKNAMEINUSE "433"
#define ERR_NICKNAMEINUSE_MSG(servername, nick) \
    ":" + servername + " " + ERR_NICKNAMEINUSE + " " + nick + " " + nick + " :Nickname is already in use\r\n"

#define ERR_ERRONEUSNICKNAME "432"
#define ERR_ERRONEUSNICKNAME_MSG(servername, nick) \
    ":" + servername + " " + ERR_ERRONEUSNICKNAME + " " + nick + " :Erroneous nickname\r\n"

#define ERR_UNKNOWNCOMMAND "421"
#define ERR_UNKNOWNCOMMAND_MSG(servername, nick, command) \
    ":" + servername + " " + ERR_UNKNOWNCOMMAND + " " + nick + " " + command + " :Unknown command\r\n"

// Command Messages
#define NICK_MSG(oldnick, user, host, newnick) \
    ":" + oldnick + "!" + user + "@" + host + " NICK " + newnick + "\r\n"

#endif