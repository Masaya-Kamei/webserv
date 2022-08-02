#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ListenSocket.hpp"

class ServerSocket : public ASocket
{
	public:
		explicit ServerSocket(const ListenSocket& lsocket);
		~ServerSocket();

		std::string	RecvData(const ssize_t size = 1048576) const;
		void		SendData(const std::string& msg) const;
};

#endif  // SERVERSOCKET_HPP