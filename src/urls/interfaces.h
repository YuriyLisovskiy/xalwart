/**
 * urls/interfaces.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Interfaces for 'urls' module.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Other libraries.
#include "../http/request.h"
#include "../http/response.h"
#include "../conf/_def_.h"


__CONF_BEGIN__

struct Settings;

__CONF_END__


__URLS_BEGIN__

// TODO: docs for 'IPattern'
class IPattern
{
public:
	[[nodiscard]]
	virtual std::string get_name() const = 0;

	[[nodiscard]]
	virtual std::string get_pattern_str() const = 0;

	virtual void add_prefix(const std::string& prefix) = 0;

	virtual void add_namespace(const std::string& ns) = 0;

	virtual std::unique_ptr<http::IResponse> apply(http::IRequest* request, conf::Settings* settings) = 0;

	virtual bool match(const std::string& url) = 0;

	[[nodiscard]]
	virtual std::string build(const std::vector<std::string>& args) const = 0;
};

__URLS_END__
