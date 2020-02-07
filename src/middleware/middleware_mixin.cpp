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
 * An implementation of middleware/middleware_mixin.h
 */

#include "./middleware_mixin.h"


__MIDDLEWARE_BEGIN__

MiddlewareMixin::MiddlewareMixin(conf::Settings* settings)
{
	this->settings = settings;
}

http::HttpResponseBase* MiddlewareMixin::process_request(http::HttpRequest* request)
{
	return nullptr;
}

http::HttpResponseBase* MiddlewareMixin::process_response(
	http::HttpRequest* request, http::HttpResponseBase* response
)
{
	return nullptr;
}

__MIDDLEWARE_END__
