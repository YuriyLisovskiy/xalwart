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
 * url definition.
 * TODO: write docs.
 */

#ifndef WASP_HTTP_URL_H
#define WASP_HTTP_URL_H

#include <string>

#include "../globals.h"
#include "parsers/url_parser.h"
#include "../core/exceptions.h"


__WASP_BEGIN__

class Url final
{
private:
	std::string _strUrl;
	std::string _scheme;
	std::string _username;
	std::string _password;
	std::string _hostname;
	std::string _port;
	std::string _path;
	std::string _query;
	std::string _fragment;
	uint16_t _integerPort;

public:
	explicit Url(const std::string& url);
	explicit Url(const char* url);

	[[nodiscard]] std::string scheme() const;
	[[nodiscard]] std::string username() const;
	[[nodiscard]] std::string password() const;
	[[nodiscard]] std::string hostname() const;
	[[nodiscard]] uint16_t port() const;
	[[nodiscard]] std::string path() const;
	[[nodiscard]] std::string query() const;
	[[nodiscard]] std::string fragment() const;
	[[nodiscard]] std::string str() const;
};

__WASP_END__


#endif // WASP_HTTP_URL_H