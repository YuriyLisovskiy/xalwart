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

/*
 * tcp/ip server definition.
 */

#ifndef LEAF_TCP_SERVER_H
#define LEAF_TCP_SERVER_H

#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include <functional>

#include "../globals.h"
#include "tcp_def.h"

__INTERNAL_BEGIN__

typedef std::function<const std::string(const std::string&)> tcpHandler;

class TcpServer
{
private:
	uint16_t port;
	std::string host;
	std::vector<std::thread> pool;
	tcpHandler handler;

	void startListener();
	void processRequest(const SOCKET& client);
	static void sendResponse(const char* data, const SOCKET& connection);
	static void closeSocket(const SOCKET& sock, const int& how);
	static void cleanUp(const SOCKET& connection);
	static std::string recvAll(const SOCKET& connection);
public:
	explicit TcpServer(const char* host, uint16_t port, leaf::internal::tcpHandler handler);
	void listenAndServe();
	~TcpServer();
};

__INTERNAL_END__


#endif // LEAF_TCP_SERVER_H