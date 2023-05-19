#include "Server.hpp"
#include <iomanip>

//:NICK!USER@HOST PART #channelname :optional_part_message
void Server::part(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		for (std::map<std::string, Channel*>::iterator itMap = client.getJoinedChannelMap().begin(); itMap != client.getJoinedChannelMap().end(); ++itMap) {
			if (itMap->first == reqVec[1]) {
				std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@127.0.0.1 PART " + reqVec[1] + "\r\n";
				send(client.getSock(), response.c_str(), response.size(), 0);
				this->sendMsgToAll(client, response);

				size_t numClients = client.getJoinedChannelMap()[reqVec[1]]->getClients().size();

				client.getJoinedChannelMap().erase(reqVec[1]);
				// client.getChannelNames().erase(name);
				
				std::cout << GRAY << client.getNickname() << " left channel: " << RESET << itMap->first << std::endl;
				
				if (numClients == 1) // if no user left, remove channel
				{
					for (std::vector<Channel>::iterator it2 = this->_channels.begin(); it2 != this->_channels.end();)
					{
						if ((*it2).getName() == reqVec[1])
							it2 = this->_channels.erase(it2);
						else
							++it2;	
					}
						std::cout << GRAY << "removed channel: " << RESET << reqVec[1] << std::endl;
				}
				break ;
			}
		}
		// for (std::vector<std::string>::iterator name = client.getChannelNames().begin(); name != client.getChannelNames().end(); ++name) {
		// 	if (*name == reqVec[1]) {
		// 		std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@127.0.0.1 PART " + reqVec[1] + "\r\n";
		// 		send(client.getSock(), response.c_str(), response.size(), 0);
		// 		this->sendMsgToAll(client, response);

		// 		size_t numClients = client.getJoinedChannelMap()[reqVec[1]]->getClientMap().size();

		// 		client.getJoinedChannelMap().erase(reqVec[1]);
		// 		client.getChannelNames().erase(name);
				
		// 		std::cout << GRAY << client.getNickname() << " left channel: " << RESET << *name << std::endl;
				
		// 		if (numClients == 1) // if no user left, remove channel
		// 		{
		// 			for (std::vector<Channel>::iterator it2 = this->_channels.begin(); it2 != this->_channels.end();)
		// 			{
		// 				if ((*it2).getName() == reqVec[1])
		// 					it2 = this->_channels.erase(it2);
		// 				else
		// 					++it2;	
		// 			}
		// 				std::cout << GRAY << "removed channel: " << RESET << reqVec[1] << std::endl;
		// 		}
		// 		break ;
		// 	}
		// }
		// std::string response = ":127.0.0.1 PART " + client.getNickname() + " " + reqVec[1] + " :left\r\n";
	}
}

//:NickName!UserName@host JOIN #channelname
void Server::join(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		// for (std::vector<Channel*>::iterator it = client.getJoinedChannels().begin(); it != client.getJoinedChannels().end(); ++it)
		// {
		// 	if ((*it)->getName() == reqVec[1])
		// 		return ;
		// }

		for (std::vector<Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
		{
			if ((*it).getName() == reqVec[1])
			{
				// client.getChannelNames().push_back(reqVec[1]);
				// client.getJoinedChannels().push_back(&(*it));
				client.getJoinedChannelMap()[reqVec[1]] = &(*it);
				(*it).getClients().push_back(&client);
				std::cout << GRAY << client.getNickname() << " joined channel: " << RESET << it->getName() << std::endl;
				
				std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost JOIN " + reqVec[1] + "\r\n";
				send(client.getSock(), response.c_str(), response.size(), 0);

				this->sendMsgToAll(client, response);
				for (std::vector<Client>::iterator it3 = this->_clients.begin(); it3 != this->_clients.end(); ++it3)
					this->sendUserList(*it3, *it);

				return ;
			}
		}
		{
			Channel channel(reqVec[1]);
			this->_channels.push_back(channel);

			Channel& channel_ref = this->_channels.back();
			channel_ref.getClients().push_back(&client);

			// client.getJoinedChannels().push_back(&channel_ref);
			// client.getChannelNames().push_back(reqVec[1]);
			client.getJoinedChannelMap()[reqVec[1]] = &channel_ref;
			std::cout << GRAY << client.getNickname() << " joined channel: " << RESET << reqVec[1] << std::endl;
					
			std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost JOIN " + reqVec[1] + "\r\n";
			send(client.getSock(), response.c_str(), response.size(), 0);
			this->sendMsgToAll(client, response);
			for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
				this->sendUserList(*it, channel_ref);
		}
	}
}

// 311 -> found
// 401 -> not found
void Server::whois(std::vector<std::string> reqVec, Client& client)
{
	// std::cout << GRAY;
	// for (std::vector<std::string>::iterator it = reqVec.begin(); it != reqVec.end(); ++it)
	// 	std::cout << *it + " ";
	// std::cout << RESET << std::endl;
	if (reqVec.size() > 1 && this->isValidClient(reqVec[1]))
	{
		Client target = this->getClientName(reqVec[1]);
		std::string response = ":127.0.0.1 311 " + client.getNickname() + " " + reqVec[1] + " " + target.getNickname() + " " + target.getUsername() + " * :" + target.getRealName() + "\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
		return ;
	}
	else if (reqVec.size() > 1)
	{
		std::string response = ":127.0.0.1 401 " + client.getNickname() + " " + reqVec[1] + " :No such nick/channel\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
	}
	else
	{
		std::string response = ":127.0.0.1 311 " + client.getNickname() + " " + reqVec[1] + client.getNickname() + " " + client.getUsername() + " * :" + client.getRealName() + "\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
	}
	// std::cout << GRAY << "WHOIS error" << RESET << std::endl;
	// this->dbgPrintAllUsers(reqVec, client);
}

