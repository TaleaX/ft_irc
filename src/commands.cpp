#include "Server.hpp"
#include <iomanip>

void Server::part(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		for (std::vector<Channel*>::iterator it = client.getJoinedChannels().begin(); it != client.getJoinedChannels().end(); ++it)	//iterating through Clients joined channel list
		{
			if ((*it)->getName() == reqVec[1])
			{
				for (std::vector<Client*>::iterator it2 = (*it)->getClients().begin(); it2 != (*it)->getClients().end(); ++it2)	//iterating through joined channels clients, finding "itself" and erase field in vec
				{
					if ((*it2)->getNickname() == client.getNickname())
					{
						(*it)->getClients().erase(it2);
						break ;
					}
				}
				size_t numClients = (*it)->getClients().size(); //store size to catch empty channel (for channel deletion)

				std::cout << GRAY << client.getNickname() << " left channel: " << RESET << (*it)->getName() << std::endl;
				
				it = client.getJoinedChannels().erase(it); //removing channel from clients joined channels
				if (numClients == 0) // if no user left, remove channel
				{
					std::cout << GRAY << "   + removed channel: " << RESET << reqVec[1] << std::endl;
					for (std::vector<Channel>::iterator it2 = this->_channels.begin(); it2 != this->_channels.end();)
					{
						if ((*it2).getName() == reqVec[1])
							it2 = this->_channels.erase(it2);
						else
							++it2;
							
					}
				}
				break ;
			}
		}
		std::string response = ":127.0.0.1 PART " + client.getNickname() + " " + reqVec[1] + " :left\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
	}
}

//:NickName!UserName@host JOIN #channelname
void Server::join(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		for (std::vector<Channel*>::iterator it = client.getJoinedChannels().begin(); it != client.getJoinedChannels().end(); ++it)
		{
			if ((*it)->getName() == reqVec[1])
				return ;
		}

		for (std::vector<Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
		{
			if ((*it).getName() == reqVec[1])
			{

				client.getJoinedChannels().push_back(&(*it));
				(*it).getClients().push_back(&client);
				std::cout << GRAY << client.getNickname() << " joined channel: " << RESET << it->getName() << std::endl;
				

				// std::string response = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 JOIN :" + reqVec[1] + "\r\n";
				std::string response = ":" + client.getNickname() + " JOIN :" + reqVec[1] + "\r\n";
				send(client.getSock(), response.c_str(), response.size(), 0);
				// std::string response = ":127.0.0.1 JOIN " + reqVec[1] + "\r\n";
				// std::string response = ":" + client.getNickname() + " JOIN " + reqVec[1] + "\r\n";
				// std::cout << BRED << response << RESET << std::flush;

				// for (std::vector<Client>::iterator it3 = this->_clients.begin(); it3 != this->_clients.end(); ++it3)
				// 	this->sendUserList(*it3, *it);

				// this->sendUserList(client, *it);
				return ;
			}
		}
		{
			Channel channel(reqVec[1]);
			this->_channels.push_back(channel);

			Channel& channel_ref = this->_channels.back();
			channel_ref.getClients().push_back(&client);
			client.getJoinedChannels().push_back(&channel_ref);
			std::cout << GRAY << client.getNickname() << " joined channel: " << RESET << reqVec[1] << std::endl;
					
			// std::string response = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 JOIN :" + reqVec[1] + "\r\n";
			std::string response = ":" + client.getNickname() + " JOIN :" + reqVec[1] + "\r\n";
			// std::string response = ":127.0.0.1 JOIN " + reqVec[1] + "\r\n";
			// std::string response = ":" + client.getNickname() + " JOIN " + reqVec[1] + "\r\n";
			// std::cout << BRED << response << RESET << std::flush;
			send(client.getSock(), response.c_str(), response.size(), 0);
			
			// for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
			// 	this->sendUserList(*it, channel_ref);
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
		std::string response = ":127.0.0.1 401 " + client.getNickname() + " " + reqVec[1] + " :No such nick/channel";
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

void Server::leave(std::vector<std::string> reqVec, Client& client)
{
	std::cout << "leave" << std::endl;
}

//:oldNick!~UserName@host NICK newNick
void Server::nick(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		std::string oldNick = client.getNickname();
		client.setNickname(reqVec[1]);

		std::string response = ":" + oldNick + "!~" + client.getUsername() + "@127.0.0.1 NICK " + reqVec[1] + "\r\n";
		send(client.getSock(), response.c_str(), response.size(), 0);
		// this->sendMsgToAllInChannel(client, response);
		std::cout << oldNick << GRAY << " NICK set to " << reqVec[1] << RESET << std::endl;
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

void Server::user(std::vector<std::string> reqVec, Client& client)
{
	if (reqVec.size() > 1)
	{
		std::cout << client.getNickname() << GRAY << " USER set to " << reqVec[1] << RESET << std::endl;
		client.setUsername(reqVec[1]);
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
