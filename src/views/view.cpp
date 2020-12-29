/**
 * views/view.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./view.h"

// Core libraries.
#include <xalwart.core/utility.h>
#include <xalwart.core/string_utils.h>

// Framework libraries.
#include "../conf/settings.h"

__VIEWS_BEGIN__

View::View(conf::Settings* settings)
	: _request(nullptr), _allowed_methods_list({"options"})
{
	this->_settings = settings;
	if (!this->_settings->LOGGER)
	{
		throw core::ImproperlyConfigured("View: LOGGER instance must be configured");
	}

	this->_logger = this->_settings->LOGGER.get();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::null()
{
	return core::Result<std::shared_ptr<http::IHttpResponse>>::null();
}

View::View(
	const std::vector<std::string>& allowed_methods,
	conf::Settings* settings
) : View(settings)
{
	this->_allowed_methods_list = allowed_methods;
	if (std::find(allowed_methods.begin(), allowed_methods.end(), "options") == allowed_methods.end())
	{
		this->_allowed_methods_list.emplace_back("options");
	}
}

void View::setup(http::HttpRequest* request)
{
	this->_request = request;
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::dispatch(Args* args)
{
	if (this->_request == nullptr)
	{
		throw core::NullPointerException(
			core::utility::demangle(typeid(*this).name()) +
			" instance has not initialized request."
			" Did you override setup() and forget to call base method?",
			_ERROR_DETAILS_
		);
	}

	std::string method = core::str::lower(this->_request->method());
	auto result = core::Result<std::shared_ptr<http::IHttpResponse>>::null();
	if (method == "get")
	{
		result = this->get(this->_request, args);
	}
	else if (method == "post")
	{
		result = this->post(this->_request, args);
	}
	else if (method == "put")
	{
		result = this->put(this->_request, args);
	}
	else if (method == "patch")
	{
		result = this->patch(this->_request, args);
	}
	else if (method == "delete")
	{
		result = this->delete_(this->_request, args);
	}
	else if (method == "head")
	{
		result = this->head(this->_request, args);
	}
	else if (method == "options")
	{
		result = this->options(this->_request, args);
	}
	else if (method == "trace")
	{
		result = this->trace(this->_request, args);
	}

	if (!result)
	{
		result = this->http_method_not_allowed(this->_request);
	}

	return result;
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::http_method_not_allowed(http::HttpRequest* request)
{
	this->_logger->warning(
		"Method Not Allowed (" + request->method() + "): " + request->path(),
		_ERROR_DETAILS_
	);
	return this->response<http::HttpResponseNotAllowed>("", this->allowed_methods());
}

std::vector<std::string> View::allowed_methods()
{
	std::vector<std::string> result;
	for (const auto& method : this->_allowed_methods_list)
	{
		bool found = std::find(
			this->_http_method_names.begin(),
			this->_http_method_names.end(),
			core::str::lower(method)
		) != this->_http_method_names.end();
		if (found)
		{
			result.push_back(core::str::upper(method));
		}
	}

	return result;
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::get(http::HttpRequest* request, Args* args)
{
	return this->null();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::post(http::HttpRequest* request, Args* args)
{
	return this->null();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::put(http::HttpRequest* request, Args* args)
{
	return this->null();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::patch(http::HttpRequest* request, Args* args)
{
	return this->null();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::delete_(http::HttpRequest* request, Args* args)
{
	return this->null();
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::head(http::HttpRequest* request, Args* args)
{
	return this->get(request, args);
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::options(http::HttpRequest* request, Args* args)
{
	auto response = std::make_shared<http::HttpResponse>(200);
	auto allowed_methods = this->allowed_methods();
	response->set_header(
		"Allow",
		core::str::join(allowed_methods.begin(), allowed_methods.end(), ", ")
	);
	response->set_header("Content-Length", "0");
	return this->response(response);
}

core::Result<std::shared_ptr<http::IHttpResponse>> View::trace(http::HttpRequest* request, Args* args)
{
	return this->null();
}

__VIEWS_END__
