
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

typedef struct sockaddr_in sockaddr_in;

class Channel;

class Client
{
	public:
		void setUsername(std::string username){
			this->_userName = username;
		}
		void setNickname(std::string nickname){
			this->_nickName = nickname;
		}
		void setRealname(std::string realname){
			this->_realName = realname;
		}
		void setRegistered(bool registered){
			this->_registered = registered;
		}
		void setOp(bool op){
			this->_op = op;
		}
		void setIpstr(){
			this->_ipStr = inet_ntoa(this->_saddr_in.sin_addr);
		}
		void setSock(int sock){
			this->_sock = sock;
		}
		void setSin(sockaddr_in& sin){
			this->_saddr_in_len = sizeof(sin);
			this->_saddr_in = sin;
		}
		void setSinLen(socklen_t sinLen) {
			this->_saddr_in_len = sinLen;
		}
		const std::string& getUsername() const {
			return (this->_userName);
		}
		const std::string& getNickname() const {
			return (this->_nickName);
		}
		const bool& getOp() const {
			return (this->_op);
		}
		const std::string& getIpStr() const {
			return (this->_ipStr);
		}
		const int& getSock() const {
			return (this->_sock);
		}
		sockaddr_in& getSin() {
			return (this->_saddr_in);
		}
		socklen_t& getSinLen() {
			return (this->_saddr_in_len);
		}
		std::vector<std::string>& getReqQueue() {
			return this->_reqQueue;
		}
		void setReqQueue(std::vector<std::string> reqQueue) {
			this->_reqQueue = reqQueue;
		}
		std::string getUsername() {
			return this->_userName;
		}
		std::string getNickname() {
			return this->_nickName;
		}
		bool getRegistered(){
			return this->_registered;
		}
		std::string getRealName();
		pollfd& getPollFd() {
			return this->_pollFd;
		}

		std::vector<std::string>& getJoinedChannels() {
			return this->_joinedChannels;
		}


		// Client(sockaddr_in sin, socklen_t sinLen, int id, char* ipStr, pollfd& pollFd);
		Client(sockaddr_in sin, int sock, std::string ipStr, pollfd pollFd);

		
	private:
		std::string _nickName;
		std::string _userName;
		std::string _realName;
		bool		_op;
		std::string _ipStr;
		int 		_sock;
		sockaddr_in _saddr_in;
		socklen_t _saddr_in_len;
		
		pollfd _pollFd;

		bool _registered;

		std::vector<std::string> _reqQueue;
		std::vector<std::string> _joinedChannels;
};

#endif