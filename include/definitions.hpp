#ifndef DEFINITIONS_HPP
# define DEFINITIONS_HPP

# include "common.hpp"

# define RPL_WELCOME 001
# define RPL_YOURHOST 002
# define RPL_CREATED 003
# define RPL_MYINFO 004
# define RPL_ISUPPORT 005
# define RPL_TOPIC 332
# define RPL_NAMREPLY 353
# define RPL_ENDOFNAMES 366


# define RPL_UMODEIS std::string("221")
# define RPL_CHANNELMODEIS std::string("324")
# define RPL_YOUREOPER std::string("381")

# define ERR_NOSUCHNICK std::string("401")
# define ERR_NOSUCHCHANNEL std::string("403")
# define ERR_NONICK std::string("431")
# define ERR_INVALIDNICK std::string("432")
# define ERR_NICKINUSE std::string("433")
# define ERR_USERNOTINCHANNEL std::string("441")
# define ERR_NOTONCHANNEL std::string("442") 
# define ERR_NEEDMOREPARAMS std::string("461")
# define ERR_ALREADYREGISTERED std::string("462")
# define ERR_INVITEONLYCHAN std::string("473")
# define ERR_BADCHANNELKEY std::string("475")
# define ERR_CHANOPRIVSNEEDED std::string("482")
# define ERR_USERONCHANNEL std::string("443") 
# define ERR_CHANOPRIVSNEEDED std::string("482")
# define ERR_UMODEUNKNOWNFLAG std::string("501")
# define ERR_USERSDONTMATCH std::string("502")

# define SERV_ADDR "0.0.0.0"
# define ERR_INVALIDMODEPARAM std::string("696")
// <-------- RESPONSE CODES ---------->
// #define ERR_ ":127.0.0.1 4 "
// START


// PASS 
# define E_NEEDMOREPARAMS ":" SERV_ADDR " 461 "
# define E_ALREADYREGISTERED ":" SERV_ADDR " 462 "
# define E_PASSWDMISMATCH ":" SERV_ADDR " 464 "

// NICK message
# define E_NONICKNAMEGIVEN ":" SERV_ADDR " 431 "
# define E_NICKNAMEINUSE ":" SERV_ADDR " 433 "

#endif
