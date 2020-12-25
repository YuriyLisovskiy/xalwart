/**
 * urls/resolver.h
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 *
 * Purpose: resolves given url as string.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./pattern.h"
#include "../http/result.h"


__URLS_BEGIN__

/// Searches path in urlpatterns and returns an expression
/// to process request if path is found, otherwise returns nullptr.
extern std::function<http::Result<std::shared_ptr<http::IHttpResponse>>(
	http::HttpRequest* request,
	conf::Settings* settings
)> resolve(
	const std::string& path, std::vector<std::shared_ptr<UrlPattern>>& urlpatterns
);

/// Return true if the given path can be found in urlpatterns,
/// false otherwise.
extern bool is_valid_path(
	const std::string& path, std::vector<std::shared_ptr<UrlPattern>>& urlpatterns
);

__URLS_END__
