/*
 * Copyright (c) 2019 Yuriy Lisovskiy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <vector>

#include "../src/http/http_server.h"
#include "../src/http/request.h"
#include "../src/http/response.h"


using wasp::internal::HttpServer;
using std::string;
using std::cout;

wasp::HttpResponseBase* handler(wasp::HttpRequest& request)
{
	cout << "\n" << request.method() << " " << request.path() << " " << request.version() << "\n";
	auto begin = request.headers.cbegin();
	auto end = request.headers.cend();

	while (begin != end)
	{
		cout << (*begin).first << ": " << (*begin).second << '\n';
		begin++;
	}

	std::cout << request.body() << "\n";

//	for (auto it = request.COOKIES.cbegin(); it != request.COOKIES.cend(); it++)
//	{
//		std::cout << it->first << ": " << it->second << '\n';
//	}

	std::string body("<form action=\"/hello\" method=\"post\" enctype=\"multipart/form-data\">\n"
	                 "\t<input type=\"file\" name=\"super_file\" />\n"
	                 "\t<input type=\"password\" name=\"first_name\" />\n"
	                 "\t<input type=\"submit\" value=\"send\" />\n"
	                 "\t</form>\n");

	return new wasp::HttpResponse(body);
}

int main()
{
	try
	{
		HttpServer::Context ctx{};
		ctx.handler = handler;
		ctx.port = 9000;

		HttpServer server(ctx);

		server.listenAndServe();
	}
	catch (const std::exception& exc)
	{
		cout << exc.what();
	}

	return 0;
}
