/**
 * conf/settings.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./settings.h"

// Core libraries.
#include <xalwart.core/path.h>
#include <xalwart.core/string_utils.h>

// Render libraries.
#include <xalwart.render/engine.h>
#include <xalwart.render/loaders.h>

// Framework modules.
#include "../render/library/default.h"
#include "../middleware/clickjacking.h"
#include "../middleware/common.h"
#include "../middleware/cookies.h"
#include "../middleware/http.h"
#include "../middleware/security.h"


__CONF_BEGIN__

YAML::Node Settings::_load_config(const std::string& file_name) const
{
	auto config_path = path::join(this->BASE_DIR, file_name);
	if (path::exists(config_path))
	{
		auto config = YAML::LoadFile(config_path);
		if (config && !config.IsNull() && !config.IsMap())
		{
			throw core::ValueError("'" + file_name + "' file must have map type");
		}

		return config;
	}

	return YAML::Node();
}

void Settings::_override_scalar(
	YAML::Node& config, YAML::Node& local, const std::string& key
)
{
	auto value = local[key];
	if (value)
	{
		if (value.IsScalar())
		{
			config[key] = local[key];
		}
		else if (value.IsNull())
		{
			if (config[key])
			{
				config.remove(key);
			}
		}
	}
}

void Settings::_override_sequence(
	YAML::Node& config, YAML::Node& local, const std::string& key
)
{
	auto value = local[key];
	if (value)
	{
		if (value.IsSequence())
		{
			config[key] = local[key];
		}
		else if (value.IsNull())
		{
			if (config[key])
			{
				config.remove(key);
			}
		}
	}
}

void Settings::_override_config(YAML::Node& config)
{
	auto local = this->_load_config(this->LOCAL_CONFIG_NAME);
	if (local.IsNull())
	{
		return;
	}

	Settings::_override_scalar(config, local, "debug");

	auto local_logger = local["logger"];
	if (local_logger)
	{
		if (local_logger.IsNull() && config["logger"])
		{
			config.remove("logger");
		}
		else if (local_logger.IsMap())
		{
			auto logger = config["logger"];
			if (!logger)
			{
				config["logger"] = local_logger;
			}
			else
			{
				auto local_levels = local_logger["levels"];
				if (local_levels)
				{
					if (local_levels.IsNull() && logger["levels"])
					{
						logger.remove("levels");
					}
					else if (local_levels.IsMap())
					{
						auto levels = logger["levels"];
						if (!levels)
						{
							logger["levels"] = local_levels;
						}
						else
						{
							Settings::_override_scalar(levels, local_levels, "info");
							Settings::_override_scalar(levels, local_levels, "debug");
							Settings::_override_scalar(levels, local_levels, "warning");
							Settings::_override_scalar(levels, local_levels, "error");
							Settings::_override_scalar(levels, local_levels, "fatal");
							Settings::_override_scalar(levels, local_levels, "print");
							logger["levels"] = levels;
						}
					}
				}

				auto local_out = local_logger["out"];
				if (local_out)
				{
					if (local_out.IsNull() && logger["out"])
					{
						logger.remove("out");
					}
					else if (local_out.IsMap())
					{
						auto out = logger["out"];
						if (!out)
						{
							logger["out"] = local_out;
						}
						else
						{
							Settings::_override_scalar(out, local_out, "console");
							Settings::_override_scalar(out, local_out, "file");
							Settings::_override_sequence(out, local_out, "files");
						}
					}
				}

				config["logger"] = logger;
			}
		}
	}

	Settings::_override_sequence(config, local, "allowed_hosts");

	auto local_timezone = config["timezone"];
	if (local_timezone)
	{
		if (local_timezone.IsNull() && config["timezone"])
		{
			config.remove("timezone");
		}
		else if (local_timezone.IsMap())
		{
			auto timezone = config["timezone"];
			if (!timezone)
			{
				config["timezone"] = local_timezone;
			}
			else
			{
				Settings::_override_scalar(timezone, local_timezone, "name");
				auto local_offset = local_timezone["offset"];
				if (local_offset.IsNull() && timezone["offset"])
				{
					timezone.remove("offset");
				}
				else if (local_offset.IsMap())
				{
					auto offset = timezone["offset"];
					if (!offset)
					{
						timezone["offset"] = local_offset;
					}
					else
					{
						Settings::_override_scalar(offset, local_offset, "days");
						Settings::_override_scalar(offset, local_offset, "seconds");
						Settings::_override_scalar(offset, local_offset, "microseconds");
						Settings::_override_scalar(offset, local_offset, "milliseconds");
						Settings::_override_scalar(offset, local_offset, "minutes");
						Settings::_override_scalar(offset, local_offset, "hours");
						Settings::_override_scalar(offset, local_offset, "weeks");
						timezone["offset"] = offset;
					}
				}

				config["timezone"] = timezone;
			}
		}
	}

	Settings::_override_scalar(config, local, "use_tz");
	Settings::_override_scalar(config, local, "charset");
	Settings::_override_scalar(config, local, "append_slash");
	Settings::_override_sequence(config, local, "disallowed_user_agents");
	Settings::_override_sequence(config, local, "ignorable_404_urls");
	Settings::_override_scalar(config, local, "secret_key");

	auto local_media = local["media"];
	if (local_media)
	{
		if (local_media.IsNull() && config["media"])
		{
			config.remove("media");
		}
		else if (local_media.IsMap())
		{
			auto media = config["media"];
			if (!media)
			{
				config["media"] = local_media;
			}
			else
			{
				Settings::_override_scalar(media, local_media, "root");
				Settings::_override_scalar(media, local_media, "url");
				config["media"] = media;
			}
		}
	}

	auto local_static = local["static"];
	if (local_static)
	{
		if (local_static.IsNull() && config["static"])
		{
			config.remove("static");
		}
		else if (local_static.IsMap())
		{
			auto static_ = config["static"];
			if (!static_)
			{
				config["static"] = local_static;
			}
			else
			{
				Settings::_override_scalar(static_, local_static, "root");
				Settings::_override_scalar(static_, local_static, "url");
				config["static"] = static_;
			}
		}
	}

	Settings::_override_scalar(config, local, "file_upload_max_memory_size");
	Settings::_override_scalar(config, local, "data_upload_max_memory_size");
	Settings::_override_scalar(config, local, "prepend_www");
	Settings::_override_scalar(config, local, "data_upload_max_number_fields");

	auto local_formats = local["formats"];
	if (local_formats)
	{
		if (local_formats.IsNull() && config["formats"])
		{
			config.remove("formats");
		}
		else if (local_formats.IsMap())
		{
			auto formats = config["formats"];
			if (!formats)
			{
				config["formats"] = local_formats;
			}
			else
			{
				Settings::_override_scalar(formats, local_formats, "date");
				Settings::_override_scalar(formats, local_formats, "datetime");
				Settings::_override_scalar(formats, local_formats, "time");
				Settings::_override_scalar(formats, local_formats, "year_month");
				Settings::_override_scalar(formats, local_formats, "month_day");
				Settings::_override_scalar(formats, local_formats, "short_date");
				Settings::_override_scalar(formats, local_formats, "short_datetime");
				config["formats"] = formats;
			}
		}
	}

	Settings::_override_scalar(config, local, "first_day_of_week");
	Settings::_override_scalar(config, local, "decimal_separator");
	Settings::_override_scalar(config, local, "use_thousand_separator");
	Settings::_override_scalar(config, local, "thousand_separator");
	Settings::_override_scalar(config, local, "x_frame_options");
	Settings::_override_scalar(config, local, "use_x_forwarded_host");
	Settings::_override_scalar(config, local, "use_x_forwarded_port");

	auto local_csrf = local["csrf"];
	if (local_csrf)
	{
		if (local_csrf.IsNull() && config["csrf"])
		{
			config.remove("csrf");
		}
		else if (local_csrf.IsMap())
		{
			auto csrf = config["csrf"];
			if (!csrf)
			{
				config["csrf"] = local_csrf;
			}
			else
			{
				auto local_cookie = local_csrf["cookie"];
				if (local_cookie)
				{
					if (local_cookie.IsNull() && csrf["cookie"])
					{
						csrf.remove("cookie");
					}
					else if (local_cookie.IsMap())
					{
						auto cookie = csrf["cookie"];
						if (!cookie)
						{
							csrf["cookie"] = local_cookie;
						}
						else
						{
							Settings::_override_scalar(cookie, local_cookie, "name");
							Settings::_override_scalar(cookie, local_cookie, "age");
							Settings::_override_scalar(cookie, local_cookie, "domain");
							Settings::_override_scalar(cookie, local_cookie, "path");
							Settings::_override_scalar(cookie, local_cookie, "secure");
							Settings::_override_scalar(cookie, local_cookie, "http_only");
							Settings::_override_scalar(cookie, local_cookie, "same_site");
							csrf["cookie"] = cookie;
						}
					}
				}

				Settings::_override_scalar(csrf, local_csrf, "header_name");
				Settings::_override_scalar(csrf, local_csrf, "use_sessions");
				Settings::_override_sequence(csrf, local_csrf, "trusted_origins");
				config["csrf"] = csrf;
			}
		}
	}

	Settings::_override_scalar(config, local, "use_ssl");
	auto local_secure = local["secure"];
	if (local_secure)
	{
		if (local_secure.IsNull() && config["secure"])
		{
			config.remove("secure");
		}
		else if (local_secure.IsMap())
		{
			auto secure = config["secure"];
			if (!secure)
			{
				config["secure"] = local_secure;
			}
			else
			{
				Settings::_override_scalar(secure, local_secure, "browser_xss_filter");
				Settings::_override_scalar(secure, local_secure, "content_type_no_sniff");
				Settings::_override_scalar(secure, local_secure, "hsts_include_subdomains");
				Settings::_override_scalar(secure, local_secure, "hsts_preload");
				Settings::_override_scalar(secure, local_secure, "hsts_seconds");
				Settings::_override_sequence(secure, local_secure, "redirect_exempt");
				Settings::_override_scalar(secure, local_secure, "referrer_policy");
				Settings::_override_scalar(secure, local_secure, "ssl_host");
				Settings::_override_scalar(secure, local_secure, "ssl_redirect");
				auto local_psslh = local_secure["proxy_ssl_header"];
				if (local_psslh)
				{
					if (local_psslh.IsNull() && secure["proxy_ssl_header"])
					{
						secure.remove("proxy_ssl_header");
					}
					else if (local_psslh.IsMap())
					{
						auto psslh = secure["proxy_ssl_header"];
						if (!psslh)
						{
							secure["proxy_ssl_header"] = local_psslh;
						}
						else
						{
							Settings::_override_scalar(psslh, local_psslh, "name");
							Settings::_override_scalar(psslh, local_psslh, "value");
							secure["proxy_ssl_header"] = psslh;
						}
					}
				}

				config["secure"] = secure;
			}
		}
	}

	Settings::_override_sequence(config, local, "installed_apps");
	Settings::_override_sequence(config, local, "middleware");

	auto local_env = local["templates_env"];
	if (local_env)
	{
		if (local_env.IsNull() && config["templates_env"])
		{
			config.remove("templates_env");
		}
		else if (local_env.IsMap())
		{
			auto env = config["templates_env"];
			if (!env)
			{
				config["templates_env"] = local_env;
			}
			else
			{
				Settings::_override_sequence(env, local_env, "directories");
				Settings::_override_sequence(env, local_env, "libraries");
				Settings::_override_scalar(env, local_env, "use_app_directories");
				Settings::_override_scalar(env, local_env, "auto_escape");
				Settings::_override_scalar(env, local_env, "use_default_engine");
			}
		}
	}
}

Settings::Settings(const std::string& base_dir)
{
	this->BASE_DIR = base_dir;
	using namespace middleware;
	this->_middleware = {
		{XFrameOptionsMiddleware::FULL_NAME, [this]() -> std::shared_ptr<middleware::IMiddleware> {
			return std::make_shared<XFrameOptionsMiddleware>(this);
		}},
		{CommonMiddleware::FULL_NAME, [this]() -> std::shared_ptr<middleware::IMiddleware> {
			return std::make_shared<CommonMiddleware>(this);
		}},
		{CookieMiddleware::FULL_NAME, [this]() -> std::shared_ptr<middleware::IMiddleware> {
			return std::make_shared<CookieMiddleware>(this);
		}},
		{ConditionalGetMiddleware::FULL_NAME, [this]() -> std::shared_ptr<middleware::IMiddleware> {
			return std::make_shared<ConditionalGetMiddleware>(this);
		}},
		{SecurityMiddleware::FULL_NAME, [this]() -> std::shared_ptr<middleware::IMiddleware> {
			return std::make_shared<SecurityMiddleware>(this);
		}}
	};

	using namespace render::lib;
	this->_libraries = {
		{DefaultLibrary::FULL_NAME, [this]() -> std::shared_ptr<ILibrary> {
			return std::make_shared<DefaultLibrary>(this);
		}}
	};

	this->_loaders = {
		{render::DefaultLoader::FULL_NAME, []() -> std::shared_ptr<render::DefaultLoader> {
			return std::make_shared<render::DefaultLoader>();
		}}
	};
}

void Settings::_init_env(YAML::Node& env)
{
	auto use_custom = env["use_custom"];
	if (use_custom && use_custom.as<bool>())
	{
		this->register_templates_engine(env);
	}
	else
	{
		std::vector<std::string> dirs;
		auto directories = env["directories"];
		if (directories && directories.IsSequence())
		{
			for (auto it = directories.begin(); it != directories.end(); it++)
			{
				if (!it->IsNull() && it->IsScalar())
				{
					auto p = it->as<std::string>();
					dirs.push_back(
						path::is_absolute(p) ? p : path::join(this->BASE_DIR, p)
					);
				}
			}
		}

		std::vector<std::shared_ptr<render::lib::ILibrary>> libs{
			this->_get_library(xw::render::lib::DefaultLibrary::FULL_NAME)
		};
		auto libraries = env["libraries"];
		if (libraries && libraries.IsSequence() && libraries.size() > 0)
		{
			this->register_libraries();
			for (auto it = libraries.begin(); it != libraries.end(); it++)
			{
				if (!it->IsNull() && it->IsScalar())
				{
					libs.push_back(this->_get_library(it->as<std::string>()));
				}
			}
		}

		std::vector<std::shared_ptr<render::ILoader>> loaders_vec;
		auto loaders = env["loaders"];
		if (loaders && loaders.IsSequence() && loaders.size() > 0)
		{
			this->register_loaders();
			for (auto it = loaders.begin(); it != loaders.end(); it++)
			{
				if (!it->IsNull() && it->IsScalar())
				{
					loaders_vec.push_back(this->_get_loader(it->as<std::string>()));
				}
			}
		}

		auto use_app_dirs = env["use_app_directories"];
		if (use_app_dirs && use_app_dirs.IsScalar() && use_app_dirs.as<bool>())
		{
			for (const auto& app : this->INSTALLED_APPS)
			{
				dirs.push_back(path::dirname(app->get_app_path()));
			}
		}

		auto auto_escape = env["auto_escape"];
		this->TEMPLATES_ENGINE = std::make_unique<render::DefaultEngine>(
			dirs,
			this->DEBUG,
			auto_escape && auto_escape.as<bool>(),
			loaders_vec,
			libs,
			this->LOGGER.get()
		);
	}
}

void Settings::_init_logger(YAML::Node& logger)
{
	core::LoggerConfig logger_config{};
	auto levels = logger["levels"];
	if (levels && levels.IsMap())
	{
		logger_config.enable_info = levels["info"].as<bool>(true);
		logger_config.enable_debug = levels["debug"].as<bool>(true);
		logger_config.enable_warning = levels["warning"].as<bool>(true);
		logger_config.enable_error = levels["error"].as<bool>(true);
		logger_config.enable_fatal = levels["fatal"].as<bool>(true);
		logger_config.enable_print = levels["print"].as<bool>(true);
	}

	auto out = logger["out"];
	if (out && out.IsMap())
	{
		if (out["console"].as<bool>(true))
		{
			logger_config.add_console_stream();
		}

		auto file_out = out["file"];
		if (file_out && file_out.IsScalar())
		{
			auto f_path = file_out.as<std::string>();
			auto full_path = path::is_absolute(f_path) ? f_path : path::join(this->BASE_DIR, f_path);
			logger_config.add_file_stream(full_path);
		}
		else
		{
			auto files_out = out["files"];
			if (files_out && files_out.IsSequence())
			{
				for (auto it = files_out.begin(); it != files_out.end(); it++)
				{
					if (!it->IsNull() && it->IsScalar())
					{
						auto f_path = it->as<std::string>();
						auto full_path = path::is_absolute(f_path) ? f_path : path::join(this->BASE_DIR, f_path);
						logger_config.add_file_stream(full_path);
					}
				}
			}
		}
	}

	this->LOGGER = core::Logger::get_instance(logger_config);
}

void Settings::_init_allowed_hosts(YAML::Node& allowed_hosts)
{
	for (auto it = allowed_hosts.begin(); it != allowed_hosts.end(); it++)
	{
		if (!it->IsNull() && it->IsScalar())
		{
			this->ALLOWED_HOSTS.push_back(it->as<std::string>());
		}
	}
}

void Settings::_init_disallowed_user_agents(YAML::Node& agents)
{
	for (auto it = agents.begin(); it != agents.end(); it++)
	{
		if (!it->IsNull() && it->IsScalar())
		{
			this->DISALLOWED_USER_AGENTS.emplace_back(it->as<std::string>());
		}
	}
}

void Settings::_init_ignorable_404_urls(YAML::Node& urls)
{
	for (auto it = urls.begin(); it != urls.end(); it++)
	{
		if (!it->IsNull() && it->IsScalar())
		{
			this->IGNORABLE_404_URLS.emplace_back(it->as<std::string>());
		}
	}
}

void Settings::_init_formats(YAML::Node& config)
{
	YAML::Node formats;
	if (config["formats"])
	{
		formats = config["formats"];
	}

	if (formats.IsMap())
	{
		this->DATE_FORMAT = formats["date"].as<std::string>("%b %d, %Y");
		this->DATETIME_FORMAT = formats["datetime"].as<std::string>("%b %d, %Y, %T");
		this->TIME_FORMAT = formats["time"].as<std::string>("%T");
		this->YEAR_MONTH_FORMAT = formats["year_month"].as<std::string>("%B %Y");
		this->MONTH_DAY_FORMAT = formats["month_day"].as<std::string>("%B %d");
		this->SHORT_DATE_FORMAT = formats["short_date"].as<std::string>("%m/%d/%Y");
		this->SHORT_DATETIME_FORMAT = formats["short_datetime"].as<std::string>("%m/%d/%Y %T");
	}
}

void Settings::_init_csrf(YAML::Node& config)
{
	YAML::Node csrf;
	if (config["csrf"])
	{
		csrf = config["csrf"];
	}

	YAML::Node cookie;
	if (csrf["cookie"])
	{
		cookie = csrf["cookie"];
	}

	this->CSRF_COOKIE_NAME = cookie["name"].as<std::string>("csrftoken");
	this->CSRF_COOKIE_AGE = cookie["age"].as<size_t>(60 * 60 * 24 * 7 * 52);
	this->CSRF_COOKIE_DOMAIN = cookie["domain"].as<std::string>("");
	this->CSRF_COOKIE_PATH = cookie["path"].as<std::string>("/");
	this->CSRF_COOKIE_SECURE = cookie["secure"].as<bool>(false);
	this->CSRF_COOKIE_HTTP_ONLY = cookie["http_only"].as<bool>(false);
	this->CSRF_COOKIE_SAME_SITE = cookie["same_site"].as<std::string>("Lax");

	this->CSRF_HEADER_NAME = csrf["header_name"].as<std::string>("X-XSRF-TOKEN");
	this->CSRF_USE_SESSIONS = csrf["use_sessions"].as<bool>(false);

	auto origins = csrf["trusted_origins"];
	if (origins && origins.IsSequence())
	{
		for (auto it = origins.begin(); it != origins.end(); it++)
		{
			if (!it->IsNull() && it->IsScalar())
			{
				this->CSRF_TRUSTED_ORIGINS.push_back(it->as<std::string>());
			}
		}
	}
}

void Settings::_init_secure(YAML::Node& config)
{
	YAML::Node secure;
	if (config["secure"])
	{
		secure = config["secure"];
	}

	this->SECURE_BROWSER_XSS_FILTER = secure["browser_xss_filter"].as<bool>(false);
	this->SECURE_CONTENT_TYPE_NO_SNIFF = secure["content_type_no_sniff"].as<bool>(true);
	this->SECURE_HSTS_INCLUDE_SUBDOMAINS = secure["hsts_include_subdomains"].as<bool>(false);
	this->SECURE_HSTS_PRELOAD = secure["hsts_preload"].as<bool>(false);
	this->SECURE_HSTS_SECONDS = secure["hsts_seconds"].as<size_t>(0);
	auto redirect_exempt = secure["redirect_exempt"];
	if (redirect_exempt && redirect_exempt.IsSequence())
	{
		for (auto it = redirect_exempt.begin(); it != redirect_exempt.end(); it++)
		{
			if (!it->IsNull() && it->IsScalar())
			{
				this->SECURE_REDIRECT_EXEMPT.push_back(it->as<std::string>());
			}
		}
	}

	this->SECURE_REFERRER_POLICY = secure["referrer_policy"].as<std::string>("");
	this->SECURE_SSL_HOST = secure["ssl_host"].as<std::string>("");
	this->SECURE_SSL_REDIRECT = secure["ssl_redirect"].as<bool>(false);
	auto psslh = secure["proxy_ssl_header"];
	if (psslh && psslh.IsMap())
	{
		this->SECURE_PROXY_SSL_HEADER = std::make_shared<std::pair<std::string, std::string>>(
			psslh["name"].as<std::string>("X-Forwarded-Proto"),
			psslh["value"].as<std::string>("https")
		);
	}
}

void Settings::_init_apps(YAML::Node& apps)
{
	for (auto it = apps.begin(); it != apps.end(); it++)
	{
		if (!it->IsNull() && it->IsScalar())
		{
			auto item = this->_get_app(it->as<std::string>());
			if (item)
			{
				this->INSTALLED_APPS.push_back(item);
			}
		}
	}
}

void Settings::_init_middleware(YAML::Node& middleware)
{
	for (auto it = middleware.begin(); it != middleware.end(); it++)
	{
		if (!it->IsNull() && it->IsScalar())
		{
			auto item = this->_get_middleware(it->as<std::string>());
			if (item)
			{
				this->MIDDLEWARE.push_back(item);
			}
		}
	}
}

void Settings::init()
{
	auto config = this->_load_config(this->CONFIG_NAME);
	this->_override_config(config);

	this->DEBUG = config["debug"].as<bool>(false);

	auto logger = config["logger"];
	if (logger && logger.IsMap())
	{
		this->_init_logger(logger);
	}
	else
	{
		this->register_logger();
	}

	auto allowed_hosts = config["allowed_hosts"];
	if (allowed_hosts && allowed_hosts.IsSequence())
	{
		this->_init_allowed_hosts(allowed_hosts);
	}

	auto timezone = config["timezone"];
	if (!timezone || !timezone.IsMap())
	{
		this->TIME_ZONE = std::make_shared<dt::Timezone>(dt::Timezone::UTC);
	}
	else
	{
		auto timezone_name = str::upper(timezone["name"].as<std::string>("UTC"));
		if (timezone_name == "UTC")
		{
			this->TIME_ZONE = std::make_shared<dt::Timezone>(dt::Timezone::UTC);
		}
		else
		{
			auto timezone_offset = timezone["offset"];
			if (timezone_offset && timezone_offset.IsMap())
			{
				this->TIME_ZONE = std::make_shared<dt::Timezone>(
					dt::Timedelta(
						timezone_offset["days"].as<long>(0),
						timezone_offset["seconds"].as<long>(0),
						timezone_offset["microseconds"].as<long>(0),
						timezone_offset["milliseconds"].as<long>(0),
						timezone_offset["minutes"].as<long>(0),
						timezone_offset["hours"].as<long>(0),
						timezone_offset["weeks"].as<long>(0)
					),
					timezone_name
				);
			}
			else
			{
				this->TIME_ZONE = std::make_shared<dt::Timezone>(
					dt::Timedelta(), timezone_name
				);
			}
		}
	}

	this->USE_TZ = config["use_tz"].as<bool>(false);

	this->CHARSET = config["charset"].as<std::string>("utf-8");

	this->APPEND_SLASH = config["append_slash"].as<bool>(true);

	auto disallowed_user_agents = config["disallowed_user_agents"];
	if (disallowed_user_agents && disallowed_user_agents.IsSequence())
	{
		this->_init_disallowed_user_agents(disallowed_user_agents);
	}

	auto ignorable_404_urls = config["ignorable_404_urls"];
	if (ignorable_404_urls && ignorable_404_urls.IsSequence())
	{
		this->_init_ignorable_404_urls(ignorable_404_urls);
	}

	this->SECRET_KEY = config["secret_key"].as<std::string>("");

	auto media = config["media"];
	if (media && media.IsMap())
	{
		auto p = media["root"].as<std::string>("media");
		this->MEDIA_ROOT = path::is_absolute(p) ? p : path::join(this->BASE_DIR, p);
		this->MEDIA_URL = media["url"].as<std::string>("/media/");
	}

	auto static_ = config["static"];
	if (static_ && static_.IsMap())
	{
		auto p = static_["root"].as<std::string>("static");
		this->STATIC_ROOT = path::is_absolute(p) ? p : path::join(this->BASE_DIR, p);
		this->STATIC_URL = static_["url"].as<std::string>("/static/");
	}

	this->FILE_UPLOAD_MAX_MEMORY_SIZE = config["file_upload_max_memory_size"].as<int>(2621440);
	this->DATA_UPLOAD_MAX_MEMORY_SIZE = config["data_upload_max_memory_size"].as<int>(2621440);

	this->PREPEND_WWW = config["prepend_www"].as<bool>(false);

	this->DATA_UPLOAD_MAX_NUMBER_FIELDS = config["data_upload_max_number_fields"].as<int>(1000);

	this->_init_formats(config);

	this->FIRST_DAY_OF_WEEK = config["first_day_of_week"].as<int>(0);

	this->DECIMAL_SEPARATOR = config["decimal_separator"].as<char>('.');

	this->USE_THOUSAND_SEPARATOR = config["use_thousand_separator"].as<bool>(false);

	this->THOUSAND_SEPARATOR = config["thousand_separator"].as<char>(',');

	this->X_FRAME_OPTIONS = config["x_frame_options"].as<std::string>("SAMEORIGIN");

	this->USE_X_FORWARDED_HOST = config["use_x_forwarded_host"].as<bool>(false);

	this->USE_X_FORWARDED_PORT = config["use_x_forwarded_port"].as<bool>(false);

	this->_init_csrf(config);

	this->USE_SSL = config["use_ssl"].as<bool>(false);

	this->_init_secure(config);

//	this->factory = new SettingsFactory(this, this->LOGGER.get());

	auto apps = config["installed_apps"];
	if (apps && apps.IsSequence() && apps.size() > 0)
	{
		this->LOGGER->print(
			"Loading installed apps...",
			core::Logger::Color::DEFAULT, '\0'
		);
		this->register_apps();
		this->_init_apps(apps);
		this->LOGGER->print(" Done.");
	}

	auto middleware = config["middleware"];
	if (middleware && middleware.IsSequence() && middleware.size() > 0)
	{
		this->LOGGER->print(
			"Loading middleware...",
			core::Logger::Color::DEFAULT, '\0'
		);
		this->register_middleware();
		this->_init_middleware(middleware);
		this->LOGGER->print(" Done.");
	}

	this->LOGGER->print(
		"Loading template environment...",
		core::Logger::Color::DEFAULT, '\0'
	);
	auto env = config["templates_engine"];
	if (env && env.IsMap())
	{
		this->_init_env(env);
	}
	else
	{
		this->register_libraries();
		this->register_loaders();
		this->register_templates_engine();
	}

	this->LOGGER->print(" Done.");

//	delete factory;
}

void Settings::prepare()
{
	if (!this->ROOT_APP && !this->INSTALLED_APPS.empty())
	{
		this->ROOT_APP = this->INSTALLED_APPS.front();
	}
}

std::shared_ptr<server::IServer> Settings::use_server(
	const std::function<core::Result<std::shared_ptr<http::IHttpResponse>>(
		http::HttpRequest*, const int&
	)>& handler,
	const collections::Dict<std::string, std::string>& kwargs
)
{
	return nullptr;
}

void Settings::register_logger()
{
}

void Settings::register_apps()
{
}

void Settings::register_middleware()
{
}

void Settings::register_libraries()
{
}

void Settings::register_templates_engine()
{
}

void Settings::register_templates_engine(const YAML::Node& config)
{
}

void Settings::register_loaders()
{
}

std::shared_ptr<apps::IAppConfig> Settings::_get_app(
	const std::string& full_name
) const
{
	if (this->_apps.find(full_name) != this->_apps.end())
	{
		return this->_apps.at(full_name)();
	}

	return nullptr;
}

std::shared_ptr<middleware::IMiddleware> Settings::_get_middleware(
	const std::string& full_name
) const
{
	if (this->_middleware.find(full_name) != this->_middleware.end())
	{
		return this->_middleware.at(full_name)();
	}

	return nullptr;
}

std::shared_ptr<render::lib::ILibrary> Settings::_get_library(
	const std::string& full_name
) const
{
	if (this->_libraries.find(full_name) != this->_libraries.end())
	{
		return this->_libraries.at(full_name)();
	}

	return nullptr;
}

std::shared_ptr<render::ILoader> Settings::_get_loader(
	const std::string& full_name
) const
{
	if (this->_loaders.find(full_name) != this->_loaders.end())
	{
		return this->_loaders.at(full_name)();
	}

	return nullptr;
}

__CONF_END__
