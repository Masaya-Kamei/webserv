#include <string>
#include "HTTPServer.hpp"
#include "ListenSocket.hpp"
#include "debug.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPError.hpp"

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::Start() const
{
	ListenSocket	*lsocket = new ListenSocket();
	EventQueue		equeue;

	lsocket->ListenConnection();
	equeue.RegisterEvent(lsocket->GetFd(), lsocket);
	MainLoop(equeue);
	delete lsocket;
}

void	HTTPServer::MainLoop(EventQueue const & equeue) const
{
	void			*udata;
	ASocket			*asocket;
	ListenSocket	*lsocket;
	ServerSocket	*ssocket;
	ServerSocket 	*new_ssocket;

	while (1)
	{
		udata = equeue.WaitEvent();
		asocket = static_cast<ASocket*>(udata);
		lsocket = dynamic_cast<ListenSocket*>(asocket);
		ssocket = dynamic_cast<ServerSocket*>(asocket);
		if (lsocket)
		{
			std::cout << "Accept!!" << std::endl;
			new_ssocket = new ServerSocket(lsocket->AcceptConnection());
			equeue.RegisterEvent(new_ssocket->GetFd(), new_ssocket);
		}
		else
			Communication(ssocket);
	}
}

void    HTTPServer::Communication(ServerSocket *ssocket) const
{
	std::string  recv_msg;
	// int            status_code;
	// HTTPRequest    req;
	HTTPMethod      method;

	try
	{
	// 	recv_msg = req.RecvRequest(ssocket);
	// 	if (recv_msg.size() == 0)
	// 	{
	// 	delete ssocket;
	// 	return;
	// 	}
		// status_code = method.ExecHTTPMethod(req);
		method.ExecHTTPMethod();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		// 	if (HTTPError ではない場合)
		// 	throw;
		// 	status_code = ?;
	}
	HTTPResponse res(method);
	res.SendResponse(ssocket);
}
