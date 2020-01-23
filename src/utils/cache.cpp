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
 * An implementation of cache.h.
 */

#include "./cache.h"


__CACHE_BEGIN__

void set_response_etag(http::HttpResponseBase* response)
{
	if (!response->is_streaming() && response->content_length() > 0)
	{
		response->set_header(
			http::E_TAG,
			utils_http::quote_etag(crypto::md5(response->get_content()).digest())
		);
	}
}

http::HttpResponseBase* get_conditional_response(
	http::HttpRequest* request,
	const std::string& etag,
	const std::string& last_modified,
	http::HttpResponseBase* response
)
{
	// TODO:
	return nullptr;
}

__CACHE_END__
