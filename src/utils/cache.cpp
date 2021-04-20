/**
 * utils/cache.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./cache.h"

// Core libraries.
#include <xalwart.core/utility.h>
#include <xalwart.core/string_utils.h>

// Framework libraries.
#include "./crypto/md5.h"
#include "../http/headers.h"
#include "./http.h"


__UTIL_CACHE_INTERNAL_BEGIN__

bool _if_match_passes(
	const std::string& target_etag, std::vector<std::string>& etags
)
{
	if (target_etag.empty())
	{
		// If there isn't an ETag, then there can't be a match.
		return false;
	}
	else if (etags.size() == 1 && etags.front() == "*")
	{
		// The existence of an ETag means that there is "a current
		// representation for the target resource", even if the ETag is weak,
		// so there is a match to '*'.
		return true;
	}
	else if (target_etag.starts_with("W/"))
	{
		// A weak ETag can never strongly match another ETag.
		return false;
	}
	else
	{
		// Since the ETag is strong, this will only return True
		// if there's a strong match.
		return util::contains(target_etag, etags.begin(), etags.end());
	}
}

bool _if_none_match_passes(
	const std::string& target_etag, std::vector<std::string>& etags
)
{
	if (target_etag.empty())
	{
		// If there isn't an ETag, then there isn't a match.
		return true;
	}
	else if (etags.size() == 1 && etags.front() == "*")
	{
		// The existence of an ETag means that there is "a current
		// representation for the target resource", so there is a match to '*'.
		return false;
	}
	else
	{
		// The comparison should be weak, so look for a match after stripping
		// off any weak indicators.
		auto target_etag_copy = str::trim(target_etag, "W/");
		for (const auto& etag : etags)
		{
			if (str::trim(etag, "W/") == target_etag_copy)
			{
				return false;
			}
		}

		return true;
	}
}

bool _if_modified_since_passes(
	long last_modified, long if_modified_since
)
{
	return last_modified == -1 || last_modified > if_modified_since;
}

bool _if_unmodified_since_passes(
	long last_modified, long if_unmodified_since
)
{
	return last_modified != -1 && last_modified <= if_unmodified_since;
}

std::shared_ptr<http::IHttpResponse> _precondition_failed(http::HttpRequest* request)
{
	return std::make_shared<http::HttpResponse>(412);
}

std::shared_ptr<http::IHttpResponse> _not_modified(
	http::HttpRequest* request, http::IHttpResponse* response
)
{
	auto new_response = std::make_unique<http::HttpResponseNotModified>("");
	if (response)
	{
		auto headers = {
			http::CACHE_CONTROL,
			http::CONTENT_LOCATION,
			http::DATE,
			http::E_TAG,
			http::EXPIRES,
			http::LAST_MODIFIED,
			http::VARY
		};

		// Preserve the headers required by Section 4.1 of RFC 7232,
		// as well as Last-Modified.
		for (const auto& header : headers)
		{
			if (response->has_header(header))
			{
				(*new_response)[header] = (*response)[header];
			}
		}

		// Preserve cookies as per the cookie specification: "If a proxy server
		// receives a response which contains a Set-cookie header, it should
		// propagate the Set-cookie header to the client, regardless of whether
		// the response was 304 (Not Modified) or 200 (OK).
		// https://curl.haxx.se/rfc/cookie_spec.html
		new_response->set_cookies(response->get_cookies());
	}

	return new_response;
}

__UTIL_CACHE_INTERNAL_END__


__UTIL_CACHE_BEGIN__

void set_response_etag(http::IHttpResponse* response)
{
	if (!response->is_streaming() && response->content_length() > 0)
	{
		response->set_header(
			http::E_TAG,
			utils_http::quote_etag(crypto::MD5(response->get_content()).hex_digest())
		);
	}
}

std::shared_ptr<http::IHttpResponse> get_conditional_response(
	http::HttpRequest* request,
	const std::string& etag,
	long last_modified,
	http::IHttpResponse* response
)
{
	// Only return conditional responses on successful requests.
	if (response && !(response->status() >= 200 && response->status() < 300))
	{
		return nullptr;
	}

	// Get HTTP request headers.
	auto if_match_etags = utils_http::parse_etags(
		request->headers.get(http::IF_MATCH, "")
	);
	long if_unmodified_since = utils_http::parse_http_date(
		request->headers.get(http::IF_UNMODIFIED_SINCE, "")
	);
	auto if_none_match_etags = utils_http::parse_etags(
		request->headers.get(http::IF_NONE_MATCH, "")
	);
	long if_modified_since = utils_http::parse_http_date(
		request->headers.get(http::IF_MODIFIED_SINCE)
	);

	// Step 1 of section 6 of RFC 7232: Test the If-Match precondition.
	if (!if_match_etags.empty() && !internal::_if_match_passes(etag, if_match_etags))
	{
		return internal::_precondition_failed(request);
	}

	// Step 2: Test the If-Unmodified-Since precondition.
	if (
		if_match_etags.empty() &&
		if_unmodified_since != -1 &&
		!internal::_if_unmodified_since_passes(last_modified, if_unmodified_since)
	)
	{
		return internal::_precondition_failed(request);
	}

	// Step 3: Test the If-None-Match precondition.
	if (!if_none_match_etags.empty() && !internal::_if_none_match_passes(etag, if_none_match_etags))
	{
		if (request->method() == "GET" || request->method() == "HEAD")
		{
			return internal::_not_modified(request, response);
		}
		else
		{
			return internal::_precondition_failed(request);
		}
	}

	// Step 4: Test the If-Modified-Since precondition.
	if (
		if_none_match_etags.empty() &&
		if_modified_since != -1 &&
		!internal::_if_modified_since_passes(last_modified, if_modified_since)
	)
	{
		if (request->method() == "GET" || request->method() == "HEAD")
		{
			return internal::_not_modified(request, response);
		}
	}

	// Step 5: Test the If-Range precondition (not supported).
	// Step 6: Return nullptr since there isn't a conditional response.
	return nullptr;
}

__UTIL_CACHE_END__
