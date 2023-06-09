# include "Server.hpp"

static bool isUserMode(std::string channelName)
{
	if (channelName[0] != '#')
		return true;
	return false;
}

static bool validUserMode(char mode) {
	std::string modes = "i";

	if (modes.find(mode) == std::string::npos)
		return false;
	return true;
}

static bool validChannelMode(std::map<char, bool> modes, char mode) {
	if (modes.find(mode) == modes.end() && mode != 'o')
		return false;
	return true;
}

static bool modeAlreadyOper(std::string operation, char mode, Channel& channel)
{
	if ((operation == "-" && !channel.getModes()[mode]) || (operation == "+" && channel.getModes()[mode] && mode != 'k' && mode != 'l'))
		return true;
	return false;
}

void Server::userMode(std::vector<std::string> reqVec, Client &client)
{
	std::string modes = reqVec[2];
	std::string clientIp = client.getHostname();
	std::string response;
	std::string operation = "+";
	if (modes[0] != '-') {
		operation = "-";
	}
	for (unsigned int i = 0; i < modes.size(); ++i) {
		if (modes[i] == '-')
			operation = "-";
		else if (modes[i] == '+')
			operation = "+";
		else if (!validUserMode(modes[i])) {
			this->sendResponse(client, E_UMODEUNKNOWNFLAG(client, modes[i]));
		} else {
			if ((operation == "-" && !client.getModeI()) || (operation == "+" && client.getModeI()))
				continue;
			client.setModeI(true);
			if (operation == "-")
				client.setModeI(false);
			response = USERMODE(client, operation + modes[i]);
			this->sendResponse(client, response);
		}
	}
}

void Server::handleModeK(std::vector<std::string> reqVec, Client &client, unsigned int args_counter, std::string operation)
{
	std::string channelName = reqVec[1];
	std::vector<std::string> args;
	std::map<char, bool> channelModes = this->_channelsM[channelName].getModes();
	std::string response;
	bool argsGiven = false;
	if (reqVec.size() >= 4) {
		createLst(reqVec[3], args);
		argsGiven = true;
	}
	if (operation == "-" && channelModes['k']) {
		response = CHANNELMODEARGS(client, channelName, operation + "k", this->_channelsM[channelName].getPassword());
		this->_channelsM[channelName].setModeFalse('k');
		this->_channelsM[channelName].setPassword("");
		this->sendResponse(client, response);
	}
	else if ((!argsGiven || args_counter >= args.size())) {
		response = E_INVALIDMODEPARAM(client, channelName, "k", "You must specify a parameter for the key mode. Syntax: <key>");
		this->sendResponse(client, response);
	}
	else if (argsGiven && args_counter < args.size()) {
		if (operation == "+") {
			response = CHANNELMODEARGS(client, channelName, operation + "k", args[args_counter]);
			this->_channelsM[channelName].setModeTrue('k');
			this->_channelsM[channelName].setPassword(args[args_counter]);
		}
		this->sendResponse(client, response);
	}
}

void Server::handleModeL(std::vector<std::string> reqVec, Client &client, unsigned int args_counter, std::string operation)
{
	std::string channelName = reqVec[1];
	std::vector<std::string> args;
	std::map<char, bool> channelModes = this->_channelsM[channelName].getModes();
	std::string response;
	bool argsGiven = false;
	if (reqVec.size() >= 4) {
		createLst(reqVec[3], args);
		argsGiven = true;
	}
	if (operation == "-" && channelModes['l']) {
		response = CHANNELMODEARGS(client, channelName, operation + "l", itos(this->_channelsM[channelName].getLimit()));
		this->_channelsM[channelName].setModeFalse('l');
		this->_channelsM[channelName].setLimit(42);
		this->sendResponse(client, response);
	}
	else if ((!argsGiven || args_counter >= args.size())) {
		response = E_INVALIDMODEPARAM(client, channelName, "l", "You must specify a parameter for the limit mode. Syntax: <limit>");
		this->sendResponse(client, response);
	}
	else if (argsGiven && args_counter < args.size()) {
		if (operation == "+") {
			int limit = ft_stoi(args[args_counter]);
			response = CHANNELMODEARGS(client, channelName, operation + "l", itos(limit));
			this->_channelsM[channelName].setModeTrue('l');
			this->_channelsM[channelName].setLimit(limit);
			this->sendResponse(client, response);
		}
	}
}

