#include <string>
#include "HTTPServer.hpp"
#include "ListenSocket.hpp"
#include "debug.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPError.hpp"
#include "ClientClosed.hpp"
#include "HTTPRequest.hpp"

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

void	HTTPServer::Communication(ServerSocket *ssocket) const
{
	HTTPRequest		req;
	int				status_code;
	HTTPMethod		method;

	try
	{
		// req.RecvRequest(*ssocket);
		// req.RequestDisplay();
		status_code = method.ExecHTTPMethod(req);
	}
	catch (const ClientClosed& e)
	{
		delete ssocket;
		return;
	}
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
	}
	HTTPResponse	res(method, status_code, req);
	res.SendResponse(ssocket);
}

/* 
catch内でerrorResを送り、returnしてしまうパターン
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
		HTTPResponse res(e.GetStatusCode(), req);
		res.SendErrorResponse(ssocket);
		return;
	}
	HTTPResponse	res(method);
	res.SendResponse(ssocket);
 */

/*
フラグを持って入るパターン 
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
		flag = true;
	}
	HTTPResponse	res(method, status_code, req, flag);
	res.SendResponse(ssocket);
 */

/* 
変更なしでmethodのbody.size()でハンドリングするパターン
 */

