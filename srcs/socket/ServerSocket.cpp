#include <unistd.h>
#include <sys/socket.h>
#include "ServerSocket.hpp"

ServerSocket::ServerSocket(const int fd) :
	ASocket(fd), socket_status_(CONNECT)
{
}

ServerSocket::~ServerSocket()
{
}

std::string ServerSocket::RecvRequest() const
{
	const ssize_t	kSize = 1048576;     // 1MiB バイト
	char 			request_msg[kSize + 1];

	ssize_t recv_size = recv(fd_, request_msg, kSize, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	request_msg[recv_size] = '\0';

	return (std::string(request_msg));
}

std::string ServerSocket::RecvData() const
{
	char 		buf[BUF_SIZE + 1];
	ssize_t		recv_size;

	recv_size = recv(fd_, buf, BUF_SIZE, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	buf[recv_size] = '\0';

	return (std::string(buf));
}

void	ServerSocket::SendResponse(const std::string& response_msg) const
{
	ssize_t send_size = send(fd_, response_msg.c_str(), response_msg.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}

void	ServerSocket::DisconnectSocket(void)
{
	socket_status_ = ServerSocket::DISCONNECT;
}

ServerSocket::e_status	ServerSocket::GetSocketStatus(void) const
{
	return (socket_status_);
}
