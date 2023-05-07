#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <netinet/in.h>
 #include "Client.hpp"
 #include <vector>

typedef struct sockaddr_in sockaddr_in;
typedef struct protoent protoent;

struct x {
	int f;
};

struct lol {
	int i;
	struct x test_struct;
};

class Server {
	public:
		void accept_client();
		void set_sock(int sock) {
			this->_sock = sock;
		};
		void set_port(int port) {
			this->_port = port;
		};
		void set_sin(struct sockaddr_in sin) {
			this->_sinLen = sizeof(sin);
			this->_sin = sin;
		};
		void set_sinLen(socklen_t sinLen) {
			this->_sinLen = sinLen;
		}
		const int& sock() const {
			return (this->_sock);
		}
		const int& port() const {
			return (this->_port);
		}
		struct sockaddr_in& sin() {
			return (this->_sin);
		}
		Client& client() {
			return (this->_clients.front());
		}
		socklen_t& sinLen() {
			return (this->_sinLen);
		}
		Server(){};
		Server(int port, int sock, sockaddr_in sin) : _sock(sock), _port(port), _sin(sin){};
		~Server(){};
	private:
		int	_sock;
		int _port;
		int _client;
		struct sockaddr_in _sin;
		socklen_t _sinLen;
		std::vector<Client> _clients;
};

#endif