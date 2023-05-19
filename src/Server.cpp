# include "Server.hpp"
# include "Client.hpp"
# include "irc.hpp"
# include <fcntl.h>

extern bool g_run;

Server::Server(int port)
{

	this->_channels.reserve(1024);
	this->_clients.reserve(1024);
	this->_pollFds.reserve(1024);


	this->_proto = getprotobyname("tcp");
	this->_sock = socket(AF_INET, SOCK_STREAM, this->_proto->p_proto);
	this->_port = port;
	this->_saddr_in.sin_family = AF_INET;
	this->_saddr_in.sin_port = htons(this->_port);
	this->_saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_saddr_in_len = sizeof(this->_saddr_in);

	int optval = 1;
    setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	fcntl(this->_sock, F_SETFL, O_NONBLOCK);
	bind(this->_sock, (struct sockaddr*)&this->_saddr_in, this->_saddr_in_len);
	listen(this->_sock, 5);

	pollfd server_poll_fd;
    server_poll_fd.fd = this->_sock;
    server_poll_fd.events = POLLIN;
    this->_pollFds.push_back(server_poll_fd);
	this->setCommands();
	std::cout << "Server listening on: " << BWHITE << inet_ntoa(this->_saddr_in.sin_addr) << ":" << this->_port << RESET <<  std::endl;
}

Server::~Server()
{
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		close((*it).getSock());
	close(this->_sock);
}

void Server::setCommands()
{
	this->_commands["JOIN"] = &Server::join;
	this->_commands["PART"] = &Server::part;
	this->_commands["LEAVE"] = &Server::leave;
	this->_commands["MSG"] = &Server::msg;
	this->_commands["NICK"] = &Server::nick;
	this->_commands["TOPIC"] = &Server::topic;
	this->_commands["MODE"] = &Server::mode;
	this->_commands["KICK"] = &Server::kick;
	this->_commands["INVITE"] = &Server::invite;
	this->_commands["USER"] = &Server::user;
	this->_commands["PING"] = &Server::ping;
	this->_commands["WHOIS"] = &Server::whois;
	this->_commands["CAP"] = &Server::capreq;

	this->_commands["dbg"] = &Server::dbgPrint;
	// this->_commands["pac"] = &Server::dbgPrintAllChannels;
}

void Server::sendMsgToAll(Client &client, std::string message)
{
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->getNickname() != client.getNickname())
			send(it->getSock(), message.c_str(), message.size(), 0);
	}
}

void Server::sendMsgToAllInChannel(Client& client, std::string message)
{

	// for (std::vector<std::string>::iterator itChannel = client.getJoinedChannels().begin(); itChannel != client.getJoinedChannels().end(); ++itChannel)
	// {
	// 	for (std::vector<std::strings>::iterator itNickname = getChannelByName())
	// }
	// {
	// 	for (std::vector<Client>::iterator itClient = itChannel->getClients().begin(); itClient != itChannel->getClients().end(); ++itClient)
	// 	{
	// 		// if ((*itClient)->getNickname() != client.getNickname())
	// 			send(itClient->getSock(), message.c_str(), message.size(), 0);
	// 	}
	// }
}

bool Server::parseReq(Client& client, std::string request)
{
	std::vector<std::string> reqVec;
	std::string reqField;

	std::istringstream iss(request);
	while (iss >> reqField)
		reqVec.push_back(reqField);

	if (reqVec.size())
	{
		std::map<std::string, void(Server::*)(std::vector<std::string> reqVec, Client& client)>::iterator it = this->_commands.find(reqVec[0]);
		
		if (request.find("CAP LS") != std::string::npos)
			this->handleReqHandshake(client, reqVec);
		
		else if (it != this->_commands.end())
			(this->*(it->second))(reqVec, client);

		else if (reqVec[0] == "QUIT")
			return false;
		else
			std::cout << GRAY << "not recognized: " RESET << request << std::endl;
	}
	return true;
}

void Server::broadcastEvent(Client& client, Channel& channel)
{
	// std::string response = :NickName!~UserName@host JOIN #channelname
}

