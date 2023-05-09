#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <string>
# include <iostream>
# include <netinet/in.h>
# include <arpa/inet.h>

typedef struct sockaddr_in sockaddr_in;

class Client
{
	public:
		void set_username(std::string username){
			this->_username = username;
		}
		void set_nickname(std::string nickname){
			this->_nickname = nickname;
		}
		void set_op(bool op){
			this->_op = op;
		}
		void set_ipstr(){
			this->_ipStr = inet_ntoa(this->_sin.sin_addr);
		}
		void set_id(int id){
			this->_id = id;
		}
		void set_sin(sockaddr_in& sin){
			this->_sinLen = sizeof(sin);
			this->_sin = sin;
		}
		void set_sinLen(socklen_t sinLen) {
			this->_sinLen = sinLen;
		}
		const std::string& username() const {
			return (this->_username);
		}
		const std::string& nickname() const {
			return (this->_nickname);
		}
		const bool& is_operator() const {
			return (this->_op);
		}
		const std::string& ipStr() const {
			return (this->_ipStr);
		}
		const int& id() const {
			return (this->_id);
		}
		sockaddr_in& sin() {
			return (this->_sin);
		}
		socklen_t& sinLen() {
			return (this->_sinLen);
		}
		Client(std::string nickname, std::string username, bool op) : _nickname(nickname), _username(username), _op(op) {}
		Client(sockaddr_in sin, socklen_t sinLen, int id, char* ipStr);
		Client(){}
		~Client(){}
	private:
		std::string _nickname;
		std::string _username;
		bool		_op;
		std::string _ipStr;
		int 		_id;
		sockaddr_in _sin;
		socklen_t _sinLen;
};

#endif