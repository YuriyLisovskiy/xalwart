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
 * HTTP/1.1 response base implementation.
 * TODO: write docs
 */

#include "response.h"


__WASP_BEGIN__

HttpResponseBase::HttpResponseBase(
	unsigned short int status,
	std::string contentType,
	const std::string& reason,
	const std::string& charset
)
{
	this->_streaming = false;
	this->_headers = QueryDict<std::string, std::string>(true);
	this->_closed = false;

	if (status != 0)
	{
		if (status < 100 || status > 599)
		{
			throw ValueError("HTTP status code must be an integer from 100 to 599.", _ERROR_DETAILS_);
		}
		this->_status = status;
	}
	else
	{
		this->_status = 200;
	}

	this->_charset = charset;

	if (contentType.empty())
	{
		contentType = "text/html; charset=" + this->_charset;
	}
	this->_headers.set("Content-Type", contentType);
}

void HttpResponseBase::setHeader(const std::string& key, const std::string& value)
{
	this->_headers.set(key, value);
}

void HttpResponseBase::removeHeader(const std::string& key)
{
	this->_headers.remove(key);
}

bool HttpResponseBase::hasHeader(const std::string& key)
{
	return this->_headers.contains(key);
}

void HttpResponseBase::setCookie(
	const std::string& name,
	const std::string& value,
	const std::string& expires,
	const std::string& domain,
	const std::string& path,
	bool isSecure,
	bool isHttpOnly
)
{
	this->_cookies.set(name, Cookie(name, value, expires, domain, path, isSecure, isHttpOnly));
}

void HttpResponseBase::setSignedCookie(
	const std::string& name,
	const std::string& value,
	const std::string& salt,
	const std::string& expires,
	const std::string& domain,
	const std::string& path,
	bool isSecure,
	bool isHttpOnly
)
{
	// TODO:
}

void HttpResponseBase::deleteCookie(const std::string& name, const std::string& path, const std::string& domain)
{
	bool isSecure = std::strncmp(name.c_str(), "__Secure-", 9) == 0 || std::strncmp(name.c_str(), "__Host-", 7) == 0;
	this->_cookies.set(name, Cookie(name, "", "Thu, 01 Jan 1970 00:00:00 GMT", domain, path, isSecure));
}

std::string HttpResponseBase::getReasonPhrase()
{
	if (!this->_reasonPhrase.empty())
	{
		return this->_reasonPhrase;
	}
	if (internal::HTTP_STATUS.contains(this->_status))
	{
		return internal::HTTP_STATUS.get(this->_status).first;
	}
	return "Unknown Status Code";
}

void HttpResponseBase::setReasonPhrase(std::string value)
{
	if (value.empty())
	{
		value = "Unknown Status Code";
	}
	this->_reasonPhrase = value;
}

void HttpResponseBase::close()
{
	this->_closed = true;
}

void HttpResponseBase::write(const std::string& content)
{
	throw WaspException("This HttpResponseBase instance is not writable", _ERROR_DETAILS_);
}

void HttpResponseBase::flush()
{
}

unsigned long int HttpResponseBase::tell()
{
	throw WaspException("This HttpResponseBase instance cannot tell its position", _ERROR_DETAILS_);
}

bool HttpResponseBase::readable()
{
	return false;
}

bool HttpResponseBase::seekable()
{
	return false;
}

bool HttpResponseBase::writable()
{
	return false;
}

void HttpResponseBase::writeLines(const std::vector<std::string>& lines)
{
	throw WaspException("This HttpResponseBase instance is not writable", _ERROR_DETAILS_);
}

std::string HttpResponseBase::serializeHeaders()
{
	auto expr = [](std::pair<std::string, std::string> _p) -> std::string { return _p.first + ": " + _p.second; };
	std::string result;
	for (auto it = this->_headers.cbegin(); it != this->_headers.cend(); it++)
	{
		result.append(expr(*it));
		if (std::next(it) != this->_headers.cend())
		{
			result.append("\r\n");
		}
	}
	return result;
}

// HttpResponse implementation
HttpResponse::HttpResponse(
	const std::string& content,
	unsigned short status,
	const std::string& contentType,
	const std::string& reason,
	const std::string& charset
) : HttpResponseBase(status, contentType, reason, charset)
{
	this->_content = content;
	this->_streaming = false;
}

void HttpResponse::write(const std::string& content)
{
	this->_content.append(content);
}

unsigned long int HttpResponse::tell()
{
	return this->_content.size();
}

bool HttpResponse::writable()
{
	return true;
}

void HttpResponse::writeLines(const std::vector<std::string>& lines)
{
	for (const auto & line : lines)
	{
		this->write(line);
	}
}

std::string HttpResponse::serialize() {
	this->setHeader("Date", dt::gmtnow().strftime("%a, %d %b %Y %T %Z"));
	this->setHeader("Content-Length", std::to_string(this->_content.size()));

	auto reasonPhrase = this->getReasonPhrase();

	auto headers = this->serializeHeaders();

	return "HTTP/1.1 " + std::to_string(this->_status) + " " + reasonPhrase + "\r\n" +
	       headers + "\r\n\r\n" + this->_content;
}

__WASP_END__