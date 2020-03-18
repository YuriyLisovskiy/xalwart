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
 * utils/cache.h
 *
 * Purpose:
 * 	Provides utils for developing middleware.
 */

#pragma once

// C++ libraries.
#include <string>
#include <vector>
#include <memory>

// Module definitions.
#include "./_def_.h"

// Wasp libraries.
#include "./http.h"
#include "./crypto/md5.h"
#include "../core/utility.h"
#include "../http/headers.h"
#include "../http/request.h"
#include "../http/response.h"


__CACHE_INTERNAL_BEGIN__

/// Test the If-Match comparison as defined in section 3.1 of RFC 7232.
extern bool _if_match_passes(
	const std::string& target_etag, std::vector<std::string>& etags
);

/// Test the If-None-Match comparison as defined in section 3.2 of RFC 7232.
extern bool _if_none_match_passes(
	const std::string& target_etag, std::vector<std::string>& etags
);

/// Test the If-Modified-Since comparison as defined in section 3.3 of RFC 7232.
extern bool _if_modified_since_passes(
	long last_modified, long if_modified_since
);

/// Test the If-Unmodified-Since comparison as defined in section 3.4 of
/// RFC 7232.
extern bool _if_unmodified_since_passes(
	long last_modified, long if_unmodified_since
);

extern std::unique_ptr<http::IHttpResponse> _precondition_failed(
	http::HttpRequest* request
);

extern std::unique_ptr<http::IHttpResponse> _not_modified(
	http::HttpRequest* request, http::IHttpResponse* response
);

__CACHE_INTERNAL_END__


__CACHE_BEGIN__

extern void set_response_etag(http::IHttpResponse* response);

extern std::unique_ptr<http::IHttpResponse> get_conditional_response(
	http::HttpRequest* request,
	const std::string& etag,
	long last_modified,
	http::IHttpResponse* response
);

__CACHE_END__
