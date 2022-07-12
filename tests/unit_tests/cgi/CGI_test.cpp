#include <gtest/gtest.h>
#include "CGI.hpp"

TEST(CGITest, standard)
{
	CGI		cgi;

	cgi.ExecuteCGI("./test.cgi");
	cgi.ParseCGI();

	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>\nWelcome CGI test page!! ;)\n</div>\n</body>\n</html>", cgi.GetBody());
}
