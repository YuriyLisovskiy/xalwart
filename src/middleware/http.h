/*
 * Copyright (c) 2019 Yuriy Lisovskiy
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
 * http.h
 * Purpose:
 * 	Handle conditional GET operations. If the response has an ETag or
 * 	Last-Modified header and the request has If-None-Match or If-Modified-Since,
 * 	replace the response with HttpNotModified. Add an ETag header if needed.
 */

#pragma once

// Module definitions
#include "./_def_.h"

// Wasp libraries.
#include "./middleware_mixin.h"
#include "../core/string/str.h"
#include "../http/headers.h"
#include "../utils/cache.h"
#include "../utils/http.h"


__MIDDLEWARE_BEGIN__

class ConditionalGetMiddleware : public MiddlewareMixin
{
protected:

	/// Return true if an ETag header should be added to response.
	static bool needs_etag(http::HttpResponseBase* response);

public:
	explicit ConditionalGetMiddleware(conf::Settings* settings);

	http::HttpResponseBase* process_response(
		http::HttpRequest* request, http::HttpResponseBase* response
	) override;
};

__MIDDLEWARE_END__