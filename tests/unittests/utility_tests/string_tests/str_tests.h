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

#ifndef WASP_UNIT_TESTS_UTILITY_TESTS_STRING_TESTS_STR_TESTS_H
#define WASP_UNIT_TESTS_UTILITY_TESTS_STRING_TESTS_STR_TESTS_H

#include <gtest/gtest.h>

#include "../../globals.h"
#include "../../../../src/utility/string/str.h"


__UNIT_TESTS_BEGIN__

TEST(StrUtilsUrlSplitTypeTestCase, TestSuccess)
{
	std::string schemeActual, dataActual;
	wasp::str::url_split_type("http://example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "http");
	ASSERT_EQ(dataActual, "//example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestMissingColon)
{
	std::string schemeActual, dataActual;
	wasp::str::url_split_type("http//example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "http//example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestSlashBeforeColon)
{
	std::string schemeActual, dataActual;
	wasp::str::url_split_type("http/://example.com/file.html", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "http/://example.com/file.html");
}

TEST(StrUtilsUrlSplitTypeTestCase, TestEmptyInput)
{
	std::string schemeActual, dataActual;
	wasp::str::url_split_type("", schemeActual, dataActual);
	ASSERT_EQ(schemeActual, "");
	ASSERT_EQ(dataActual, "");
}


TEST(StrUtilsSplitTestCase, TestSplitByDefaultChar)
{
	std::string toSplit("Alphanumeric and printable shellcode");
	std::vector<std::string> expected = {"Alphanumeric", "and", "printable", "shellcode"};
	auto actual = wasp::str::split(toSplit);
	ASSERT_EQ(expected.size(), actual.size());
	for (size_t i = 0; i < expected.size(); i++)
	{
		ASSERT_EQ(expected[i], actual[i]);
	}
}

TEST(StrUtilsSplitTestCase, TestSplit)
{
	std::string toSplit("Alphanumeric:and: printable : shellcode");
	std::vector<std::string> expected = {"Alphanumeric", "and", " printable ", " shellcode"};
	auto actual = wasp::str::split(toSplit, ':');
	ASSERT_EQ(expected.size(), actual.size());
	for (size_t i = 0; i < expected.size(); i++)
	{
		ASSERT_EQ(expected[i], actual[i]);
	}
}


TEST(StrUtilsJoinVectorTestCase, TestJoinStringVectorWithSpace)
{
	std::string expected = "Alphanumeric and printable shellcode";
	std::vector<std::string> strVec = {"Alphanumeric", "and", "printable", "shellcode"};
	ASSERT_EQ(wasp::str::join(strVec.cbegin(), strVec.cend(), " "), expected);
}

TEST(StrUtilsJoinVectorTestCase, TestJoinIntVectorWithComma)
{
	std::string expected = "{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}";
	std::vector<int> intVec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	ASSERT_EQ("{" + wasp::str::join(intVec.cbegin(), intVec.cend(), ", ") + "}", expected);
}

TEST(StrUtilsJoinVectorTestCase, TestJoinCharVectorWithputSpace)
{
	std::string expected = "Alphanumeric";
	std::vector<char> intVec = {'A', 'l', 'p', 'h', 'a', 'n', 'u', 'm', 'e', 'r', 'i', 'c'};
	ASSERT_EQ(wasp::str::join(intVec.cbegin(), intVec.cend(), ""), expected);
}


TEST(StrUtilsLTrimTestCase, TestLeftTrimmingInPlace)
{
	std::string expected = "Hello, World   ";
	std::string actual = "       " + expected;
	wasp::str::ltrim(actual);
	ASSERT_EQ(actual, expected);

	expected = "Hello, World  - ---";
	actual = "------" + expected;
	wasp::str::ltrim(actual, '-');
	ASSERT_EQ(actual, expected);
}

TEST(StrUtilsRTrimTestCase, TestRightTrimmingInPlace)
{
	std::string expected = "      Hello, World";
	std::string actual = expected + "       ";
	wasp::str::rtrim(actual);
	ASSERT_EQ(actual, expected);

	expected = "--------Hello, World";
	actual = expected + "------";
	wasp::str::rtrim(actual, '-');
	ASSERT_EQ(actual, expected);
}

TEST(StrUtilsTrimTestCase, TestTrimmingInPlace)
{
	std::string expected = "Hello, World      --- - ---";
	std::string actual = "       " + expected + "      ";
	wasp::str::trim(actual);
	ASSERT_EQ(actual, expected);

	expected = "Hello, World";
	actual = "------" + expected + "----";
	wasp::str::trim(actual, '-');
	ASSERT_EQ(actual, expected);
}

TEST(StrUtilsLTrimTestCase, TestLeftTrimmingCopy)
{
	std::string expected = "Hello, World   ";
	ASSERT_EQ(wasp::str::ltrim("       " + expected), expected);

	expected = "Hello, World  - ---";
	ASSERT_EQ(wasp::str::ltrim("------" + expected, '-'), expected);
}

TEST(StrUtilsRTrimTestCase, TestRightTrimmingCopy)
{
	std::string expected = "      Hello, World";
	ASSERT_EQ(wasp::str::rtrim(expected + "       "), expected);

	expected = "--------Hello, World";
	ASSERT_EQ(wasp::str::rtrim(expected + "------", '-'), expected);
}

TEST(StrUtilsTrimTestCase, TestTrimmingCopy)
{
	std::string expected = "Hello, World      --- - ---";
	ASSERT_EQ(wasp::str::trim("       " + expected + "      "), expected);

	expected = "Hello, World";
	ASSERT_EQ(wasp::str::trim("------" + expected + "----", '-'), expected);
}


TEST(StrUpperTestCase, TestUpper)
{
	std::string expected = "HELLO, WORLD";
	std::string actual = wasp::str::upper("Hello, world");

	ASSERT_EQ(expected, actual);
}


TEST(StrLowerTestCase, TestLower)
{
	std::string expected = "hello, world";
	std::string actual = wasp::str::lower("HeLlO, WoRlD");

	ASSERT_EQ(expected, actual);
}


TEST(StrStartsWithTestCase, TestStartsWithPrefixIsGreaterThanStr)
{
	ASSERT_FALSE(wasp::str::starts_with("Hello", "Hello,"));
}

TEST(StrStartsWithTestCase, TestStartsWithReturnsFalse)
{
	ASSERT_FALSE(wasp::str::starts_with("Hello, World", "hello,"));
}

TEST(StrStartsWithTestCase, TestStartsWithReturnsTrue)
{
	ASSERT_TRUE(wasp::str::starts_with("Hello, World", "Hello,"));
}

__UNIT_TESTS_END__


#endif // WASP_UNIT_TESTS_UTILITY_TESTS_STRING_TESTS_STR_TESTS_H