#include <gtest/gtest.h>
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"

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
