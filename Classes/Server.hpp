
#ifndef SERVER_HPP
# define SERVER_HPP

# define RECV_BUF 1024

# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <string>
# include <netinet/in.h>
# include "Client.hpp"
# include "irc.hpp"
# include <algorithm>
# include <vector>
# include <poll.h>
# include <stdexcept>
# include <iostream>
# include <sstream>
# include <map>
# include <cstring>
# include "Channel.hpp"

typedef struct sockaddr_in sockaddr_in;
typedef struct protoent protoent;

class Server {
	public:
		class	InvalidArgsException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		
		class	ErrorEstablishingConException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class	ErrorBindingSocketException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		
		class	ErrorOnAcceptException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		class	ErrorGettingProtonException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		class	ErrorPoll: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		void setSock(int sock) {
			this->_sock = sock;
		};
		void setPort(int port) {
			this->_port = port;
		};
		void setSin(sockaddr_in _saddr_in) {
			this->_saddr_in_len = sizeof(_saddr_in);
			this->_saddr_in = _saddr_in;
		};
		void setSinLen(socklen_t _saddr_in_len) {
			this->_saddr_in_len = _saddr_in_len;
		}
		void setCommands() ;
		void sendMsgToAll(Client &client, std::string message);
		bool checkCmd(std::vector<std::string> req);
		const int& getSock() const {
			return (this->_sock);
		}
		const int& getPort() const {
			return (this->_port);
		}
		sockaddr_in& getSin() {
			return (this->_saddr_in);
		}
		Client& getClient(int i) {
			return (this->_clients[i]);
		}
		socklen_t& getSinLen() {
			return (this->_saddr_in_len);
		}
		// Server(int port, std::vector<pollfd>& poll_fds);
		Server(int port);
		Server(int port, int sock, sockaddr_in _saddr_in) : _sock(sock), _port(port), _saddr_in(_saddr_in){};
		~Server();
		void startServer();
		void removeClientFromList(Client &client, std::string channelName);
		void accept_client();

		bool handleClientReq(Client& client);
		
		void handleReqHandshake(Client& client, std::vector<std::string> reqVec);
		void handleReqPing(Client& client, std::vector<std::string> reqVec);
		void handleReqNick(Client& client, std::vector<std::string> reqVec);
		void handleReqUser(Client& client, std::vector<std::string> reqVec);
		void handleReqMode(Client& client, std::vector<std::string> reqVec);
		void handleReqQuit(Client& client, int i);

		void	join(std::vector<std::string> reqVec, Client& client);
		void	part(std::vector<std::string> reqVec, Client& client);
		void	leave(std::vector<std::string> reqVec, Client& client);
		void	nick(std::vector<std::string> reqVec, Client& client);
		void	quit(std::vector<std::string> reqVec, Client& client);
		void	msg(std::vector<std::string> reqVec, Client& client);
		void	topic(std::vector<std::string> reqVec, Client& client);
		void	mode(std::vector<std::string> reqVec, Client& client);
		void	kick(std::vector<std::string> reqVec, Client& client);
		void	invite(std::vector<std::string> reqVec, Client& client);
		void	user(std::vector<std::string> reqVec, Client& client);
		void	ping(std::vector<std::string> reqVec, Client& client);
		void whois(std::vector<std::string> reqVec, Client &client);
		void	capreq(std::vector<std::string> reqVec, Client &client);

		// void	dbgPrintAllUsers(std::vector<std::string> reqVec, Client &client);
		// void	dbgPrintAllChannels(std::vector<std::string> reqVec, Client &client);
		void dbgPrint(std::vector<std::string> reqVec, Client &client);

		void buildReqQueue(Client& client, char buffer_arr[RECV_BUF]);
		bool parseReqQueue(Client& client);
		
		void disconnectClient(Client& client, int i);
	
		bool parseReq(Client& client, std::string command);

		void broadcastEvent(Client &client, Channel &channel);
		void sendUserList(Client& client, Channel& channel);

		bool isValidClient(std::string name);
		Client& getClientName(std::string name);

		// void sendMsgToAll(std::string message);
		void sendMsgToAllInChannel(Client &client, std::string message);

	private:
		int	_sock;
		int _port;
		protoent* _proto;
		int _client;
		sockaddr_in _saddr_in;
		socklen_t _saddr_in_len;
		
		std::vector<Client> _clients;
		std::vector<Channel> _channels;

		std::vector<pollfd> _pollFds;
		
		std::map<std::string, void(Server::*)(std::vector<std::string> reqVec, Client& client)> _commands;
};

#endif