//:<server> 353 <nick> = <channel> :<space-separated list of nicks>
void Server::sendUserList(Client& client, Channel& channel)
{
	std::string response = ":127.0.0.1 353 " + client.getNickname() + " = " + channel.getName() + " :";
    for (std::vector<std::string>::iterator it = channel.getClients().begin(); it != channel.getClients().end(); ++it)
	{
 		response += *it;
		if (it + 1 != channel.getClients().end())
			response += " ";
	}
	response += "\r\n";
	send(client.getSock(), response.c_str(), response.size(), 0);
	
	response = ":127.0.0.1 366 " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list\r\n";
	send(client.getSock(), response.c_str(), response.size(), 0);

	// std::cout << BRED << response << RESET << std::endl;
}

bool Server::isValidClient(std::string name)
{
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->getNickname() == name)
			return true;
	}
	return false;
}

// ! use isValidClient() as protection !
Client &Server::getClientName(std::string name)
{
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->getNickname() == name)
			return (*it);
	}
	Client *err;
	return *(err);
}

bool Server::parseReqQueue(Client& client)
{
	for (std::vector<std::string>::iterator it = client.getReqQueue().begin(); it != client.getReqQueue().end(); ++it)
	{
		if (!this->parseReq(client, *it))
			return false;
	}
	client.getReqQueue().clear();
	return true;
}

void Server::buildReqQueue(Client& client, char buffer_arr[RECV_BUF])
{
	std::istringstream iss(buffer_arr);
	std::string buffer_str;

	while (std::getline(iss, buffer_str, '\n'))
		client.getReqQueue().push_back(buffer_str);
}

bool Server::handleClientReq(Client& client)
{
	char buffer_arr[RECV_BUF];
	memset(buffer_arr, 0, RECV_BUF);

	int recv_len = recv(client.getPollFd().fd, &buffer_arr, RECV_BUF, 0);

	// std::cout << BLUE << " < " << buffer_arr << RESET << std::endl;

	if (recv_len <= 0)
		return false;

	else
	{		
		this->buildReqQueue(client, buffer_arr);
		if (!this->parseReqQueue(client))
			return false;
		memset(buffer_arr, 0, RECV_BUF);
	}
	return true;
}

void Server::accept_client()
{
	sockaddr_in sin;
	socklen_t size = sizeof(sin);
	int sock = accept(this->_sock, (struct sockaddr*)&sin, &size);
	if (sock > 0)
	{
		getsockname(sock, (struct sockaddr*)&sin, &size);
		std::string ipStr = inet_ntoa(sin.sin_addr);

		pollfd client_poll_fd;
		client_poll_fd.fd = sock;
		client_poll_fd.events = POLLIN;
		this->_pollFds.push_back(client_poll_fd);

		Client client(sin, sock, ipStr, client_poll_fd);
		this->_clients.push_back(client);
	}
}

void Server::disconnectClient(Client& client, int i)
{
	std::cout << RED << "Client " << BRED << this->_clients[i - 1].getIpStr() << RED << " disconnected." << RESET << std::endl;
	close(this->_pollFds[i].fd);
	this->_pollFds.erase(this->_pollFds.begin() + i);
	this->_clients.erase(this->_clients.begin() + (i - 1));
}

void Server::startServer()
{
	int res = 0;

	while (g_run)
	{
		res = poll(this->_pollFds.data(), this->_pollFds.size(), 5000);
		for (int i = 0; res > 0 && i < this->_pollFds.size(); i++)
		{
			if (this->_pollFds[i].fd == this->_sock && this->_pollFds[i].revents & POLLIN)
				this->accept_client();

			else if (i > 0 && this->_pollFds[i].revents & POLLIN)
			{
				if (!this->handleClientReq(this->_clients[i - 1]))
					this->disconnectClient(this->_clients[i - 1], i);
			}
		}
	}
}

void		Server::removeClientFromList(Client& client, std::string channelName)
{
	
}

const char* Server::InvalidArgsException::what() const throw()
{
	return "Invalid Arguments";
}

const char* Server::ErrorEstablishingConException::what() const throw()
{
	return "Error establishing connection...";
}

const char* Server::ErrorBindingSocketException::what() const throw()
{
	return "Error binding socket...";
}

const char* Server::ErrorOnAcceptException::what() const throw()
{
	return "Error on accepting...";
}

const char* Server::ErrorGettingProtonException::what() const throw()
{
	return "Error getting proton...";
}

const char* Server::ErrorPoll::what() const throw()
{
	return "Error: poll returned -1";
}
