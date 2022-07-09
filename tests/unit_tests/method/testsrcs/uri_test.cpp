#include <gtest/gtest.h>
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
// #include "URI.hpp"
/* 
TEST(Uritest, Location)
{
	Config config("conf/simple2.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator site = servers.begin();

	for (; site != servers.end(); site++)
	{
		std::vector<LocationDirective> locations = site->GetLocations();
		std::vector<LocationDirective>::const_iterator lite = locations.begin();
		for (; lite != locations.end(); lite++)
			std::cout << lite->GetPath() << std::endl;
	}
}
 */

TEST(MethodTest, FindServer)
{
	Config config("conf/simple.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	HTTPRequest req(HTTPRequest::GET);
	HTTPMethod method;
	ServerDirective server;

	std::cout << "servers.size: " << servers.size() << std::endl;
	method.ParseReq(req, servers, &server);
	std::vector<LocationDirective> locations = server.GetLocations();
	std::cout << "locations.size: " << locations.size() << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
		std::cout << locations[i].GetPath() << std::endl;
}


TEST(UriTest, UriGetType)
{
	Config config("conf/simple.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	HTTPRequest req(HTTPRequest::GET);
	HTTPMethod method;
	ServerDirective server;

	method.ParseReq(req, servers, &server);

	URI uri(method.GetUri(), server, method.GetMethod());

	std::cout << uri.GetType() << std::endl;
	std::cout << uri.GetRawPath() << std::endl;
	std::cout << uri.GetPath() << std::endl;
	std::cout << uri.GetQuery() << std::endl;

}
