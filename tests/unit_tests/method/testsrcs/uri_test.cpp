#include <gtest/gtest.h>
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

TEST(UriTest, UriGetType)
{
	Config config("conf/default.conf");
	// Config config("conf/simple2.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	HTTPRequest req(HTTPRequest::GET);
	HTTPMethod method;

	method.ParseReq(req);

	std::cout << "req_Uri: " << req.GetTarget() << std::endl;
	std::cout << "req_method: " << req.GetMethod() << std::endl;
	URI uri(req.GetTarget(), servers.at(0), method.GetMethod());

	std::cout << "\n~ URI_INFO ~" << std::endl;
	std::cout << uri.GetType() << std::endl;
	std::cout << uri.GetRawPath() << std::endl;
	std::cout << uri.GetPath() << std::endl;
	std::cout << uri.GetQuery() << std::endl;
}

TEST(MethodTest, CreateBody)
{
	// Config config("conf/default.conf");
	Config config("conf/simple2.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	HTTPRequest req(HTTPRequest::GET);
	HTTPMethod method;
	int status_code;


	method.ParseReq(req);
	URI uri(req.GetTarget(), servers.at(0), method.GetMethod());

	std::cout << "\n~ URI_INFO ~" << std::endl;
	std::cout << uri.GetType() << std::endl;
	std::cout << uri.GetRawPath() << std::endl;
	std::cout << uri.GetPath() << std::endl;
	std::cout << uri.GetQuery() << std::endl;

	status_code = method.ExecHTTPMethod(req, servers.at(0));
	HTTPResponse res(method, status_code, req);
	std::cout << res.GetResMsg() << std::endl;
}
