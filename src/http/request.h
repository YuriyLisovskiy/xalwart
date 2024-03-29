/**
 * http/request.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 * Implementation is based on Golang 1.15.8: net/http/request.go
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

// Base libraries.
#include <xalwart.base/exceptions.h>
#include <xalwart.base/io.h>
#include <xalwart.base/net/request_context.h>
#include <xalwart.base/vendor/nlohmann/json.h>

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./headers.h"
#include "./cookie/parser.h"
#include "./mime/multipart/body_reader.h"
#include "./mime/content_types.h"
#include "./interfaces.h"


__HTTP_BEGIN__

// TESTME: Request
// TODO: docs for 'Request'
class Request : public IRequest
{
public:
	explicit Request(
		const net::RequestContext& context,
		ssize_t max_file_upload_size, ssize_t max_fields_count,
		ssize_t max_header_length, ssize_t max_headers_count,
		long long int multipart_max_memory,
		std::map<std::string, std::string> environment
	);

	[[nodiscard]]
	inline std::string method() const final
	{
		return this->_method;
	}

	[[nodiscard]]
	const URL& url() const final
	{
		return this->_url;
	}

	[[nodiscard]]
	inline Proto proto() const final
	{
		return this->_proto;
	}

	[[nodiscard]]
	inline const std::map<std::string, std::string>& headers() const final
	{
		return this->_headers;
	}

	// 'user_agent' returns the client's User-Agent, if sent in the request.
	[[nodiscard]]
	inline std::string user_agent() const final
	{
		return this->get_header(USER_AGENT, "");
	}

	// TODO: docs for 'has_header'
	[[nodiscard]]
	inline bool has_header(const std::string& key) const final
	{
		return this->_headers.contains(key);
	}

	// TODO: docs for 'get_header'
	[[nodiscard]]
	inline std::string get_header(const std::string& key, const std::string& default_value="") const final
	{
		return this->_headers.contains(key) ? this->_headers.at(key) : default_value;
	}

	// TODO: docs for 'set_header'
	inline void set_header(const std::string& key, const std::string& value) final
	{
		if (this->_headers.contains(key))
		{
			this->_headers[key] = value;
		}
		else
		{
			this->_headers.insert(std::make_pair(key, value));
		}
	}

	[[nodiscard]]
	inline bool is_json() const final
	{
		return this->get_header(CONTENT_TYPE, "").find(mime::APPLICATION_JSON) != std::string::npos;
	}

	// 'cookies' parses and returns the HTTP cookies sent with the request.
	[[nodiscard]]
	inline std::vector<Cookie> cookies() const final
	{
		return parse_cookies(this->get_header(COOKIE, ""), "");
	}

	// 'cookie' returns the named cookie provided in the request or
	// 'nullptr' if not found.
	// If multiple cookies match the given name, only one cookie will
	// be returned.
	[[nodiscard]]
	inline std::optional<Cookie> cookie(const std::string& name) const final
	{
		auto cookies = parse_cookies(this->get_header(COOKIE), name);
		if (cookies.empty())
		{
			return {};
		}

		return cookies[0];
	}

	// 'referer' returns the referring URL, if sent in the request.
	//
	// 'referer' is misspelled as in the request itself, a mistake from the
	// earliest days of HTTP.  This value can also be fetched from the
	// 'header' dictionary as header.get("Referer"); the benefit of making it available
	// as a method is that the compiler can diagnose programs that use the
	// alternate (correct English) spelling req.referrer() but cannot
	// diagnose programs that use header.get("Referrer").
	[[nodiscard]]
	inline std::string referer() const final
	{
		return this->get_header(REFERER, "");
	}

	inline const Query& form() final
	{
		this->_parse_form();
		return this->_form.value();
	}

	inline const mime::multipart::Form& multipart_form() final
	{
		this->_parse_multipart_form();
		return this->_multipart_form.value();
	}

	inline nlohmann::json json() final
	{
		this->_parse_json_data();
		return this->_json.value();
	}

	[[nodiscard]]
	inline std::string host() const final
	{
		return this->_host;
	}

	[[nodiscard]]
	inline ssize_t content_length() const final
	{
		return this->_content_length;
	}

	[[nodiscard]]
	const std::vector<std::string>& transfer_encoding() const final
	{
		return this->_transfer_encoding;
	}

	[[nodiscard]]
	inline const std::map<std::string, std::string>& environment() const final
	{
		return this->_environment;
	}

	// TODO: docs for 'scheme'
	[[nodiscard]]
	std::string scheme(const std::optional<conf::Secure::Header>& secure_proxy_ssl_header) const final;

	// Return the HTTP host using the environment or request headers.
	std::string get_host(
		const std::optional<conf::Secure::Header>& secure_proxy_ssl_header,
		bool use_x_forwarded_host, bool use_x_forwarded_port, bool debug, std::vector<std::string> allowed_hosts
	) final;

	// TODO: docs for 'is_secure'
	[[nodiscard]]
	inline bool is_secure(const std::optional<conf::Secure::Header>& secure_proxy_ssl_header) const final
	{
		return this->scheme(secure_proxy_ssl_header) == "https";
	}

private:
	// Specifies the URI being requested.
	//
	// The URL is parsed from the URI supplied on the Request-Line.
	// For most requests, fields other than 'path' and 'raw_query' will
	// be empty. (See RFC 7230, Section 5.3)
	URL _url;

	// Specifies the HTTP method (GET, POST, PUT, etc.).
	std::string _method;

	// The protocol version for incoming server requests.
	Proto _proto;

	// Contains the request header fields either received
	// by the server.
	//
	// If a server received a request with header lines,
	//
	//	Host: example.com
	//	accept-encoding: gzip, deflate
	//	Accept-Language: en-us
	//	fOO: Bar
	//	foo: two
	//
	// then
	//
	//	header = Header(std::map<std::string, std::vector<std::string>>{
	//	    {"Accept-Encoding", {"gzip, deflate"}},
	//	    {"Accept-Language", {"en-us"}},
	//	    {"Foo", {"Bar", "two"}},
	//	});
	//
	// For incoming requests, the Host header is promoted to the
	// 'Request::host' field and removed from the 'header' map.
	//
	// HTTP defines that header names are case-insensitive. The
	// request parser implements this by making the first character
	// and any characters following a hyphen uppercase and the
	// rest lowercase.
	std::map<std::string, std::string> _headers;

	// Body is the request's body.
	std::shared_ptr<io::ILimitedBufferedReader> _body_reader = nullptr;

	// '_form' contains the parsed form data, including both the URL
	// field's query parameters and the PATCH, POST, or PUT form data.
	std::optional<Query> _form;

	// '_multipart_form' is the parsed multipart form, including file uploads.
	std::optional<mime::multipart::Form> _multipart_form;

	// '_json' is parsed request body when Content-Type is 'application/json'.
	std::optional<nlohmann::json> _json;

	// '_host' specifies the host on which the
	// URL is sought. For HTTP/1 (per RFC 7230, section 5.4), this
	// is either the value of the "Host" header or the host name
	// given in the URL itself. For HTTP/2, it is the value of the
	// ":authority" pseudo-header field.
	// It may be of the form "host:port". For international domain
	// names, 'host' may be in Punycode or Unicode form.
	std::string _host;

	// 'content_length' records the length of the associated content.
	// The value -1 indicates that the length is unknown.
	// Values >= 0 indicate that the given number of bytes were
	// written to body.
	ssize_t _content_length;

	// 'transfer_encoding' lists the transfer encodings from outermost to
	// innermost. An empty list denotes the "identity" encoding.
	// 'transfer_encoding' can usually be ignored; chunked encoding is
	// automatically added and removed as necessary when sending and
	// receiving requests.
	std::vector<std::string> _transfer_encoding{};

	// TODO: docs for Request::environment
	std::map<std::string, std::string> _environment;

	ssize_t max_file_upload_size;
	ssize_t max_fields_count;
	ssize_t max_header_length;
	ssize_t max_headers_count;
	ssize_t multipart_max_memory;

	// '_parse_form' populates '_form'.
	//
	// For all requests, '_parse_form' parses the raw query from the URL and updates
	// '_form'.
	//
	// For POST, PUT, and PATCH requests, it also reads the request body, parses it
	// as a form and puts the results into '_form'. Request body
	// parameters take precedence over URL query string values in '_form'.
	//
	// For other HTTP methods, or when the Content-Type is not
	// application/x-www-form-urlencoded, the request 'body' is not read, and
	// 'form' is initialized to a non-nil, empty value.
	//
	// '_parse_multipart_form' and '_parse_json_data' call '_parse_form' automatically.
	// '_parse_form' is idempotent.
	void _parse_form();

	// '_parse_multipart_form' parses a request body as multipart/form-data.
	// The whole request body is parsed and up to a total of `multipart_max_memory` bytes of
	// its file parts are stored in memory, with the remainder stored on
	// disk in temporary files.
	// After one call to `_parse_multipart_form`, subsequent calls have no effect.
	void _parse_multipart_form();

	// '_parse_json_data' parses a request body as application/json.
	// After one call to `_parse_json_data`, subsequent calls have no effect.
	void _parse_json_data();

	[[nodiscard]]
	std::unique_ptr<mime::multipart::BodyReader> _multipart_reader(bool allow_mixed) const;

	// Return the HTTP host using the environment or request headers. Skip
	// allowed hosts protection, so may return an insecure host.
	[[nodiscard]]
	std::string _get_raw_host(
		bool use_x_forwarded_host, bool use_x_forwarded_port,
		const std::optional<conf::Secure::Header>& secure_proxy_ssl_header
	) const;

	// TODO: dos for 'get_port'
	[[nodiscard]]
	std::string _get_port(bool use_x_forwarded_port) const;
};

// TESTME: valid_method
//
//    Method         = "OPTIONS"                ; Section 9.2
//                   | "GET"                    ; Section 9.3
//                   | "HEAD"                   ; Section 9.4
//                   | "POST"                   ; Section 9.5
//                   | "PUT"                    ; Section 9.6
//                   | "DELETE"                 ; Section 9.7
//                   | "TRACE"                  ; Section 9.8
//                   | "CONNECT"                ; Section 9.9
//                   | extension-method
//  extension-method = token
//    token          = 1*<any CHAR except CTLs or separators>
inline bool valid_method(const std::string& method)
{
	return !method.empty() && !std::any_of(method.begin(),  method.end(), is_not_token);
}

// TESTME: has_port
// Given a string of the form "host", "host:port", or "[ipv6::address]:port",
// return true if the string includes a port.
extern bool has_port(const std::string& host);

// TESTME: remove_empty_port
// 'remove_empty_port' strips the empty port in ":port" to ""
// as mandated by RFC 3986 Section 6.2.3.
extern std::string remove_empty_port(const std::string& host);

// TESTME: read_full_request_body
// TODO: docs for 'read_full_request_body'
extern void read_full_request_body(std::string& buffer, io::IReader* reader, ssize_t content_length);

// TESTME: read_body_to_string
// TODO: docs for 'read_body_to_string'
extern std::tuple<std::string, bool> read_body_to_string(
	http::IRequest* request, io::ILimitedBufferedReader* body_reader, const std::string& target_content_type
);

// TESTME: parse_post_form
// TODO: docs for 'parse_post_form'
extern Query parse_post_form(http::IRequest* request, io::ILimitedBufferedReader* body_reader);

// TESTME: 'get_content_type'
// TODO: docs for 'get_content_type'
extern std::string parse_content_type(http::IRequest* request);

__HTTP_END__