void Server::capreq(std::vector<std::string> reqVec, Client &client)
{
	// if (reqVec.size() > 1)
	// {
		std::string response = ":127.0.0.1 CAP * LS :multi-prefix away-notify account-notify\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
	// }
	std::cout << GRAY;
	for (std::vector<std::string>::iterator it = reqVec.begin(); it != reqVec.end(); ++it)
		std::cout << *it + " ";
	std::cout << RESET << std::endl;
}

//:NICK!USER@host PRIVMSG <recipient> :<text>
//PRIVMSG <recipient>{,<recipient>} <text to be sent>
void Server::privmsg(std::vector<std::string> reqVec, Client &client)
{
	// for (std::vector<std::string>::iterator it = reqVec.begin(); it != reqVec.end(); ++it)
	// 	std::cout << ">" << *it << std::endl;
	// if (reqVec.size() > 2)
	// {
	// 	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	// 	{
	// 		if ()
	// 	}
	// }
}

void Server::leave(std::vector<std::string> reqVec, Client& client)
{
	std::cout << "leave" << std::endl;
}

//:oldNick!~UserName@host NICK newNick
void Server::nick(std::vector<std::string> reqVec, Client& client)
{
	if (checkNick(reqVec, client))
	{
		std::string oldNick = client.getNickname();
		client.setNickname(reqVec[1]);

		std::string response = ":" + oldNick + "!" + client.getUsername() + "@127.0.0.1 NICK " + reqVec[1] + "\r\n";

		std::cout << BRED << response << RESET << std::flush;

		// usleep(50000);
		//:oldName NICK newName
		send(client.getSock(), response.c_str(), response.size(), 0);
		// response = ":" + oldNick + "NICK" + reqVec[1] + "\r\n";
		// this->sendMsgToAllInChannel(client, response);
		// this->sendMsgToAll(client, response);
		std::cout << oldNick << GRAY << " NICK set to " << reqVec[1] << RESET << std::endl;
		if (!client.getRegistered())
		{
			std::string response = ":127.0.0.1 001 " + client.getNickname() + " :welcome, " + client.getNickname() + "!" + client.getUsername() + "@" + "127.0.0.1\r\n";
			send(client.getSock(), response.c_str(), response.size(), 0);
			client.setRegistered(true);
		}
	}
}



void Server::msg(std::vector<std::string> reqVec, Client& client)
{
	if (VERBOSE)
		std::cout << client.getNickname() << GRAY << " msg" << std::endl;
}

void Server::topic(std::vector<std::string> reqVec, Client& client)
{
	if (VERBOSE)
		std::cout << client.getNickname() << GRAY << " topic" << std::endl;
}

void Server::mode(std::vector<std::string> reqVec, Client& client)
{
	if (VERBOSE)
		std::cout << client.getNickname() << GRAY << " mode" << std::endl;
}

void Server::kick(std::vector<std::string> reqVec, Client& client)
{
	if (VERBOSE)
		std::cout << client.getNickname() << GRAY << " kick" << std::endl;
}

void Server::invite(std::vector<std::string> reqVec, Client& client)
{
	if (VERBOSE)
		std::cout << client.getNickname() << GRAY << " invite" << std::endl;
}

//USER max 0 127.0.0.1 :1,11KVIrc 5.0.0 Aria http://kvirc.net/
/*
recVec[0] == USER
recVec[1] == max
recVec[2] == 0
recVec[3] == 127.0.0.1
recVec[4] == :1,11KVIrc
recVec[5] == 5.0.0
recVec[6] == Aria
recVec[7] == http://kvirc.net/
*/
void Server::user(std::vector<std::string> reqVec, Client& client)
{
	if (checkUser(reqVec, client))
	{
		std::cout << client.getNickname() << GRAY << " USER set to " << reqVec[1] << RESET << std::endl;
		client.setUsername(reqVec[1]);
		client.setRealname(reqVec[4]);
	}
}

void Server::ping(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		std::string response = "PONG " + reqVec[1] + "\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
		std::cout << client.getNickname() << GRAY << " PING recieved (" << reqVec[1] << ")" << RESET << std::endl;
	}
}

void Server::dbgPrint(std::vector<std::string> reqVec, Client& client)
{
	std::vector<Client>::iterator clientIt = this->_clients.begin();
	std::vector<Channel>::iterator channelIt = this->_channels.begin();

	int numClients = 0;
	int numChannels = 0;

	std::cout << BBLUE
	<< "┌────────────┐   ┌────────────┐" << std::endl
	<< "│   USERS    │   │  CHANNELS  │" << std::endl
	<< "├────────────┤   ├────────────┤" << std::endl;

	while (clientIt != this->_clients.end() || channelIt != this->_channels.end())
	{
		std::cout << "│ " << RESET;
		if (clientIt != this->_clients.end())
		{
			std::cout << std::setw(11) << std::left << clientIt->getNickname();
			++clientIt;
			++numClients;
		}
		else
			std::cout << std::setw(11) << "";
		
		std::cout << BBLUE << "│   │ " << RESET;

		if (channelIt != this->_channels.end())
		{
			std::cout << std::setw(11) << std::left << channelIt->getName();
			++channelIt;
			++numChannels;
		}
		else
			std::cout << std::setw(11) << "";

		std::cout << BBLUE << "│" << std::endl;
	}

	std::cout << BBLUE
	<< "└────────────┘   └────────────┘" << RESET << std::endl;
	std::cout << " " << numClients << " user(s)" << "        " << numChannels << " channel(s)" << std::endl;
}
