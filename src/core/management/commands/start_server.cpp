/*
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
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

/**
 * An implementation of core/managements/commands/runserver.h
 */

#include "./start_server.h"


__CORE_COMMANDS_BEGIN__

StartServerCommand::StartServerCommand(apps::IAppConfig* config, conf::Settings* settings)
	: AppCommand(config, settings, "start-server", "Starts Wasp web server")
{
	this->_host_port_flag = nullptr;
	this->_threads_flag = nullptr;
	this->_host_flag = nullptr;
	this->_port_flag = nullptr;
	this->_use_ipv6_flag = nullptr;

	std::string port = R"(\d+)";
	this->_port_regex = new rgx::Regex(port);

	std::string ipv4 = R"(\d{1,3}(?:\.\d{1,3}){3})";
	this->_ipv4_regex = new rgx::Regex(ipv4);

	std::string ipv6 = R"(\[[a-fA-F0-9:]+\])";
	this->_ipv6_regex = new rgx::Regex(ipv6);

	std::string fqdn = R"([a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*)";

	this->_ipv4_ipv6_port_regex = new rgx::Regex(
		R"((?:(()" + ipv4 + R"()|()" + ipv6 + R"()|()" + fqdn + R"()):)?()" + port + R"())"
	);
}

StartServerCommand::~StartServerCommand()
{
	delete this->_ipv4_ipv6_port_regex;
	delete this->_ipv4_regex;
	delete this->_ipv6_regex;
	delete this->_port_regex;
}

void StartServerCommand::add_flags()
{
	this->_host_port_flag = this->_flag_set->make_string(
		"host-port", "", "Server host and port formatted as ip_addr:port"
	);
	this->_host_flag = this->_flag_set->make_string(
		"host", "", "Server host"
	);
	this->_port_flag = this->_flag_set->make_long(
		"port", this->DEFAULT_PORT, "Server port"
	);
	this->_threads_flag = this->_flag_set->make_long(
		"threads", this->DEFAULT_THREADS, "Tells Wasp how many threads to use"
	);
	this->_use_ipv6_flag = this->_flag_set->make_bool(
		"use-ipv6", false, "Detect if use IPv6 or not"
	);
}

void StartServerCommand::handle()
{
	if (!this->settings->DEBUG && this->settings->ALLOWED_HOSTS.empty())
	{
		throw CommandError("You must set ALLOWED_HOSTS if DEBUG is false.");
	}

	core::net::internal::HttpServer::context ctx{};

	// TODO: remove http server's verbose setting
	ctx.verbose = true;

	this->setup_server_ctx(ctx);

	core::net::internal::HttpServer server(ctx);
	try
	{
		server.listen_and_serve();
	}
	catch (const core::InterruptException& exc)
	{
		this->settings->LOGGER->debug("Interrupted");
	}
	catch (const core::BaseException& exc)
	{
		this->settings->LOGGER->error(exc);
	}
	catch (const std::exception& exc)
	{
		this->settings->LOGGER->error(exc.what(), _ERROR_DETAILS_);
	}
}

