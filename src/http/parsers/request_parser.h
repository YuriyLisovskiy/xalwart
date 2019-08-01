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

/*
 * http_parser definition.
 * TODO: write docs.
 */

#ifndef WASP_HTTP_PARSERS_REQUEST_PARSER_H
#define WASP_HTTP_PARSERS_REQUEST_PARSER_H

#include <algorithm>
#include <string>
#include <cstdlib>
#include <strings.h>
#include <map>
#include <iostream>

#include "../../globals.h"
#include "../request.h"


__INTERNAL_BEGIN__

class HttpRequestParser
{
protected:
	size_t _majorV{};
	size_t _minorV{};
	std::string _path;
	std::string _query;
	std::string _method;
	bool _keepAlive{};
	std::string _content;
	std::map<std::string, std::string> _headers;
	std::map<std::string, std::string> _getParameters;
	std::map<std::string, std::string> _postParameters;

	unsigned long long _contentSize{};
	std::string _chunkSizeStr;
	unsigned long long _chunkSize{};
	bool _chunked{};

	// Used only for POST, PUT or PATCH methods type
	enum ContentType
	{
		ApplicationXWwwFormUrlencoded,
		ApplicationJson,
		MultipartFormData,
		Other

	} _contentType{};

	wasp::HttpRequest _buildHttpRequest();

	enum ParserState
	{
		MethodBegin,
		Method,
		PathBegin,
		Path,
		Query,
		Fragment,
		HttpVersionH,
		HttpVersionHt,
		HttpVersionHtt,
		HttpVersionHttp,
		HttpVersionSlash,
		HttpVersionMajorBegin,
		HttpVersionMajor,
		HttpVersionMinorBegin,
		HttpVersionMinor,
		HttpVersionNewLine,

		HeaderLineStart,
		HeaderLws,
		HeaderName,
		HeaderSpaceBeforeValue,
		HeaderValue,

		ExpectingNewline_2,
		ExpectingNewline_3,

		RequestBody,

		ChunkSize,
		ChunkExtensionName,
		ChunkExtensionValue,
		ChunkSizeNewLine,
		ChunkSizeNewLine_2,
		ChunkSizeNewLine_3,
		ChunkTrailerName,
		ChunkTrailerValue,

		ChunkDataNewLine_1,
		ChunkDataNewLine_2,
		ChunkData

	} _state{};

	enum QueryParserState
	{
		Key,
		Val
	};

	enum MultipartParserState
	{
		BoundaryBegin,
		Boundary,
		BoundaryEnd,
		ContentDispositionBegin,
		ContentDisposition,
		NameBegin,
		Name,
		FileNameBegin,
		FileName,
		ContentTypeBegin,
		ContentType,
		ContentBegin,
		Content,
	};

	void _parseHttpWord(char input, char expectedInput, HttpRequestParser::ParserState newState);

	// Parses http request stream
	void _parseRequest(const std::string& data);

	// Parses 'application/json' content type
	void _parseApplicationJson();

	// Parses 'multipart/form-data' content type
	void _parseMultipart();

	// Parses url's query or 'application/x-www-form-urlencoded' content type
	static std::map<std::string, std::string>* _parseQuery(const std::string& content);

	void _setParameters(std::map<std::string, std::string>* params);

	// Helpers
	// Check if a byte is an HTTP character.
	static bool isChar(uint c);

	// Check if a byte is an HTTP control character.
	static bool isControl(uint c);

	// Check if a byte is defined as an HTTP special character.
	static bool isSpecial(uint c);

	// Check if a byte is a digit.
	static bool isDigit(uint c);

	// May be overloaded for custom parser which is inherited from HttpRequestParser
	virtual void setParameters(std::map<std::string, std::string>* params);

public:
	HttpRequestParser() = default;
	wasp::HttpRequest parse(const std::string& data);
};

__INTERNAL_END__


#endif // WASP_HTTP_PARSERS_REQUEST_PARSER_H
