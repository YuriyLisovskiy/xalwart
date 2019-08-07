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
 * encoding definition.
 * TODO: write docs.
 */

#ifndef WASP_UTILS_ENCODING_H
#define WASP_UTILS_ENCODING_H

#include <string>
#include <sstream>
#include <iomanip>

#include "../globals.h"


__INTERNAL_BEGIN__

// Converts character to percent-encoded character and writes it to stream.
//
// @safe - string which contains chars that must not be converted. If char 'c'
// is in this sequence, it will be written to stream without converting.
//
// RFC 3986 section 2.2 Reserved Characters: ! * ' ( ) ; : @ & = + $ , / ? # [ ]
// RFC 3986 section 2.3 Unreserved Characters: ALPHA NUMERIC - _ . ~
extern void escape(std::ostringstream& stream, char c, const std::string& safe = "");

__INTERNAL_END__


__WASP_BEGIN__

// Encodes url using percent-encoding.
extern std::string encodeUrl(const std::string& url);

__WASP_END__


#endif // WASP_UTILS_ENCODING_H