std::function<void(http::HttpRequest*, const core::net::internal::socket_t&)>
StartServerCommand::get_handler()
{
	// Check if static files can be served
	// and create necessary urls.
	this->build_static_patterns(this->settings->ROOT_URLCONF);

	// Retrieve main app patterns and append them
	// to result.
	this->build_app_patterns(this->settings->ROOT_URLCONF);

	auto handler = [this](
		http::HttpRequest* request, const core::net::internal::socket_t& client
	) mutable -> void
	{
		http::HttpResponseBase* error_response = nullptr;
		http::HttpResponseBase* response = nullptr;
		try
		{
			response = StartServerCommand::process_request_middleware(
				request, this->settings
			);
			if (!response)
			{
				response = StartServerCommand::process_urlpatterns(
					request, this->settings->ROOT_URLCONF, this->settings
				);
				if (!response)
				{
					response = new http::HttpResponseNotFound("<h2>404 - Not Found</h2>");
				}

				response = StartServerCommand::process_response_middleware(
					request, response, this->settings
				);
			}
		}
		catch (const core::ErrorResponseException& exc)
		{
			this->settings->LOGGER->error(exc);
			auto status_code = exc.status_code() < 400 ? 500 : exc.status_code();
			auto err_msg = "<p style=\"font-size: 24px;\" >" + std::to_string(status_code) + "</p>"
						   "<p>" + std::string(exc.what()) + "</p>";
			error_response = new http::HttpResponse(err_msg, status_code);
		}
		catch (const core::BaseException& exc)
		{
			this->settings->LOGGER->error(exc);
			error_response = new http::HttpResponseServerError(
				"<p style=\"font-size: 24px;\" >Internal Server Error</p><p>" + std::string(exc.what()) + "</p>"
			);
		}
		catch (const std::exception& exc)
		{
			this->settings->LOGGER->error(exc.what(), __LINE__, "request handler function", __FILE__);
			error_response = new http::HttpResponseServerError(
				"<p style=\"font-size: 24px;\" >Internal Server Error</p>"
			);
		}

		if (!response)
		{
			// Response was not instantiated.
			error_response = new http::HttpResponseServerError("<h2>500 - Internal Server Error</h2>");
		}

		if (error_response)
		{
			delete response;

			// TODO: render error response;
			//  if setting.DEBUG is true render detailed error,
			//  otherwise render status code and reason phrase.
			response = error_response;
		}

		send_response(request, response, client, this->settings);
		delete response;
	};

	return handler;
}

bool StartServerCommand::static_is_allowed(const std::string& static_url)
{
	auto parser = core::internal::url_parser();
	parser.parse(static_url);

	// Allow serving local static files if debug and
	// static url is local.
	return this->settings->DEBUG && parser.hostname.empty();
}

void StartServerCommand::build_static_patterns(std::vector<urls::UrlPattern>& patterns)
{
	if (!this->settings->STATIC_ROOT.empty() && this->static_is_allowed(this->settings->STATIC_URL))
	{
		patterns.push_back(urls::make_static(this->settings->STATIC_URL, this->settings->STATIC_ROOT));
	}

	if (!this->settings->MEDIA_ROOT.empty() && this->static_is_allowed(this->settings->MEDIA_URL))
	{
		patterns.push_back(
			urls::make_static(this->settings->MEDIA_URL, this->settings->MEDIA_ROOT, "media")
		);
	}
}

void StartServerCommand::build_app_patterns(std::vector<urls::UrlPattern>& patterns)
{
	if (this->settings->ROOT_APP)
	{
		auto apps_patterns = this->settings->ROOT_APP->get_urlpatterns();
		patterns.insert(patterns.end(), apps_patterns.begin(), apps_patterns.end());
	}
}

