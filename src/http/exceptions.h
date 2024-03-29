/**
 * http/exceptions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * List of http exceptions:
 *  - HttpError
 *  - EntityTooLarge
 *  - DisallowedHost
 *  - DisallowedRedirect
 *  - FileDoesNotExist
 *  - InternalServerError
 *  - NotFound
 *  - PermissionDenied
 *  - RequestTimeout
 *  - SuspiciousOperation
 */

#pragma once

// Base libraries.
#include <xalwart.base/exceptions.h>

// Module definitions.
#include "./_def_.h"


__HTTP_EXC_BEGIN__

// TESTME: HttpError
// TODO: docs for 'HttpError'
class HttpError : public BaseException
{
private:
	unsigned short int _status_code;

protected:
	// Use only when initializing of a derived exception!
	inline HttpError(
		unsigned short int status_code, const std::string& message,
		int line, const char* function, const char* file, const char* type
	) : BaseException(message.c_str(), line, function, file, type), _status_code(status_code)
	{
	}

public:
	inline HttpError(
		unsigned short int status_code, const std::string& message,
		int line=0, const char* function="", const char* file=""
	) : HttpError(status_code, message, line, function, file, "xw::http::exc::HttpError")
	{
	}

	[[nodiscard]]
	inline unsigned short int status_code() const
	{
		return this->_status_code;
	}

	// Returns error message with exception type as `std::string`.
	[[nodiscard]]
	inline std::string get_message() const noexcept override
	{
		return this->_exception_type + " [" + std::to_string(this->status_code()) + "]: " + this->_message;
	}
};

// TESTME: EntityTooLarge
// TODO: docs for 'EntityTooLarge
class EntityTooLarge : public HttpError
{
public:
	inline explicit EntityTooLarge(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(413, message, line, function, file, "xw::http::exc::EntityTooLarge")
	{
	}
};

// TESTME: DisallowedHostException
// TODO: docs for 'DisallowedHostException'
class DisallowedHost : public HttpError
{
public:
	inline explicit DisallowedHost(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(400, message, line, function, file, "xw::http::exc::DisallowedHost")
	{
	}
};

// TESTME: DisallowedRedirect
// TODO: docs for 'DisallowedRedirect'
class DisallowedRedirect : public HttpError
{
public:
	inline explicit DisallowedRedirect(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(400, message, line, function, file, "xw::http::exc::DisallowedRedirect")
	{
	}
};

// TESTME: FileDoesNotExist
// TODO: docs for 'FileDoesNotExist'
class FileDoesNotExist : public HttpError
{
public:
	inline explicit FileDoesNotExist(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(404, message, line, function, file, "xw::http::exc::FileDoesNotExist")
	{
	}
};

// TESTME: InternalServerError
// TODO: docs for 'InternalServerError'
class InternalServerError : public HttpError
{
public:
	inline explicit InternalServerError(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(500, message, line, function, file, "xw::http::exc::InternalServerError")
	{
	}
};

// TESTME: NotFound
// TODO: docs for 'NotFound'
class NotFound : public HttpError
{
public:
	inline explicit NotFound(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(404, message, line, function, file, "xw::http::exc::NotFound")
	{
	}
};

// TESTME: BadRequest
// TODO: docs for 'BadRequest'
class BadRequest : public HttpError
{
public:
	inline explicit BadRequest(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(400, message, line, function, file, "xw::http::exc::BadRequest")
	{
	}
};

// TESTME: PermissionDenied
// TODO: docs for 'PermissionDenied'
class PermissionDenied : public HttpError
{
public:
	inline explicit PermissionDenied(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(403, message, line, function, file, "xw::http::exc::PermissionDenied")
	{
	}
};

// TESTME: RequestTimeout
// TODO: docs for 'RequestTimeout'
class RequestTimeout : public HttpError
{
public:
	inline explicit RequestTimeout(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(408, message, line, function, file, "xw::http::exc::RequestTimeout")
	{
	}
};

// TESTME: SuspiciousOperation
// TODO: docs for 'SuspiciousOperation'
class SuspiciousOperation : public HttpError
{
public:
	inline explicit SuspiciousOperation(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(400, message, line, function, file, "xw::http::exc::SuspiciousOperation")
	{
	}
};

// TESTME: PayloadTooLarge
// TODO: docs for 'PayloadTooLarge'
class PayloadTooLarge : public HttpError
{
public:
	inline explicit PayloadTooLarge(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(413, message, line, function, file, "xw::http::exc::PayloadTooLarge")
	{
	}
};

// TESTME: NotMultipart
// TODO: docs for 'NotMultipart'
class NotMultipart : public ParseError
{
protected:
	// Use only when initializing of a derived exception!
	inline NotMultipart(
		const std::string& message, int line, const char* function, const char* file, const char* type
	) : ParseError(message.c_str(), line, function, file, type)
	{
	}

public:
	inline explicit NotMultipart(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : NotMultipart(message, line, function, file, "xw::http::exc::NotMultipart")
	{
	}
};

// TESTME: RequestHeaderFieldsTooLarge
// TODO: docs for 'RequestHeaderFieldsTooLarge'
class RequestHeaderFieldsTooLarge : public HttpError
{
public:
	inline explicit RequestHeaderFieldsTooLarge(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : HttpError(431, message, line, function, file, "xw::http::exc::RequestHeaderFieldsTooLarge")
	{
	}
};

__HTTP_EXC_END__
