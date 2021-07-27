/**
 * management/commands/runserver.cpp
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 */

#include "./start_server.h"

// Base libraries.
#include <xalwart.base/string_utils.h>
#include <xalwart.base/datetime.h>


__MANAGEMENT_COMMANDS_BEGIN__

StartServerCommand::StartServerCommand(
	conf::IModuleConfig* config,
	conf::Settings* settings,
	std::function<std::shared_ptr<net::abc::IServer>(log::ILogger*, const Kwargs&)> make_server
) : Command(
	config, settings, "start-server", "Starts a web application"
), make_server(std::move(make_server))
{
	if (!this->make_server)
	{
		throw ImproperlyConfigured(
			"StartServerCommand: server initializer must be instantiated in order to use the application",
			_ERROR_DETAILS_
		);
	}

	std::string ipv4 = R"((\d{1,3}(?:\.\d{1,3}){3})|localhost)";
	this->_ipv4_regex = re::Regex(ipv4);

	std::string ipv6 = R"(\[([a-fA-F0-9:]+)\]|\[localhost\])";
	this->_ipv6_regex = re::Regex(ipv6);

	std::string fqdn = R"([a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*)";

	std::string port = R"(\d+)";
	this->_ipv4_ipv6_port_regex = re::Regex(
		R"((?:(()" + ipv4 + R"()|()" + ipv6 + R"()|()" + fqdn + R"()):)?()" + port + R"())"
	);
}

Kwargs StartServerCommand::get_kwargs()
{
	auto kwargs = Command::get_kwargs();
	kwargs.set("xw.workers", std::to_string(this->_threads_count));
	kwargs.set("xw.max_body_size", std::to_string(this->settings->DATA_UPLOAD_MAX_MEMORY_SIZE));

	// TODO: add timeout to command args!
	kwargs.set("xw.timeout_sec", std::to_string(5));
	kwargs.set("xw.timeout_usec", std::to_string(0));
	return kwargs;
}

void StartServerCommand::add_flags()
{
	this->_addr_port_flag = this->flag_set->make_string(
		"bind", "", "Server address and port formatted as host:port"
	);
	this->_addr_flag = this->flag_set->make_string(
		"host", "", "Server address"
	);
	this->_port_flag = this->flag_set->make_uint16(
		"port", this->DEFAULT_PORT, "Server port"
	);
	this->_threads_flag = this->flag_set->make_unsigned_long(
		"threads", this->DEFAULT_THREADS, "Threads count"
	);
	this->_use_ipv6_flag = this->flag_set->make_bool(
		"use-ipv6", false, "Use IPv6 address or not (used in case when host is set to 'localhost')"
	);
	this->_no_colors_flag = this->flag_set->make_bool(
		"no-colors", false, "Disable colors in logs"
	);
}

void StartServerCommand::handle()
{
	this->settings->LOGGER->use_colors(!this->_no_colors_flag->get());
	if (!this->settings->DEBUG && this->settings->ALLOWED_HOSTS.empty())
	{
		throw CommandError(
			"You must set 'allowed_hosts' if 'debug' is false.",
			_ERROR_DETAILS_
		);
	}

	this->retrieve_args(
		this->_host, this->_port, this->_use_ipv6, this->_threads_count
	);
	auto server = this->make_server(this->settings->LOGGER.get(), this->get_kwargs());
	try
	{
		server->bind(this->_host, this->_port);
		std::string message = dt::Datetime::now().strftime("%B %d, %Y - %T") + "\n" +
		                      v::framework_name + " version " + v::version.to_string() + "\n" +
		                      "Starting development server at " +
		                      "http://" + this->_host + ":" + std::to_string(this->_port) + "/\n" +
		                      "Quit the server with CONTROL-C.";
		server->listen(message);
	}
	catch (const InterruptException& exc)
	{
		this->settings->LOGGER->debug("Interrupted");
	}
	catch (const BaseException& exc)
	{
		this->settings->LOGGER->error(exc);
	}
	catch (const std::exception& exc)
	{
		this->settings->LOGGER->error(exc.what(), _ERROR_DETAILS_);
	}

	server->close();
}

void StartServerCommand::retrieve_args(
	std::string& host, uint16_t& port, bool& use_ipv6, size_t& threads_count
)
{
	// Setup address and port.
	auto host_port_str = this->_addr_port_flag->get();
	if (!host_port_str.empty())
	{
		if (!this->_ipv4_ipv6_port_regex.match(host_port_str))
		{
			throw CommandError(
				host_port_str + " is not valid host:port pair", _ERROR_DETAILS_
			);
		}

		auto groups = this->_ipv4_ipv6_port_regex.groups();
		auto address = (groups[1].empty() ? this->DEFAULT_IPV4_HOST : groups[1]);
		use_ipv6 = this->_ipv6_regex.match(address);
		auto trimmed_addr = str::trim(address, "[]");
		if (trimmed_addr == "localhost")
		{
			address = use_ipv6 ? this->DEFAULT_IPV6_HOST : this->DEFAULT_IPV4_HOST;
		}

		host = address;
		port = groups[4].empty() ? this->DEFAULT_PORT : (uint16_t) std::stoi(groups[4]);
	}
	else
	{
		auto address = this->_addr_flag->get();
		if (address.empty())
		{
			use_ipv6 = this->_use_ipv6_flag->get();
			address = use_ipv6 ? this->DEFAULT_IPV6_HOST : this->DEFAULT_IPV4_HOST;
		}
		else
		{
			if (this->_ipv6_regex.match(address))
			{
				use_ipv6 = true;
			}
			else if (!this->_ipv4_regex.match(address))
			{
				throw CommandError(
					this->_addr_flag->get_raw() + " is invalid address",
					_ERROR_DETAILS_
				);
			}
		}

		auto trimmed_addr = str::trim(address, "[]");
		if (trimmed_addr == "localhost")
		{
			address = use_ipv6 ? this->DEFAULT_IPV6_HOST : this->DEFAULT_IPV4_HOST;
		}

		host = address;
		if (this->_port_flag->valid())
		{
			port = this->_port_flag->get();
		}
		else
		{
			throw CommandError(
				this->_port_flag->get_raw() + " is invalid port",
				_ERROR_DETAILS_
			);
		}
	}

	if (!this->_threads_flag->valid())
	{
		throw CommandError(
			"threads count is not a valid positive integer: " + this->_threads_flag->get_raw(),
			_ERROR_DETAILS_
		);
	}

	threads_count = this->_threads_flag->get();
}

__MANAGEMENT_COMMANDS_END__
