
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>
#include "irc.hpp"
#include "Client.hpp"
#include <iostream>

class Channel
{
	public:
		const std::string& get_topic() const {
			return (this->_topic);
		}
		const std::string& get_password() const {
			return (this->_password);
		}
		const int get_num_clients() const {
			return (this->num_clients);
		}
		const int get_channel_size() const {
			return (this->channel_size);
		}
		std::vector<std::string>& getClients() {
			return (this->_clients);
		}
		void set_topic(std::string topic){
			this->_topic = topic;
		}
		void set_password(std::string password){
			this->_password = password;
		}
		void set_channel_size(int size) {
			this->channel_size = size;
		}
		// void add_client(Client client){
		// 	this->_clients.push_back(client);
		// }
		Channel(std::string topic, std::string password, int size) : num_clients(0) {
			this->_topic = topic;
			this->_password = password;
			this->channel_size = size;
		}
		std::string getName() {
			return this->_name;
		}
		Channel();
		Channel(std::string name);
		~Channel();

	private:
		std::string	_name;
		std::string	_topic;
		std::string	_password;
		std::string	_modes;
		bool		mode_i;
		bool		mode_t;
		bool		mode_k;
		bool		mode_o;
		int			num_clients;
		int			channel_size;

		std::vector<std::string> _clients;
};

#endif