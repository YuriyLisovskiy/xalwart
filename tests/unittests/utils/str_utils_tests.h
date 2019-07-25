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

#ifndef WASP_UNIT_TESTS_UTILS_STR_TESTS_H
#define WASP_UNIT_TESTS_UTILS_STR_TESTS_H

#include <gtest/gtest.h>

#include "../globals.h"
#include "../../../src/utils/str.h"


__UNIT_TESTS_BEGIN__

TEST(StrUtilsUrlSplitTypeTestCase, TestSuccess)
{
	std::string schemeActual, dataActual;
	wasp::str::urlSplitType("http://example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "http");
	ASSERT_EQ(dataActual, "//example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestMissingColon)
{
	std::string schemeActual, dataActual;
	wasp::str::urlSplitType("http//example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "http//example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestSlashBeforeColon)
{
	std::string schemeActual, dataActual;
	wasp::str::urlSplitType("http/://example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "http/://example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestEmptyInput)
{
	std::string schemeActual, dataActual;
	wasp::str::urlSplitType("", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "");
}

__UNIT_TESTS_END__


#endif // WASP_UNIT_TESTS_UTILS_STR_TESTS_H