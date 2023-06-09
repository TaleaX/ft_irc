
#ifndef SERVER_HPP
# define SERVER_HPP

# define RECV_BUF 1024
# define USR_LIMIT 1000

# include "Bot.hpp"
# include "common.hpp"
# include "definitions.hpp"
# include "Client.hpp"
# include "Channel.hpp"

typedef struct sockaddr_in sockaddr_in;
typedef struct protoent protoent;

class Server {
	public:
		Server(int port, std::string key);
		Server(int port, int sock, sockaddr_in _saddr_in) : _sock(sock), _port(port), _saddr_in(_saddr_in){};
		~Server();

		void							setSock(int sock);
		void							setPort(int port);
		void							setKey(std::string key);
		void							setSin(sockaddr_in _saddr_in);
		void							setSinLen(socklen_t _saddr_in_len);
		void							setCommands() ;
		bool							isUserInChannel(Client &client, std::string channelName);
		void							sendMsgToAll(Client &client, std::string message);
		void							sendResponse(Client &client, const std::string &response);
		bool							checkCmd(std::vector<std::string> req);

		const int& 						getSock() const;
		const int& 						getPort() const;
		const std::string& 				getHostname() const;
		const std::string& 				getKey() const;
		const bool& 					getKeySet() const;
		sockaddr_in&					getSin();
		Client&							getClient(int i);
		std::vector<Client>&			getClients();
		std::string&					getDateString();
		socklen_t&						getSinLen();
		std::map<std::string, Client*>&	getClientsM();
		std::map<std::string, Channel>&	getChannelsM();

		void							startServer();
		void							acceptClient();


		void							handleReqHandshake(Client& client, std::vector<std::string> reqVec);
		void							handleReqPing(Client& client, std::vector<std::string> reqVec);
		void							handleReqNick(Client& client, std::vector<std::string> reqVec);
		void							handleReqUser(Client& client, std::vector<std::string> reqVec);
		void							handleReqMode(Client& client, std::vector<std::string> reqVec);
		void							handleReqQuit(Client& client, int i);

		void							join(std::vector<std::string> reqVec, Client& client);
		void							part(std::vector<std::string> reqVec, Client& client);
		void							leave(std::vector<std::string> reqVec, Client& client);
		void							nick(std::vector<std::string> reqVec, Client& client);
		void							who(std::vector<std::string> reqVec, Client &client);
		void							quit(std::vector<std::string> reqVec, Client &client);
		void							msg(std::vector<std::string> reqVec, Client& client);
		void							topic(std::vector<std::string> reqVec, Client& client);
		void							mode(std::vector<std::string> reqVec, Client& client);
		void							kick(std::vector<std::string> reqVec, Client& client);
		void							notice(std::vector<std::string> reqVec, Client &client);
		void							invite(std::vector<std::string> reqVec, Client &client);
		void							user(std::vector<std::string> reqVec, Client& client);
		void							welcome(Client &client);
		void							ping(std::vector<std::string> reqVec, Client& client);
		void							whois(std::vector<std::string> reqVec, Client &client);
		void							capreq(std::vector<std::string> reqVec, Client &client);
		void							privmsg(std::vector<std::string> reqVec, Client &client);
		void							dcc(std::vector<std::string> reqVeq, Client &client);
		void							pass(std::vector<std::string> reqVec, Client &client);
		void							bot(std::vector<std::string> reqVec, Client &client);

		void 							sendMsgToAllInChannel(Channel &channel, const std::string &response, const std::string& nickname);
		void 							dbgPrint(std::vector<std::string> reqVec, Client &client);
		void 							disconnectClient(Client& client, int i);
		void 							broadcastEvent(Client &client, Channel &channel);
		void 							sendUserList(Client& client, Channel& channel);
		
		bool 							parseReqQueue(Client& client);
		bool 							buildReqQueue(Client &client, const std::string &buffer);
		bool							isValidClient(std::string name);
		bool							handleClientReq(Client &client);
		bool 							parseReq(Client& client, std::string command);
		
		bool							checkNick(std::vector<std::string> reqVec, Client& client);
		bool							checkUser(std::vector<std::string> reqVec, Client& client);
		bool							checkPart(std::vector<std::string> reqVec, Client& client);
		bool							checkInvite(std::vector<std::string> reqVec, Client& client);
		bool							checkTopic(std::vector<std::string> reqVec, Client& client);
		bool							checkJoin(std::vector<std::string> reqVec, Client& client);
		bool							checkPassword(std::string channelName, std::string password);
		bool							checkChannelMode(std::vector<std::string> reqVec, Client& client);
		bool							checkUserMode(std::vector<std::string> reqVec, Client& client);
		bool							checkKick(std::vector<std::string> reqVec, Client &client);
		void							joinAsOperator(std::string channelName, Client &client);
		void							joinAsNormal(std::string channelName, Client &client);
		void							channelMode(std::vector<std::string> reqVec, Client& client);
		void							userMode(std::vector<std::string> reqVec, Client& client);
		void							channelModeLoop(std::vector<std::string> reqVec, Client &client);
		void							handleModeK(std::vector<std::string> reqVec, Client &client, unsigned int args_counter, std::string operation);
		void							handleModeO(std::vector<std::string> reqVec, Client & client, unsigned int args_counter, std::string operation);
		void							sendNewNickReq(std::vector<std::string> reqVec, Client& client);
		void							partAllChannels(Client& client);
		void							handleModeL(std::vector<std::string> reqVec, Client &client, unsigned int args_counter, std::string operation);

		Client&							getClientName(std::string name);

	private:
		int								_sock;
		int								_port;
		std::string						_key;
		bool							_key_set;
		size_t 							_num_channels;
		size_t 							_num_ops;
		size_t 							_num_users;
		protoent* 						_proto;
		int 							_client;
		sockaddr_in						_saddr_in;
		socklen_t						_saddr_in_len;
		std::string 					_hostname;
		std::string 					_dateString;
		Bot 							*_bot;
		bool							_bot_on;
		
		std::vector<pollfd>				_pollFds;
		std::vector<Client>				_clients;
		std::map<std::string, Client*>	_clientsM;
		std::map<std::string, Channel>	_channelsM;
		std::map<std::string, void(Server::*)(std::vector<std::string> reqVec, Client& client)> _commands;

};

bool	channelPrivNeed(Channel channel, std::string nickname, char mode);
int		ft_stoi(std::string str);

#endif