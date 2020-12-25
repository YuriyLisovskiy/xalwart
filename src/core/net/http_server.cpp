/**
 * core/net/http_server.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./http_server.h"

// C++ libraries.
#include <iostream>
#include <cstring>

// Core libraries.
#include <xalwart.core/datetime.h>
#include <xalwart.core/string_utils.h>

// Framework libraries.
#include "../parsers/request_parser.h"
#include "../../http/headers.h"


__NET_INTERNAL_BEGIN__

// Public methods.
HttpServer::HttpServer(HttpServer::context& ctx) : _finished(true)
{
	HttpServer::_check_context(ctx);

	this->_verbose = ctx.verbose;

	this->_host = ctx.host.c_str();
	this->_port = ctx.port;
	this->_use_ipv6 = ctx.use_ipv6;
	this->_logger = ctx.logger;

	this->_max_body_size = ctx.max_body_size;
	this->_threads_count = ctx.threads_count;
	this->_timeout_sec = ctx.timeout_sec;
	this->_timeout_us = ctx.timeout_us;

	// Default schema, https will be implemented in future.
	this->_schema = "http";

	this->_http_handler = ctx.handler;

	this->_media_root = ctx.media_root;

	this->_thread_pool = new core::internal::ThreadPool(this->_threads_count);
}

HttpServer::~HttpServer()
{
	this->finish();
}

void HttpServer::finish()
{
	delete this->_thread_pool;
	this->_thread_pool = nullptr;
	if (this->_finished)
	{
		return;
	}

	if (this->_socket.close() == SOCKET_ERROR)
	{
		this->_logger->error("Failed to close socket connection", _ERROR_DETAILS_);
	}

	HttpServer::_wsa_clean_up();
	this->_finished = true;
}

void HttpServer::listen_and_serve()
{
	if (this->_init() != 0)
	{
		return;
	}

#if defined(_WIN32) || defined(_WIN64)
	int status;
	WSADATA wsaData;
	status = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (status != 0)
	{
		if (this->_logger != nullptr)
		{
			this->_logger->error("WSAStartup() failed with error #" + std::to_string(status));
		}
		return;
	}
#endif

	// TODO: add timezone from settings!
//	auto tz = std::make_shared<dt::Timezone>(
//		dt::Timedelta(0, 0, 0, 0, 0, 0),
//		"UTC"
//	);
	std::string message = dt::Datetime::now().strftime("%B %d, %Y - %T") + "\n" +
		LIB_NAME + " version " + LIB_VERSION + "\n" +
		"Starting development server at " +
			std::string(this->_schema) + "://" + std::string(this->_host) + ":" + std::to_string(this->_port) + "/\n" +
		"Quit the server with CONTROL-C.\n";
	std::cout << message;
	std::cout.flush();

	this->_finished = false;
	this->_start_listener();
}

http::error HttpServer::send(http::IHttpResponse* response, const socket_t& client)
{
	return HttpServer::_send(response->serialize().c_str(), client);
}

http::error HttpServer::send(http::StreamingHttpResponse* response, const socket_t& client)
{
	std::string chunk;
	while (!(chunk = response->get_chunk()).empty())
	{
		auto err = HttpServer::_write(chunk.c_str(), chunk.size(), client);
		if (err)
		{
			return err;
		}
	}

	response->close();
	return http::error::none();
}

// Private methods.
int HttpServer::_init()
{
	int init_status = this->_initialize();
	if (init_status != 0)
	{
		return init_status;
	}

	init_status = this->_create();
	if (init_status == INVALID_SOCKET)
	{
		return init_status;
	}

	init_status = this->_set_reuse_socket();
	if (init_status == SOCKET_ERROR)
	{
		return init_status;
	}

	init_status = this->_bind();
	if (init_status == SOCKET_ERROR)
	{
		return init_status;
	}

	init_status = this->_listen();
	if (init_status == SOCKET_ERROR)
	{
		return init_status;
	}

	return 0;
}

int HttpServer::_initialize()
{
#if defined(_WIN32) || defined(_WIN64)
	if (this->_socket.initialize() != 0)
	{
		auto err = WSAGetLastError();
		this->_logger->error(
			"\"WSAStartup failed with error: " + std::to_string(err),
			_ERROR_DETAILS_
		);
		HttpServer::_wsa_clean_up();
		return err;
	}
#endif
	return 0;
}

int HttpServer::_create()
{
	if (this->_socket.create(this->_host, this->_port, this->_use_ipv6) == INVALID_SOCKET)
	{
		this->_logger->error(
			"Failed to initialize server at port " + std::to_string(this->_port),
			_ERROR_DETAILS_
		);

		HttpServer::_wsa_clean_up();
		return INVALID_SOCKET;
	}

	return 0;
}

int HttpServer::_bind()
{
	if (this->_socket.bind() == SOCKET_ERROR)
	{
		this->_logger->error(
			"Failed to bind socket to port " + std::to_string(this->_port),
			_ERROR_DETAILS_
		);

		this->_close_server_socket();
		return SOCKET_ERROR;
	}

	return 0;
}

int HttpServer::_listen()
{
	if (this->_socket.listen() == SOCKET_ERROR)
	{
		this->_logger->error(
			"Failed to listen at port " + std::to_string(this->_port),
			_ERROR_DETAILS_
		);

		this->_close_server_socket();
		return SOCKET_ERROR;
	}

	return 0;
}

int HttpServer::_set_reuse_socket()
{
	if (this->_socket.set_reuse_addr() == SOCKET_ERROR)
	{
		this->_logger->error(
			"Failed to set reuse address mode to socket",
			_ERROR_DETAILS_
		);

		this->_close_server_socket();
		return SOCKET_ERROR;
	}

	return 0;
}

void HttpServer::_close_server_socket()
{
	if (this->_socket.close() == SOCKET_ERROR)
	{
		this->_logger->error("Failed to close socket connection", _ERROR_DETAILS_);
	}

	HttpServer::_wsa_clean_up();
}

void HttpServer::_clean_up(const socket_t& client)
{
	int ret;
#if defined(_WIN32) || defined(_WIN64)
	ret = ::closesocket(client);
#elif defined(__unix__) || defined(__linux__)
	ret = ::close(client);
#else
	ret = SOCKET_ERROR;
#endif

	if (ret == SOCKET_ERROR)
	{
		this->_logger->error("Failed to close socket connection", _ERROR_DETAILS_);
	}

	HttpServer::_wsa_clean_up();
}

http::Result<std::shared_ptr<http::HttpRequest>> HttpServer::_handle_request(const socket_t& client)
{
	if (!this->_wait_for_client(client))
	{
		return http::Result<std::shared_ptr<http::HttpRequest>>::null();
	}

	core::internal::request_parser rp;
	std::string body_beginning;

	auto result = HttpServer::_read_headers(client, body_beginning);
	if (result.err)
	{
		return result.forward<std::shared_ptr<http::HttpRequest>>();
	}

	rp.parse_headers(result.value);
	if (rp.headers.find(http::CONTENT_LENGTH) != rp.headers.end())
	{
		size_t body_length = std::strtol(rp.headers[http::CONTENT_LENGTH].c_str(), nullptr, 10);
		std::string body;
		if (body_length == body_beginning.size())
		{
			body = body_beginning;
		}
		else
		{
			result = this->_read_body(client, body_beginning, body_length);
			if (result.err)
			{
				return result.forward<std::shared_ptr<http::HttpRequest>>();
			}

			body = result.value;
		}

		rp.parse_body(body, this->_media_root);
	}

	return http::Result(std::make_shared<http::HttpRequest>(
		rp.method,
		rp.path,
		rp.major_v,
		rp.minor_v,
		rp.query,
		rp.keep_alive,
		rp.content,
		rp.headers,
		rp.get_parameters,
		rp.post_parameters,
		rp.files_parameters
	));
}

http::Result<std::string> HttpServer::_read_body(
	const socket_t& client, const std::string& body_beginning, size_t body_length
) const
{
	std::string data;
	if (body_length <= 0)
	{
		return http::Result(data);
	}

	size_t size = body_beginning.size();
	if (size == body_length)
	{
		return http::Result(body_beginning);
	}

	msg_size_t ret;
	size_t buff_size = MAX_BUFF_SIZE < body_length ? MAX_BUFF_SIZE : body_length;
	char* buffer = (char*) calloc(buff_size, sizeof(char));
	while (size < body_length)
	{
	//	ret = read(client, buffer, buff_size);
		ret = ::recv(client, buffer, buff_size, 0);
		if (ret > 0)
		{
			data.append(buffer, ret);
			size += ret;
			if (size > this->_max_body_size)
			{
				return http::raise<http::EntityTooLargeError, std::string>(
					"Request data is too big", _ERROR_DETAILS_
				);
			}
		}
		else if (ret == -1)
		{
			auto result = HttpServer::_handle_error(buffer, _ERROR_DETAILS_);
			if (result.err)
			{
				return result.forward<std::string>();
			}

			read_result_enum status = result.value;
			if (status == read_result_enum::rr_continue)
			{
				continue;
			}
			else if (status == read_result_enum::rr_break)
			{
				break;
			}
		}
	}

	free(buffer);
	data = body_beginning + data;
	if (data.size() != body_length)
	{
		return http::raise<http::HttpError, std::string>(
			"Actual body size is not equal to header's value", _ERROR_DETAILS_
		);
	}

	return http::Result(data);
}

bool HttpServer::_wait_for_client(const socket_t& client) const
{
	struct timeval tv{};
	tv.tv_sec = this->_timeout_sec;
	tv.tv_usec = this->_timeout_us;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(client, &fds);
	int ret = select(client + 1, &fds, nullptr, nullptr, &tv);
	if (ret == -1)
	{
		if (this->_verbose)
		{
			this->_logger->error("select failed: " + std::string(strerror(errno)));
		}
	}
	else if (ret == 0)
	{
		if (this->_verbose)
		{
			this->_logger->error("timeout waiting for client");
		}
	}
	else if (!FD_ISSET(client, &fds))
	{
		if (this->_verbose)
		{
			this->_logger->error("file descriptor is not set");
		}
	}
	else
	{
		return true;
	}

	return false;
}

http::Result<std::string> HttpServer::_read_headers(
	const socket_t& client, std::string& body_beginning
)
{
	msg_size_t ret;
	unsigned long size = 0;
	std::string data;
	size_t headers_delimiter_pos = std::string::npos;
	std::string delimiter = "\r\n\r\n";

	char* buffer = (char*) calloc(MAX_BUFF_SIZE, sizeof(char));
	do
	{
		ret = ::recv(client, buffer, MAX_BUFF_SIZE, 0);
		if (ret > 0)
		{
			data.append(buffer, ret);
			size += ret;

			// Maybe it is better to check each header value's size.
			if (size > MAX_HEADERS_SIZE)
			{
				return http::raise<http::EntityTooLargeError, std::string>(
					"Request data is too big", _ERROR_DETAILS_
				);
			}
		}
		else if (ret == -1)
		{
			auto result = HttpServer::_handle_error(buffer, _ERROR_DETAILS_);
			if (result.err)
			{
				return result.forward<std::string>();
			}

			read_result_enum status = result.value;
			if (status == read_result_enum::rr_continue)
			{
				continue;
			}
			else if (status == read_result_enum::rr_break)
			{
				break;
			}
		}

		headers_delimiter_pos = data.find(delimiter);
	}
	while (headers_delimiter_pos == std::string::npos);

	free(buffer);
	if (headers_delimiter_pos == std::string::npos)
	{
		return http::raise<http::HttpError, std::string>(
			"Invalid http request has been received", _ERROR_DETAILS_
		);
	}

	headers_delimiter_pos += delimiter.size();
	body_beginning = data.substr(headers_delimiter_pos);
	return http::Result(data.substr(0, headers_delimiter_pos));
}

void HttpServer::_start_listener()
{
	bool running = true;
	while (running)
	{
		try
		{
			socket_t connection = this->_socket.accept();
			if (connection != INVALID_SOCKET)
			{
				this->_thread_pool->push(
					[this, connection] { this->_thread_func(connection); }
				);
			}
			else
			{
				this->_logger->error("Invalid socket connection", _ERROR_DETAILS_);
				running = false;
			}
		}
		catch (core::InterruptException&)
		{
			running = false;
		}
	}
}

void HttpServer::_serve_connection(const socket_t& client)
{
	Measure<std::chrono::milliseconds> measure;
	if (this->_verbose)
	{
		measure.start();
	}

	auto result = this->_handle_request(client);
	if (result)
	{
		if (result.catch_(http::HttpError))
		{
			this->_logger->error(result.err.msg);
		}
		else
		{
			auto request = result.value;
			this->_http_handler(request.get(), client);
			if (this->_verbose)
			{
				measure.end();
				std::cout << '\n' << request->method() << " request took " << measure.elapsed() << " ms\n";
			}
		}
	}

	if (Socket::close_socket(client) == SOCKET_ERROR)
	{
		this->_logger->error("Failed to close client connection");
	}
}

void HttpServer::_thread_func(const socket_t& client)
{
	this->_serve_connection(client);
	HttpServer::_clean_up(client);
}

// Private static functions.
http::Result<HttpServer::read_result_enum> HttpServer::_handle_error(
	char* buffer, int line, const char *function, const char *file
)
{
	switch (errno)
	{
		case EBADF:
		case EFAULT:
		case EINVAL:
		case ENXIO:
			// Fatal error.
			free(buffer);
			return http::raise<http::HttpError, read_result_enum>(
				"Critical error: " + std::to_string(errno), line, function, file
			);
		case EIO:
		case ENOBUFS:
		case ENOMEM:
			// Resource acquisition failure or device error.
			free(buffer);
			http::raise<http::HttpError, read_result_enum>(
				"Resource failure: " + std::to_string(errno), line, function, file
			);
		case EINTR:
			// TODO: Check for user interrupt flags.
		case ETIMEDOUT:
		case EAGAIN:
			// Temporary error.
			return http::Result(HttpServer::read_result_enum::rr_continue);
		case ECONNRESET:
		case ENOTCONN:
			// Connection broken.
			// Return the data we have available and exit
			// as if the connection was closed correctly.
			return http::Result(HttpServer::read_result_enum::rr_break);
		default:
			free(buffer);
			return http::raise<http::HttpError, read_result_enum>(
				"Returned -1: " + std::to_string(errno), line, function, file
			);
	}
}

void HttpServer::_check_context(HttpServer::context& ctx)
{
	if (ctx.host.empty())
	{
		throw ValueError("host address can not be nullptr", _ERROR_DETAILS_);
	}

	if (ctx.port == 0)
	{
		throw ValueError("server port can not be nullptr", _ERROR_DETAILS_);
	}

	if (ctx.handler == nullptr)
	{
		throw ValueError("HttpServer::Context::handler can not be nullptr", _ERROR_DETAILS_);
	}

	if (ctx.logger == nullptr)
	{
		throw ValueError("logger can not be nullptr", _ERROR_DETAILS_);
	}

	str::rtrim(ctx.media_root, "/");
	str::rtrim(ctx.media_root, "\\");
}

http::error HttpServer::_send(const char* data, const socket_t& client)
{
	if (::send(client, data, std::strlen(data), 0) == SOCKET_ERROR)
	{
		return http::error(
			http::HttpError, "Failed to send bytes to socket connection", _ERROR_DETAILS_
		);
	}

	return http::error::none();
}

http::error HttpServer::_write(const char* data, size_t bytesToWrite, const socket_t& client)
{
	if (::write(client, data, bytesToWrite) == -1)
	{
		return http::error(
			http::HttpError, "Failed to send bytes to socket connection", _ERROR_DETAILS_
		);
	}

	return http::error::none();
}

void HttpServer::_wsa_clean_up()
{
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
}

__NET_INTERNAL_END__