void Server::handleModeO(std::vector<std::string> reqVec, Client &client, unsigned int args_counter, std::string operation)
{
	std::string channelName = reqVec[1];
	std::vector<std::string> args;
	std::map<char, bool> channelModes = this->_channelsM[channelName].getModes();
	std::map<std::string, Client*> operators = this->_channelsM[channelName].getOperators();
	std::string response;
	if (reqVec.size() >= 4)
		createLst(reqVec[3], args);

	if ((args.size() == 0 || args_counter >= args.size()))
		response = E_INVALIDMODEPARAM(client, channelName, "o", "You must specify a parameter for the operator mode. Syntax: <nick>");
	else if ((operation == "-" && operators.find(args[args_counter]) == operators.end()) || (operation == "+" && operators.find(args[args_counter]) != operators.end()))
		return;
	else {
		if (this->_channelsM[channelName].getOperators().find(args[args_counter]) != this->_channelsM[channelName].getOperators().end())
			return;
		else if (this->_clientsM.find(args[args_counter]) == this->_clientsM.end())
			response = E_NOSUCHNICK(client, reqVec[1]);
		else if (this->_channelsM[channelName].getClientsM().find(args[args_counter]) == this->_channelsM[channelName].getClientsM().end())
			response = E_NOTONCHANNEL(client, channelName);
		else {
			this->_channelsM[channelName].addOperator(args[args_counter], &client);
			if (operation == "-") {
				this->_channelsM[channelName].removeOperator(args[args_counter]);
			}
			response = CHANNELMODEARGS(client, channelName, operation + "o", args[args_counter]);
		}
	}
	this->sendResponse(client, response);
}

void Server::channelModeLoop(std::vector<std::string> reqVec, Client &client)
{
	
	std::string channelName = reqVec[1];
	std::string modes = reqVec[2];
	Channel& channel = this->_channelsM[channelName];
	std::map<char, bool> channelModes = this->_channelsM[channelName].getModes();
	std::string clientIp = client.getHostname();
	std::string response;
	std::vector<std::string> args;
	int args_counter = 0;
	std::string operation = "+";
	if (reqVec.size() >= 4) {
		createLst(reqVec[3], args);
	}
	if (modes[0] != '-') {
		operation = "-";
	}
	for (unsigned int i = 0; i < modes.size(); ++i) {
		if (modes[i] == '-') {
			operation = "-";
			continue;
		}
		else if (modes[i] == '+') {
			operation = "+";
			continue;
		}
		else if (!validChannelMode(channelModes, modes[i])) {
			std::string mode(1, modes[i]);
			response = E_UMODEUNKNOWNFLAG(client, modes[i]);
			this->sendResponse(client, response);
		} else if (channel.getOperators().find(client.getNickname()) == channel.getOperators().end()) {
		 	response = E_CHANOPRIVSNEEDED(client, channelName);
		 	this->sendResponse(client, response);
		}
		else if (modeAlreadyOper(operation, modes[i], channel)) {
			++args_counter;
			continue;
		}
		else if (modes[i] == 'k')
			this->handleModeK(reqVec, client, args_counter, operation);
		else if (modes[i] == 'o') {
			this->handleModeO(reqVec, client, args_counter, operation);
		}
		else if (modes[i] == 'l') {
			this->handleModeL(reqVec, client, args_counter, operation);
		}
		else {
			if (operation == "-")
				this->_channelsM[channelName].setModeFalse(modes[i]);
			else
				this->_channelsM[channelName].setModeTrue(modes[i]);
			response = CHANNELMODE(client, channelName, operation + modes[i]);
			this->sendResponse(client, response);
		}
		++args_counter;
	}
}

void Server::channelMode(std::vector<std::string> reqVec, Client &client)
{
	channelModeLoop(reqVec, client);
}

void Server::mode(std::vector<std::string> reqVec, Client &client)
{
	if (isUserMode(reqVec[1]) && this->checkUserMode(reqVec, client)) {
		this->userMode(reqVec, client);
	}
	else if (!isUserMode(reqVec[1]) && this->checkChannelMode(reqVec, client)) {
		channelMode(reqVec, client);
	}
}