void StartServerCommand::setup_server_ctx(core::net::internal::HttpServer::context& ctx)
{
	// Setup host and port.
	auto host_port_str = this->_host_port_flag->get();
	if (!host_port_str.empty())
	{
		if (!this->_ipv4_ipv6_port_regex->match(host_port_str))
		{
			throw CommandError(host_port_str + " is not valid address:port pair");
		}

		auto groups = this->_ipv4_ipv6_port_regex->groups();
		auto address = (groups[1].empty() ? this->DEFAULT_IPV4_HOST : groups[1]);
		if (this->_ipv6_regex->match(address))
		{
			ctx.use_ipv6 = true;
		}

		ctx.host = address;
		ctx.port = groups[2].empty() ? this->DEFAULT_PORT : (uint16_t) std::stoi(groups[2]);
	}
	else
	{
		auto address = this->_host_flag->get();
		if (address.empty())
		{
			ctx.use_ipv6 = this->_use_ipv6_flag->get();
			address = ctx.use_ipv6 ? this->DEFAULT_IPV6_HOST : this->DEFAULT_IPV4_HOST;
		}
		else
		{
			if (this->_ipv6_regex->match(address))
			{
				ctx.use_ipv6 = true;
			}
			else if (!this->_ipv4_regex->match(address))
			{
				throw CommandError(this->_host_flag->get_raw() + " is invalid address");
			}
		}

		ctx.host = address;
		auto raw_port = this->_port_flag->get_raw();
		if (raw_port.empty())
		{
			ctx.port = this->DEFAULT_PORT;
		}
		else
		{
			if (!this->_port_regex->match(raw_port))
			{
				throw CommandError(raw_port + " is invalid port");
			}

			ctx.port = this->_port_flag->get();
		}
	}

	ctx.max_body_size = this->settings->DATA_UPLOAD_MAX_MEMORY_SIZE;
	ctx.media_root = this->settings->MEDIA_ROOT;
	ctx.logger = this->settings->LOGGER;

	if (std::regex_match(this->_threads_flag->get_raw(), std::regex(R"(\d+)")))
	{
		throw CommandError("threads count is not a number: " + this->_threads_flag->get_raw());
	}

	ctx.threads_count = this->_threads_flag->get();
	ctx.handler = this->get_handler();
}

http::HttpResponseBase* StartServerCommand::process_request_middleware(
	http::HttpRequest* request, conf::Settings* settings
)
{
	http::HttpResponseBase* response = nullptr;
	for (auto& middleware : settings->MIDDLEWARE)
	{
		response = middleware->process_request(request);
		if (response)
		{
			break;
		}
	}

	return response;
}

http::HttpResponseBase* StartServerCommand::process_urlpatterns(
	http::HttpRequest* request,
	std::vector<urls::UrlPattern>& urlpatterns,
	conf::Settings* settings
)
{
	http::HttpResponseBase* response = nullptr;
	auto apply = urls::resolve(request->path(), settings->ROOT_URLCONF);
	if (apply)
	{
		response = apply(request, settings);
	}

	return response;
}

http::HttpResponseBase* StartServerCommand::process_response_middleware(
	http::HttpRequest* request,
	http::HttpResponseBase* response,
	conf::Settings* settings
)
{
	for (long int i = (long int) settings->MIDDLEWARE.size() - 1; i >= 0; i--)
	{
		auto curr_response = settings->MIDDLEWARE[i]->process_response(request, response);
		if (curr_response)
		{
			delete response;
			response = curr_response;
			break;
		}
	}

	return response;
}

void StartServerCommand::send_response(
	http::HttpRequest* request,
	http::HttpResponseBase* response,
	const core::net::internal::socket_t& client,
	conf::Settings* settings
)
{
	if (response->is_streaming())
	{
		auto* streaming_response = dynamic_cast<http::StreamingHttpResponse*>(response);
		core::net::internal::HttpServer::send(streaming_response, client);
	}
	else
	{
		core::net::internal::HttpServer::send(response, client);
	}

	log_request(
		request->method() + " " +
		request->path() + " " +
		"HTTP" + (settings->USE_SSL ? "S/" : "/") +
		request->version(),
		response->status(),
		settings
	);
}

void StartServerCommand::log_request(
	const std::string& info,
	unsigned short status_code,
	conf::Settings* settings
)
{
	if (settings->LOGGER)
	{
		core::Logger::Color color = core::Logger::Color::GREEN;
		if (status_code >= 400)
		{
			color = core::Logger::Color::YELLOW;
		}
		else if (status_code >= 500)
		{
			color = core::Logger::Color::RED;
		}

		settings->LOGGER->print(str::format(
			"[{0!s}] \"{1!s}\" {2!d}",
			dt::now().strftime("%d/%b/%Y %T").c_str(),
			info.c_str(),
			status_code
		), color);
	}
}

__CORE_COMMANDS_END__