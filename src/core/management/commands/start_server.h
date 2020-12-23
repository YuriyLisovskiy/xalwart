/**
 * core/management/commands/runserver.h
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 *
 * Purpose: runs server from command line.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "../app_command.h"
#include "../../../core/net/http_server.h"


__CORE_COMMANDS_BEGIN__

class StartServerCommand final: public AppCommand
{
private:
	flags::StringFlag* _addr_port_flag;
	flags::StringFlag* _addr_flag;
	flags::LongIntFlag* _port_flag;
	flags::LongIntFlag* _threads_flag;
	flags::BoolFlag* _use_ipv6_flag;

	rgx::Regex* _ipv4_ipv6_port_regex;
	rgx::Regex* _ipv4_regex;
	rgx::Regex* _ipv6_regex;
	rgx::Regex* _port_regex;

	const std::string DEFAULT_IPV4_HOST = "127.0.0.1";
	const std::string DEFAULT_IPV6_HOST = "[::1]";
	const uint16_t DEFAULT_PORT = 8000;
	const size_t DEFAULT_THREADS = 16;

protected:
	void add_flags() final;
	void handle() final;
	std::function<void(http::HttpRequest*, const core::net::internal::socket_t&)> get_handler();
	bool static_is_allowed(const std::string& static_url);
	void build_static_patterns(std::vector<std::shared_ptr<urls::UrlPattern>>& patterns);
	void build_app_patterns(std::vector<std::shared_ptr<urls::UrlPattern>>& patterns);
	void setup_server_ctx(core::net::internal::HttpServer::context& ctx);

	static std::unique_ptr<http::IHttpResponse> process_request_middleware(
		http::HttpRequest* request, conf::Settings* settings
	);
	static std::unique_ptr<http::IHttpResponse> process_response_middleware(
		http::HttpRequest* request,
		http::IHttpResponse* response,
		conf::Settings* settings
	);
	static std::unique_ptr<http::IHttpResponse> process_urlpatterns(
		http::HttpRequest* request,
		std::vector<std::shared_ptr<urls::UrlPattern>>& urlpatterns,
		conf::Settings* settings
	);
	static void send_response(
		http::HttpRequest* request,
		http::IHttpResponse* response,
		const core::net::internal::socket_t& client,
		conf::Settings* settings
	);
	static void log_request(
		const std::string& info,
		unsigned short status_code,
		conf::Settings* settings
	);

public:
	explicit StartServerCommand(apps::IAppConfig* config, conf::Settings* settings);
	~StartServerCommand() final;
};

__CORE_COMMANDS_END__
